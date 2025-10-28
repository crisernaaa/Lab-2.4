/*
 * src/gui_ncurses.c
 *
 * Terminal UI using ncurses for the CPU Scheduler Simulator.
 *
 * Controls:
 *   r - Run simulation
 *   t - Cycle algorithm (FIFO -> SJF -> STCF -> RR -> MLFQ)
 *   q - Quit
 *   a - Add process (interactive)
 *   d - Delete process by PID (interactive)
 *   s - Save workload to file (interactive)
 *   l - Load workload from file (interactive)
 *   + / - - increase / decrease quantum (for RR)
 *
 * Note: Gantt is simplified: for preemptive algorithms it draws from first start to completion.
 * To show exact preemptive segments, algorithms must populate timeline_event_t consistently per time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>

#include "../include/scheduler.h"
#include "../include/algorithms.h"
#include "../include/metrics.h"

#define MAX_PROCESSES 128
#define MAX_TIMELINE  10000

/* Local copy of processes */
static process_t processes[MAX_PROCESSES];
static int proc_count = 0;

/* timeline events produced by scheduler (we will build a simplified one) */
static timeline_event_t timeline[MAX_TIMELINE];
static int timeline_len = 0;

/* Metrics */
static metrics_t last_metrics;

/* Scheduler selection */
typedef enum { ALG_FIFO=0, ALG_SJF, ALG_STCF, ALG_RR, ALG_MLFQ, ALG_COUNT } alg_t;
static alg_t curr_alg = ALG_FIFO;
static int rr_quantum = 3;

/* MLFQ default config (simplified) */
static int mlfq_num_queues = 3;
static int mlfq_quantums_default[3] = {2, 4, 8};
static mlfq_config_t mlfq_config;

/* Helpers */
static void draw_borders();
static void draw_process_table(int starty, int startx, int h, int w);
static void draw_controls(int starty, int startx);
static void draw_gantt(int starty, int startx, int h, int w);
static void draw_metrics(int starty, int startx, int h, int w);
static void run_selected_scheduler();
static void clear_timeline();
static int get_max_completion();
static void save_workload(const char *filename);
static int load_workload(const char *filename);
static int prompt_number(const char *prompt, int minv, int maxv);
static void prompt_string(const char *prompt, char *buf, int maxlen);

/* Utility: find process index by pid */
static int find_proc_index_by_pid(int pid) {
    for (int i = 0; i < proc_count; ++i)
        if (processes[i].pid == pid) return i;
    return -1;
}

/* Draw UI frame */
static void draw_ui() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    clear();
    draw_borders();

    int left_w = cols * 55 / 100;
    int right_w = cols - left_w - 3;
    int header_h = 3;

    draw_process_table(1, 1, rows - header_h - 2, left_w);
    draw_controls(1, left_w + 2);
    draw_gantt(header_h + 1, left_w + 2, rows/2 - 2, right_w);
    draw_metrics(header_h + rows/2 - 1, left_w + 2, rows - (header_h + rows/2 - 1) - 2, right_w);

    mvprintw(rows-1, 1, "r:Run  t:ChangeAlg  a:Add  d:Delete  s:Save  l:Load  +/-:Quantum  q:Quit");
    refresh();
}

/* Draw box around main UI */
static void draw_borders() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    box(stdscr, 0, 0);
    mvprintw(0, 2, " CPU Scheduler Simulator (ncurses) ");
}

/* Process table */
static void draw_process_table(int starty, int startx, int h, int w) {
    WINDOW *win = newwin(h, w, starty, startx);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Processes (%d) ", proc_count);
    mvwprintw(win, 1, 1, " PID | Arrival | Burst | Pri | Rem | Start | Complete ");
    int row = 2;
    for (int i = 0; i < proc_count && row < h-1; ++i, ++row) {
        process_t *p = &processes[i];
        mvwprintw(win, row, 1, " %3d | %7d | %5d | %3d | %3d | %5d | %8d ",
                  p->pid, p->arrival_time, p->burst_time, p->priority,
                  p->remaining_time, p->start_time, p->completion_time);
    }
    wrefresh(win);
    delwin(win);
}

/* Controls: show selected algorithm and quantum */
static void draw_controls(int starty, int startx) {
    int y = starty;
    mvprintw(y++, startx, "Algorithm:");
    const char *names[ALG_COUNT] = { "FIFO", "SJF", "STCF", "RR", "MLFQ" };
    for (int i = 0; i < ALG_COUNT; ++i) {
        if (i == curr_alg) attron(A_REVERSE);
        mvprintw(y++, startx, " %s", names[i]);
        if (i == curr_alg) attroff(A_REVERSE);
    }
    mvprintw(y++, startx, "Quantum (RR): %d", rr_quantum);
    mvprintw(y++, startx, "MLFQ queues: %d", mlfq_num_queues);
    mvprintw(y++, startx, "MLFQ quantums: ");
    for (int i = 0; i < mlfq_num_queues; ++i) {
        printw("%d ", mlfq_config.quantums[i]);
    }
}

/* Build simplified timeline from process start & completion */
static void build_simplified_timeline_from_processes() {
    clear_timeline();
    int idx = 0;
    for (int i = 0; i < proc_count; ++i) {
        process_t *p = &processes[i];
        if (p->completion_time <= p->start_time) continue;
        timeline[idx].time = p->start_time;
        timeline[idx].pid = p->pid;
        timeline[idx].duration = p->completion_time - p->start_time;
        idx++;
        if (idx >= MAX_TIMELINE) break;
    }
    timeline_len = idx;
}

/* Draw a very simple Gantt chart */
static void draw_gantt(int starty, int startx, int h, int w) {
    WINDOW *win = newwin(h, w, starty, startx);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Gantt Chart ");

    if (timeline_len == 0) {
        mvwprintw(win, 2, 2, "No timeline. Run the scheduler (r).");
        wrefresh(win);
        delwin(win);
        return;
    }

    /* Determine span */
    int min_t = timeline[0].time;
    int max_t = timeline[0].time + timeline[0].duration;
    for (int i = 1; i < timeline_len; ++i) {
        if (timeline[i].time < min_t) min_t = timeline[i].time;
        int endt = timeline[i].time + timeline[i].duration;
        if (endt > max_t) max_t = endt;
    }
    int span = max_t - min_t;
    if (span <= 0) span = 1;

    int gantt_y = 2;
    int gantt_h = 3;
    int gantt_w = w - 4;
    int px_per_unit = gantt_w / span;
    if (px_per_unit < 1) px_per_unit = 1;

    /* Draw time scale */
    for (int i = 0; i < timeline_len; ++i) {
        int rel = timeline[i].time - min_t;
        int x = 2 + rel * px_per_unit;
        int block_w = timeline[i].duration * px_per_unit;
        if (x + block_w > 2 + gantt_w) block_w = 2 + gantt_w - x;
        if (block_w <= 0) continue;
        /* show pid label centered */
        char label[16];
        snprintf(label, sizeof(label), "P%d", timeline[i].pid);
        int label_pos = x + block_w/2 - (int)strlen(label)/2;
        if (label_pos < 2) label_pos = 2;
        for (int c = 0; c < block_w; ++c) {
            mvwaddch(win, gantt_y, x + c, ACS_CKBOARD);
        }
        mvwprintw(win, gantt_y+1, label_pos, "%s", label);

        /* times below */
        mvwprintw(win, gantt_y+2, x, "%d", timeline[i].time);
    }
    /* draw final maximum time at end */
    mvwprintw(win, gantt_y+2, 2 + gantt_w - 4, "%d", max_t);

    wrefresh(win);
    delwin(win);
}

/* Metrics display */
static void draw_metrics(int starty, int startx, int h, int w) {
    WINDOW *win = newwin(h, w, starty, startx);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Metrics ");

    if (proc_count == 0) {
        mvwprintw(win, 2, 2, "No processes.");
        wrefresh(win);
        delwin(win);
        return;
    }

    mvwprintw(win, 2, 2, "Avg Turnaround: %.2f", last_metrics.avg_turnaround_time);
    mvwprintw(win, 3, 2, "Avg Waiting:    %.2f", last_metrics.avg_waiting_time);
    mvwprintw(win, 4, 2, "Avg Response:   %.2f", last_metrics.avg_response_time);
    mvwprintw(win, 5, 2, "CPU Utilization: %.2f %%", last_metrics.cpu_utilization);
    mvwprintw(win, 6, 2, "Throughput:     %.4f p/u", last_metrics.throughput);
    mvwprintw(win, 7, 2, "Fairness (Jain): %.4f", last_metrics.fairness_index);

    wrefresh(win);
    delwin(win);
}

/* Clear timeline events */
static void clear_timeline() {
    for (int i = 0; i < MAX_TIMELINE; ++i) {
        timeline[i].time = 0;
        timeline[i].pid = -1;
        timeline[i].duration = 0;
    }
    timeline_len = 0;
}

/* Run scheduler and compute metrics */
static void run_selected_scheduler() {
    if (proc_count == 0) return;

    /* copy processes into a temporary array the schedulers can modify */
    process_t temp[MAX_PROCESSES];
    for (int i = 0; i < proc_count; ++i) temp[i] = processes[i];

    clear_timeline();
    /* zero some fields */
    for (int i = 0; i < proc_count; ++i) {
        temp[i].remaining_time = temp[i].burst_time;
        temp[i].start_time = -1;
        temp[i].completion_time = -1;
        temp[i].turnaround_time = 0;
        temp[i].waiting_time = 0;
        temp[i].response_time = -1;
    }

    /* Call chosen algorithm */
    switch (curr_alg) {
        case ALG_FIFO:
            schedule_fifo(temp, proc_count, timeline);
            break;
        case ALG_SJF:
            schedule_sjf(temp, proc_count, timeline);
            break;
        case ALG_STCF:
            schedule_stcf(temp, proc_count, timeline);
            break;
        case ALG_RR:
            schedule_rr(temp, proc_count, rr_quantum, timeline);
            break;
        case ALG_MLFQ:
            mlfq_config.num_queues = mlfq_num_queues;
            mlfq_config.quantums = mlfq_quantums_default; /* simplified */
            mlfq_config.boost_interval = 50;
            schedule_mlfq(temp, proc_count, &mlfq_config, timeline);
            break;
        default:
            break;
    }

    /* After scheduling, compute total_time: use max completion_time if available */
    int total_time = 0;
    for (int i = 0; i < proc_count; ++i) {
        if (temp[i].completion_time > total_time) total_time = temp[i].completion_time;
        if (temp[i].completion_time <= 0) {
            /* fallback: sum bursts */
            total_time += temp[i].burst_time;
        }
    }
    if (total_time <= 0) total_time = 1;

    /* Calculate metrics */
    calculate_metrics(temp, proc_count, total_time, &last_metrics);

    /* Copy back some fields to main processes array for display (start/completion) */
    for (int i = 0; i < proc_count; ++i) {
        int pid = temp[i].pid;
        int idx = find_proc_index_by_pid(pid);
        if (idx >= 0) {
            processes[idx].start_time = temp[i].start_time;
            processes[idx].completion_time = temp[i].completion_time;
            processes[idx].remaining_time = temp[i].remaining_time;
            processes[idx].turnaround_time = temp[i].turnaround_time;
            processes[idx].waiting_time = temp[i].waiting_time;
            processes[idx].response_time = temp[i].response_time;
        }
    }

    /* Build simplified timeline from start/completion */
    build_simplified_timeline_from_processes();
}

/* Prompt helpers (blocking) */
static int prompt_number(const char *prompt, int minv, int maxv) {
    echo();
    curs_set(1);
    char buf[64];
    mvprintw(LINES-3, 2, "%s ", prompt);
    clrtoeol();
    getnstr(buf, sizeof(buf)-1);
    int val = atoi(buf);
    if (val < minv) val = minv;
    if (maxv >= minv && val > maxv) val = maxv;
    noecho();
    curs_set(0);
    return val;
}

static void prompt_string(const char *prompt, char *buf, int maxlen) {
    echo();
    curs_set(1);
    mvprintw(LINES-3, 2, "%s ", prompt);
    clrtoeol();
    getnstr(buf, maxlen-1);
    noecho();
    curs_set(0);
}

/* Save workload: one process per line: pid arrival burst priority */
static void save_workload(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        mvprintw(LINES-4, 2, "Error saving to '%s'", filename);
        return;
    }
    for (int i = 0; i < proc_count; ++i) {
        process_t *p = &processes[i];
        fprintf(f, "%d %d %d %d\n", p->pid, p->arrival_time, p->burst_time, p->priority);
    }
    fclose(f);
    mvprintw(LINES-4, 2, "Saved to '%s' (%d processes)", filename, proc_count);
}

/* Load workload: returns number loaded or -1 on error */
static int load_workload(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        mvprintw(LINES-4, 2, "Error opening '%s'", filename);
        return -1;
    }
    proc_count = 0;
    while (!feof(f) && proc_count < MAX_PROCESSES) {
        int pid, arrival, burst, priority;
        int ret = fscanf(f, "%d %d %d %d", &pid, &arrival, &burst, &priority);
        if (ret != 4) break;
        process_t p;
        p.pid = pid;
        p.arrival_time = arrival;
        p.burst_time = burst;
        p.priority = priority;
        p.remaining_time = burst;
        p.start_time = -1;
        p.completion_time = -1;
        processes[proc_count++] = p;
    }
    fclose(f);
    mvprintw(LINES-4, 2, "Loaded %d processes from '%s'    ", proc_count, filename);
    return proc_count;
}

/* Interactive add process */
static void add_process_interactive() {
    int pid = prompt_number("PID:", 1, 999999);
    int arrival = prompt_number("Arrival time:", 0, 1000000);
    int burst = prompt_number("Burst time:", 1, 1000000);
    int priority = prompt_number("Priority (lower=more):", 0, 1000);

    if (proc_count >= MAX_PROCESSES) {
        mvprintw(LINES-4, 2, "Max processes reached.");
        return;
    }
    processes[proc_count].pid = pid;
    processes[proc_count].arrival_time = arrival;
    processes[proc_count].burst_time = burst;
    processes[proc_count].priority = priority;
    processes[proc_count].remaining_time = burst;
    processes[proc_count].start_time = -1;
    processes[proc_count].completion_time = -1;
    proc_count++;
}

/* Interactive delete by PID */
static void delete_process_interactive() {
    int pid = prompt_number("Delete PID:", 1, 999999);
    int idx = find_proc_index_by_pid(pid);
    if (idx < 0) {
        mvprintw(LINES-4, 2, "PID %d not found.", pid);
        return;
    }
    for (int i = idx; i < proc_count-1; ++i) processes[i] = processes[i+1];
    proc_count--;
    mvprintw(LINES-4, 2, "Deleted PID %d.", pid);
}

/* Main loop */
int main(int argc, char **argv) {
    /* Initialize MLFQ config array */
    mlfq_config.num_queues = mlfq_num_queues;
    mlfq_config.quantums = mlfq_quantums_default;
    mlfq_config.boost_interval = 50;

    /* Example initial processes (if none loaded) */
    processes[0].pid = 1; processes[0].arrival_time = 0; processes[0].burst_time = 5; processes[0].priority = 1;
    processes[1].pid = 2; processes[1].arrival_time = 1; processes[1].burst_time = 3; processes[1].priority = 2;
    processes[2].pid = 3; processes[2].arrival_time = 2; processes[2].burst_time = 8; processes[2].priority = 1;
    proc_count = 3;

    /* Init ncurses */
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    clear_timeline();

    draw_ui();

    int ch;
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case 'r':
                run_selected_scheduler();
                break;
            case 't':
                curr_alg = (curr_alg + 1) % ALG_COUNT;
                break;
            case 'a':
                add_process_interactive();
                break;
            case 'd':
                delete_process_interactive();
                break;
            case 's': {
                char fname[256];
                prompt_string("Save as filename:", fname, sizeof(fname));
                if (strlen(fname) > 0) save_workload(fname);
                break;
            }
            case 'l': {
                char fname[256];
                prompt_string("Load filename:", fname, sizeof(fname));
                if (strlen(fname) > 0) load_workload(fname);
                break;
            }
            case '+':
            case '=':
                rr_quantum++;
                break;
            case '-':
                if (rr_quantum > 1) rr_quantum--;
                break;
            case KEY_RESIZE:
                break;
            default:
                break;
        }
        draw_ui();
    }

    endwin();
    return 0;
}
