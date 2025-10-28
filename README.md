# 🧠 CPU Scheduler Simulator (Homework 3)

**Author:** *Cristóbal Serna*  
**Course:** Operating Systems  
**Platform:** Ubuntu 22.04 LTS  
**Language:** C (GCC 11+)  
**Interface:** GTK3 / ncurses  

---

## 📘 Overview

This project implements a **CPU Scheduling Simulator** that compares the performance of several classic scheduling algorithms:

- **FIFO** (First In First Out)  
- **SJF** (Shortest Job First)  
- **STCF** (Shortest Time to Completion First)  
- **Round Robin (RR)**  
- **MLFQ** (Multi-Level Feedback Queue)

The simulator computes performance metrics (turnaround, waiting, response, throughput, fairness, and CPU utilization) and provides an interactive interface using either **GTK** or **ncurses**.

---

## 🧩 Project Structure

homework3/
├── src/ # Core source code
│ ├── scheduler.c
│ ├── algorithms.c
│ ├── metrics.c
│ ├── gui_gtk.c
│ ├── gui_ncurses.c
│ └── report.c
├── include/ # Header files
│ ├── scheduler.h
│ ├── algorithms.h
│ └── metrics.h
├── tests/ # Unit tests
│ ├── test_fifo.c
│ ├── test_sjf.c
│ ├── test_stcf.c
│ ├── test_rr.c
│ └── test_mlfq.c
├── workloads/ # Sample process sets
│ ├── workload1.txt
│ ├── workload2.txt
│ └── workload3.txt
├── docs/ # Documentation and reports
│ ├── DESIGN.md
│ ├── ALGORITHMS.md
│ └── ANALYSIS.pdf
├── Makefile
└── README.md
---


