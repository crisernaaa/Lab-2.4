# DESIGN.md — CPU Scheduler Simulator

## 1. Overview
This project implements a CPU scheduling simulator that supports multiple algorithms:
- FIFO
- SJF
- STCF
- Round Robin
- MLFQ

It is modular and extensible, with separate components for scheduling, metrics, GUI, and reporting.

---

## 2. Architecture
homework3/
├── src/ # Core source code
├── include/ # Header files
├── tests/ # Unit tests for each algorithm
├── workloads/ # Input datasets
├── docs/ # Documentation and reports

Each source file serves a clear purpose:
- **scheduler.c** — main controller, orchestrates simulations.
- **algorithms.c** — contains implementations of the five scheduling algorithms.
- **metrics.c** — computes performance metrics.
- **report.c** — generates Markdown/HTML comparison reports.
- **gui_gtk.c / gui_ncurses.c** — user interfaces.

---

## 3. Data Structures
Defined in `scheduler.h`:
- `process_t` — holds process attributes (arrival, burst, etc.)
- `timeline_event_t` — represents execution intervals for the Gantt chart.

---

## 4. Scheduling Pipeline
1. Load process data (from file or manual input)
2. Run selected scheduling algorithm
3. Record timeline events
4. Compute metrics
5. Generate report and/or visualize via GUI

---

## 5. Key Design Choices
| Decision | Justification |
|-----------|----------------|
| Modular structure | Easier testing and extension |
| Separate metrics module | Enables comparison across algorithms |
| Common process struct | Simplifies switching between algorithms |
| Timeline array | Provides consistent base for Gantt visualization |
| Markdown report | Human-readable and easy to convert to PDF/HTML |

---

## 6. Limitations & Future Work
- MLFQ simplified (no real dynamic boosting yet)
- No persistent GUI settings
- Metrics based on static workloads

---

## 7. Tools & Environment
- **Language:** C (C11)
- **OS:** Ubuntu 22.04 LTS
- **Compiler:** GCC 11+
- **Libraries:** GTK3 / ncurses
