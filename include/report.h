#ifndef REPORT_H
#define REPORT_H

#include "scheduler.h"
#include "metrics.h"

/**
 * Genera un informe de comparación de algoritmos.
 * @param filename Nombre del archivo de salida (.md o .html)
 * @param processes Conjunto de procesos simulados
 * @param n Número de procesos
 */
void generate_report(const char *filename, process_t *processes, int n);

#endif // REPORT_H
