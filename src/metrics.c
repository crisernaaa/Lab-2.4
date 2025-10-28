#include <stdio.h>
#include <math.h>
#include "metrics.h"

void calculate_metrics(process_t *processes, int n, int total_time,
                       metrics_t *metrics) {
    double sum_turnaround = 0.0;
    double sum_waiting = 0.0;
    double sum_response = 0.0;
    double busy_time = 0.0;  // Tiempo que la CPU estuvo ocupada

    double sum_x = 0.0;      // Para fairness
    double sum_x2 = 0.0;

    for (int i = 0; i < n; i++) {
        process_t *p = &processes[i];

        // Por si no fueron calculadas aún
        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->turnaround_time - p->burst_time;
        p->response_time = p->start_time - p->arrival_time;

        sum_turnaround += p->turnaround_time;
        sum_waiting += p->waiting_time;
        sum_response += p->response_time;

        busy_time += p->burst_time;

        sum_x += p->turnaround_time;
        sum_x2 += pow(p->turnaround_time, 2);
    }

    metrics->avg_turnaround_time = sum_turnaround / n;
    metrics->avg_waiting_time = sum_waiting / n;
    metrics->avg_response_time = sum_response / n;

    metrics->cpu_utilization = (total_time > 0)
        ? (busy_time / total_time) * 100.0
        : 0.0;

    metrics->throughput = (total_time > 0)
        ? (double)n / total_time
        : 0.0;

    metrics->fairness_index = (sum_x2 > 0)
        ? pow(sum_x, 2) / (n * sum_x2)
        : 0.0;
}

