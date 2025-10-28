#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "scheduler.h"

// -----------------------------
// FIFO (First In First Out)
// -----------------------------
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline);

// -----------------------------
// SJF (Shortest Job First) - non-preemptive
// -----------------------------
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline);

// -----------------------------
// STCF (Shortest Time to Completion First) - preemptive SJF
// -----------------------------
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline);

// -----------------------------
// Round Robin
// -----------------------------
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline);

// -----------------------------
// MLFQ (Multi-Level Feedback Queue)
// -----------------------------
typedef struct {
    int num_queues;
    int *quantums;          // Quantum por cada cola
    int boost_interval;     // Tiempo de refuerzo (boost)
} mlfq_config_t;

void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config,
                   timeline_event_t *timeline);

#endif // ALGORITHMS_H
