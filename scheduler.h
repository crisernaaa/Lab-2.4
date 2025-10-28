#ifndef SCHEDULER_H
#define SCHEDULER_H

// -----------------------------
// Estructura que representa un proceso
// -----------------------------
typedef struct {
    int pid;                // ID del proceso
    int arrival_time;       // Tiempo en que llega al sistema
    int burst_time;         // Tiempo total de CPU requerido
    int priority;           // Prioridad (menor valor = más prioridad)
    int remaining_time;     // Tiempo restante por ejecutar
    int start_time;         // Primer momento en que fue planificado
    int completion_time;    // Momento en que finalizó
    int turnaround_time;    // turnaround = completion - arrival
    int waiting_time;       // waiting = turnaround - burst
    int response_time;      // response = start - arrival
} process_t;

// -----------------------------
// Estructura para eventos en la línea de tiempo
// -----------------------------
typedef struct {
    int time;       // Momento en que comienza el evento
    int pid;        // Proceso en ejecución
    int duration;   // Duración del evento
} timeline_event_t;

#endif // SCHEDULER_H

