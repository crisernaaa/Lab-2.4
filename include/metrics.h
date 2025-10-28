#ifndef METRICS_H
#define METRICS_H

#include "scheduler.h"

typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    double cpu_utilization;
    double throughput;
    double fairness_index;   // Jain’s fairness index
} metrics_t;

/**
 * Calcula métricas de rendimiento del planificador.
 * @param processes Lista de procesos
 * @param n Número de procesos
 * @param total_time Tiempo total de simulación
 * @param metrics Puntero a estructura donde guardar resultados
 */
void calculate_metrics(process_t *processes, int n, int total_time,
                       metrics_t *metrics);

#endif // METRICS_H
