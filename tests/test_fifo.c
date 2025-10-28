#include <stdio.h>
#include "scheduler.h"
#include "algorithms.h"
#include "metrics.h"

int main() {
    process_t processes[] = {
        {1, 0, 5, 1, 0,0,0,0,0,0},
        {2, 2, 3, 2, 0,0,0,0,0,0},
        {3, 4, 2, 1, 0,0,0,0,0,0}
    };
    int n = 3;
    timeline_event_t timeline[100];
    metrics_t m;

    schedule_fifo(processes, n, timeline);
    calculate_metrics(processes, n, 10, &m);

    printf("FIFO Test\n");
    for (int i = 0; i < n; i++) {
        printf("PID %d: Start %d, Complete %d, TAT %.2f, WT %.2f\n",
               processes[i].pid,
               processes[i].start_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);
    }
    printf("Avg TAT: %.2f, Avg WT: %.2f, CPU Util: %.2f%%\n",
           m.avg_turnaround_time, m.avg_waiting_time, m.cpu_utilization);
    return 0;
}
