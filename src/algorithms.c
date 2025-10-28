#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "algorithms.h"

// -----------------------------
// FIFO (First In First Out)
// -----------------------------
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline) {
    int time = 0;
    for (int i = 0; i < n; i++) {
        process_t *p = &processes[i];
        if (time < p->arrival_time)
            time = p->arrival_time;
        p->start_time = time;
        p->completion_time = time + p->burst_time;
        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->turnaround_time - p->burst_time;
        p->response_time = p->start_time - p->arrival_time;

        timeline[i].time = p->start_time;
        timeline[i].pid = p->pid;
        timeline[i].duration = p->burst_time;

        time += p->burst_time;
    }
}

// -----------------------------
// SJF (Shortest Job First)
// -----------------------------
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline) {
    int completed = 0, time = 0;
    int used[n];
    for (int i = 0; i < n; i++) used[i] = 0;

    while (completed < n) {
        int idx = -1, min_burst = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (!used[i] && processes[i].arrival_time <= time) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        process_t *p = &processes[idx];
        p->start_time = time;
        p->completion_time = time + p->burst_time;
        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->turnaround_time - p->burst_time;
        p->response_time = p->start_time - p->arrival_time;

        timeline[completed].time = p->start_time;
        timeline[completed].pid = p->pid;
        timeline[completed].duration = p->burst_time;

        time += p->burst_time;
        used[idx] = 1;
        completed++;
    }
}

// -----------------------------
// STCF (Shortest Time to Completion First)
// -----------------------------
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline) {
    int time = 0, completed = 0;
    for (int i = 0; i < n; i++)
        processes[i].remaining_time = processes[i].burst_time;

    while (completed < n) {
        int idx = -1, min_remaining = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_remaining) {
                    min_remaining = processes[i].remaining_time;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        process_t *p = &processes[idx];
        if (p->remaining_time == p->burst_time)
            p->start_time = time;

        p->remaining_time--;
        timeline[time].time = time;
        timeline[time].pid = p->pid;
        timeline[time].duration = 1;

        time++;
        if (p->remaining_time == 0) {
            p->completion_time = time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->response_time = p->start_time - p->arrival_time;
            completed++;
        }
    }
}

// -----------------------------
// Round Robin
// -----------------------------
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline) {
    int time = 0, completed = 0;
    for (int i = 0; i < n; i++)
        processes[i].remaining_time = processes[i].burst_time;

    while (completed < n) {
        int idle = 1;
        for (int i = 0; i < n; i++) {
            process_t *p = &processes[i];
            if (p->arrival_time <= time && p->remaining_time > 0) {
                idle = 0;
                if (p->remaining_time == p->burst_time)
                    p->start_time = time;

                int exec_time = (p->remaining_time > quantum) ? quantum : p->remaining_time;
                p->remaining_time -= exec_time;

                timeline[time].time = time;
                timeline[time].pid = p->pid;
                timeline[time].duration = exec_time;

                time += exec_time;

                if (p->remaining_time == 0) {
                    p->completion_time = time;
                    p->turnaround_time = p->completion_time - p->arrival_time;
                    p->waiting_time = p->turnaround_time - p->burst_time;
                    p->response_time = p->start_time - p->arrival_time;
                    completed++;
                }
            }
        }
        if (idle) time++;
    }
}

// -----------------------------
// MLFQ (Multi-Level Feedback Queue) - Simplified Version
// -----------------------------
void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config,
                   timeline_event_t *timeline) {
    // ⚠️ Simplified version placeholder
    // Full implementation would use multiple queues and boosting logic.
    // Here we just call Round Robin on highest queue as a stub.
    schedule_rr(processes, n, config->quantums[0], timeline);
}
