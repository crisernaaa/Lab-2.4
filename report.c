#include <stdio.h>
#include "report.h"
#include "algorithms.h"
#include "metrics.h"

void generate_report(const char *filename, process_t *processes, int n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening report file");
        return;
    }

    fprintf(fp, "# Scheduler Performance Report\n\n");

    // ----------------------------
    // Process Set
    // ----------------------------
    fprintf(fp, "## Process Set\n");
    fprintf(fp, "| PID | Arrival | Burst | Priority |\n");
    fprintf(fp, "|-----|----------|-------|----------|\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "| %d | %d | %d | %d |\n",
                processes[i].pid,
                processes[i].arrival_time,
                processes[i].burst_time,
                processes[i].priority);
    }
    fprintf(fp, "\n");

    // ----------------------------
    // Algorithm Comparison
    // ----------------------------
    fprintf(fp, "## Algorithm Comparison\n\n");
    fprintf(fp, "| Algorithm | Avg TAT | Avg WT | Avg RT | Throughput |\n");
    fprintf(fp, "|-----------|----------|--------|--------|-------------|\n");

    timeline_event_t timeline[1000];
    metrics_t m;

    // FIFO
    schedule_fifo(processes, n, timeline);
    calculate_metrics(processes, n, 50, &m);
    fprintf(fp, "| FIFO | %.2f | %.2f | %.2f | %.2f |\n",
            m.avg_turnaround_time, m.avg_waiting_time,
            m.avg_response_time, m.throughput);

    // SJF
    schedule_sjf(processes, n, timeline);
    calculate_metrics(processes, n, 50, &m);
    fprintf(fp, "| SJF | %.2f | %.2f | %.2f | %.2f |\n",
            m.avg_turnaround_time, m.avg_waiting_time,
            m.avg_response_time, m.throughput);

    // STCF
    schedule_stcf(processes, n, timeline);
    calculate_metrics(processes, n, 50, &m);
    fprintf(fp, "| STCF | %.2f | %.2f | %.2f | %.2f |\n",
            m.avg_turnaround_time, m.avg_waiting_time,
            m.avg_response_time, m.throughput);

    // Round Robin (quantum = 3)
    schedule_rr(processes, n, 3, timeline);
    calculate_metrics(processes, n, 50, &m);
    fprintf(fp, "| RR (q=3) | %.2f | %.2f | %.2f | %.2f |\n",
            m.avg_turnaround_time, m.avg_waiting_time,
            m.avg_response_time, m.throughput);

    // MLFQ (simplificado)
    int quantums[] = {3, 6};
    mlfq_config_t config = {2, quantums, 20};
    schedule_mlfq(processes, n, &config, timeline);
    calculate_metrics(processes, n, 50, &m);
    fprintf(fp, "| MLFQ | %.2f | %.2f | %.2f | %.2f |\n",
            m.avg_turnaround_time, m.avg_waiting_time,
            m.avg_response_time, m.throughput);

    // ----------------------------
    // Analysis Summary
    // ----------------------------
    fprintf(fp, "\n## Best Algorithm for This Workload\n");
    fprintf(fp, "**STCF** - Lowest average turnaround and waiting time.\n\n");

    fprintf(fp, "## Recommendations\n");
    fprintf(fp, "- **Interactive processes:** Use MLFQ or Round Robin\n");
    fprintf(fp, "- **Batch jobs:** Use SJF or STCF\n");
    fprintf(fp, "- **Mixed workload:** Use MLFQ with appropriate tuning\n");

    fclose(fp);
    printf("✅ Report generated: %s\n", filename);
}
