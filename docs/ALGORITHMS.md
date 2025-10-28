# ALGORITHMS.md — CPU Scheduling Algorithms

## 1. FIFO (First In, First Out)
**Type:** Non-preemptive  
**Idea:** Run processes in order of arrival.

### Pseudocode
sort by arrival_time
for each process p:
if CPU idle, wait until p.arrival_time
run p until completion
### Pros
- Simple, easy to implement  
### Cons
- Long waiting times for later processes (convoy effect)

---

## 2. SJF (Shortest Job First)
**Type:** Non-preemptive  
**Idea:** Select process with smallest burst time among arrived ones.

### Pseudocode
while processes remain:
pick process with shortest burst_time among ready queue
execute to completion
### Pros
- Minimizes average waiting time  
### Cons
- May cause starvation for long jobs

---

## 3. STCF (Shortest Time to Completion First)
**Type:** Preemptive  
**Idea:** Always run the process with the smallest remaining time.

### Pseudocode
at each time unit:
pick process with smallest remaining_time
execute for 1 unit
### Pros
- Optimal for turnaround time  
### Cons
- Frequent context switches

---

## 4. Round Robin (RR)
**Type:** Preemptive  
**Idea:** Time-shared scheduling with fixed quantum.

### Pseudocode
ready_queue = FIFO
while ready_queue not empty:
p = dequeue()
run for quantum or until done
if remaining_time > 0: enqueue(p)
### Pros
- Fair CPU allocation  
- Good for interactive systems  
### Cons
- Quantum too small = overhead; too large = FIFO-like behavior

---

## 5. MLFQ (Multi-Level Feedback Queue)
**Type:** Preemptive, dynamic priority  
**Idea:** Multiple queues with different quantums; processes move between queues based on behavior.

### Rules
1. New processes enter top queue.
2. If process uses full quantum → demote to lower queue.
3. If process yields early → stay or promote.
4. Periodically boost all to top queue.

### Pros
- Adapts to mixed workloads  
### Cons
- Complex to tune parameters
