#include <stdio.h>
#include "scheduler.h"
#include "algorithms.h"
#include "metrics.h"

int main() {
    process_t processes[] = {
        {1, 0, 5, 1, 0,0,0,0,0,0},
        {2, 1, 8, 2, 0,0,0,0,0,0},
        {3, 2, 3, 1, 0,0,0,0,0,0}
    };
    int n = 3;
    timeline_event_t timeline[1000];
    metrics_t m;

    int quantums[] = {2, 4};
    mlfq_config_t config = {2, quantums, 50};

    schedule_mlfq(processes, n, &config, timeline);
    calculate_metrics(processes, n, 15, &m);

    printf("MLFQ Test\n");
    for (int i = 0; i < n; i++)
        printf("PID %d: Start %d, Complete %d\n",
               processes[i].pid, processes[i].start_time, processes[i].completion_time);

    printf("Avg TAT: %.2f, Avg WT: %.2f\n", m.avg_turnaround_time, m.avg_waiting_time);
    return 0;
}
