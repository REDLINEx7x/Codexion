*This project has been created as part of the 42 curriculum by moamhouc.*

# Codexion — Master the race for resources before the deadline masters you

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers problem, reimagined in a coding hub setting. Coders sit in a circular arrangement around a shared Quantum Compiler. Each coder needs two USB dongles simultaneously to compile their quantum code. Dongles are shared between adjacent coders, creating a resource contention problem that must be solved without deadlocks, starvation, or burnout.

The simulation models real-world challenges in concurrent systems: fair resource allocation, precise timing, and safe thread synchronization. Each coder is represented by a POSIX thread. A dedicated monitor thread watches for burnout and clean termination conditions. Two scheduling policies are supported: FIFO (First In, First Out) and EDF (Earliest Deadline First), both implemented via a custom binary min-heap priority queue.

The simulation ends either when a coder burns out (fails to compile within `time_to_burnout` milliseconds) or when all coders have compiled at least `number_of_compiles_required` times.

## Instructions

### Compilation

```bash
make
```

This compiles all source files with `-Wall -Wextra -Werror -pthread` and produces the `codexion` binary.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

### Arguments

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads and dongles (>= 1) |
| `time_to_burnout` | Max ms a coder can go without starting a compile (>= 1) |
| `time_to_compile` | Time in ms to complete a compile (>= 1) |
| `time_to_debug` | Time in ms spent debugging after compile (>= 1) |
| `time_to_refactor` | Time in ms spent refactoring after debug (>= 1) |
| `number_of_compiles_required` | Simulation ends when all coders reach this count (>= 1) |
| `dongle_cooldown` | Ms a dongle is unavailable after being released (>= 0) |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### Examples

```bash
# 2 coders, feasible parameters, clean stop after 5 compiles each
./codexion 2 800 300 100 100 5 0 fifo

# 3 coders with EDF scheduling
./codexion 3 1500 300 100 100 3 0 edf

# Burnout scenario
./codexion 2 400 300 100 100 5 500 fifo
```

### Cleanup

```bash
make clean    # remove object files
make fclean   # remove object files and binary
make re       # full recompile
```

## Blocking Cases Handled

### Deadlock Prevention
The classic deadlock condition occurs when every coder holds one dongle and waits for the other. This is prevented by requiring each coder to acquire both dongles atomically inside a single mutex-protected section (`state_lock`). A coder either gets both dongles at once or gets neither — partial acquisition is never allowed.

Coffman's four conditions are broken as follows:
- **Mutual exclusion**: necessary, dongles are exclusive resources
- **Hold and wait**: eliminated — coders never hold one dongle while waiting for the other
- **No preemption**: dongles are only released voluntarily after compile completes
- **Circular wait**: broken by the atomic acquisition strategy

An additional staggered start (`usleep(1000)` for even-numbered coders) prevents all coders from racing simultaneously at t=0.

### Starvation Prevention
Fair arbitration is enforced through a priority queue. Every dongle request is registered in a shared queue before attempting acquisition. A coder can only take dongles if it is at the front of the queue. With FIFO, requests are served in arrival order. With EDF, the coder whose burnout deadline is earliest is served first.

### Dongle Cooldown
After a coder releases a dongle, it is marked unavailable until `dongle_cooldown` milliseconds have elapsed via a `cooldown_until_ms` timestamp. Acquisition checks both that the dongle is not taken and that the cooldown has expired.

### Precise Burnout Detection
The monitor thread polls all coders every 1ms, checking `current_time - last_compile_start_ms > time_to_burnout`. When burnout is detected, `sim_active` is set to false and the burnout message is printed within 10ms of the actual deadline.

### Log Serialization
All output is protected by a dedicated `write_lock` mutex. No two threads can write to stdout simultaneously, preventing interleaved log lines.

## Thread Synchronization Mechanisms

### `pthread_mutex_t state_lock`
The primary shared-state mutex. Protects `sim_active`, all dongle fields, `last_compile_start_ms`, `compiles_done`, and the priority queue. Every access to shared simulation state is wrapped in lock/unlock on `state_lock`.

Example — atomic dongle acquisition in `try_acquire`:
```c
pthread_mutex_lock(&coder->data->state_lock);
if (pqueue_min_is(&coder->data->queue, coder))
{
    // check both dongles available
    // mark both taken atomically
    // pop from queue
}
pthread_mutex_unlock(&coder->data->state_lock);
```

### `pthread_mutex_t write_lock`
A dedicated output mutex. Protects all `printf` calls so that log lines from different threads never interleave on stdout.

### Priority Queue (Binary Min-Heap)
A custom binary min-heap (`heap_help.c`, `pqueue.c`) serves as the waiting queue for dongle requests. Each entry stores the coder pointer, request timestamp, and EDF deadline. The heap root always holds the highest-priority coder. `pqueue_min_is` checks in O(1) whether a given coder is at the front. Coders spin-poll with `usleep(500)` rather than blocking, keeping response time low and avoiding missed wakeups.

### Monitor Thread
A dedicated thread runs `monitor_routine`, polling every 1ms. It acquires `state_lock` to safely read all coder state. When it detects burnout or completion, it sets `sim_active = false`, causing all coder threads to exit on the next `check_sim_active` call.

### `check_sim_active`
A utility that acquires `state_lock`, reads `sim_active`, and releases the lock. Used by all threads to safely check simulation state without direct shared access.

## Resources

- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [The Little Book of Semaphores — Allen B. Downey](https://greenteapress.com/wp/semaphores/)
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary Heap — Wikipedia](https://en.wikipedia.org/wiki/Binary_heap)
- `man pthread_create`, `man pthread_mutex_init`, `man pthread_mutex_lock`, `man gettimeofday`, `man usleep`

### AI Usage
AI was used during this project for the following tasks:
- Cross-checking the implementation against subject requirements
- Reviewing heap logic for correctness (sift_up, sift_down, remove with rebalance)
- Identifying Norminette violations across all source files
- Generating and reviewing test cases for burnout detection, cooldown, and multi-coder fairness

