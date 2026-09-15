*This project has been created as part of the 42 curriculum by jbarreir.*

<div align="center">

# codexion

<img width="100%" alt="codexion_logo_1000" src="https://github.com/user-attachments/assets/8d892a72-6619-4063-93c0-1bdf15be778c" />

### 🔥 *Master the Burnout* 🔥

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Standard](https://img.shields.io/badge/standard-C89-blue.svg)
![Threads](https://img.shields.io/badge/threads-pthreads-orange.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![42 School](https://img.shields.io/badge/school-42-black?logo=42&logoColor=white)

</div>

## Description

**codexion** is a concurrent-programming simulation written in C with POSIX threads. A group of coders work in a shared coworking space, where each compilation requires two adjacent quantum dongles. After compiling, a coder debugs and refactors before requesting access to the dongles again.

The project focuses on coordinating multiple threads safely while preserving liveness: coders must not deadlock, shared dongles must not be used simultaneously, access must follow the selected scheduling policy, and a dedicated monitor must detect burnout and stop the simulation cleanly.

### Contents

- [Instructions](#instructions)
- [Scheduling policies](#scheduling-policies)
- [Blocking cases handled](#blocking-cases-handled)
- [Thread synchronization mechanisms](#thread-synchronization-mechanisms)
- [Project structure](#project-structure)
- [Testing](#testing)
- [Resources](#resources)

---

## Instructions

### Compilation

Compile the project with:

```bash
make
```

This creates the `codexion` executable.

Useful Makefile rules:

```bash
make clean     # Remove object files
make fclean    # Remove object files and the executable
make re        # Rebuild everything
```

### Execution

```bash
./codexion \
  <number_of_coders> \
  <time_to_burnout> \
  <time_to_compile> \
  <time_to_debug> \
  <time_to_refactor> \
  <compiles_required> \
  <dongle_cooldown> \
  <scheduler>
```

All times are expressed in milliseconds.

Example:

```bash
./codexion 4 800 200 200 200 5 10 fifo
```

This starts four coders. A coder burns out if more than `800 ms` pass without starting a new compilation. Each compilation, debugging phase, and refactoring phase lasts `200 ms`; every coder must compile five times; dongles have a `10 ms` cooldown; and access is scheduled with FIFO.

Valid schedulers are:

```text
fifo
edf
```

---

## Scheduling policies

### FIFO

The **First In, First Out** scheduler gives priority to the coder that first entered a dongle queue.

### EDF

The **Earliest Deadline First** scheduler prioritizes the waiting coder with the earliest recorded compilation timestamp. This aims to favor the coder that has waited longest since its last compilation.

---

## Blocking cases handled

Concurrency introduces several failure modes that must be handled explicitly.

### Deadlock prevention

Each coder needs two dongles, which could create circular waiting. Dongles are always locked in a deterministic order based on their identifier. By removing circular wait, the implementation prevents Coffman-style deadlocks.

### Mutual exclusion

Every dongle owns a mutex. A coder may mark a dongle as `PLUGGED` only while holding the corresponding lock, preventing simultaneous access by two coders.

### Waiting without busy loops

When one or both dongles are unavailable, the coder waits on a condition variable instead of continuously polling. Once a dongle becomes available, its condition variable is broadcast so waiting coders can re-evaluate access safely.

### Cooldown handling

After a compilation starts, both dongles enter `COOLING_DOWN`. The monitor periodically checks every dongle and makes it available again once its cooldown has elapsed.

Cooldown updates are independent of whether the coder associated with a dongle has already completed its work. This prevents a completed coder from leaving a dongle permanently unavailable to its neighbor.

### Burnout detection

A dedicated monitor thread checks each coder's last compilation start time. If the configured burnout limit is exceeded, the monitor marks the coder as burned out, sends the shutdown signal, wakes any threads blocked on dongle condition variables, and stops the simulation.

### Serialized logging

All output is protected by a logging mutex. This ensures that a log line is printed atomically and that concurrent threads cannot interleave their messages.

---

## Thread synchronization mechanisms

### `pthread_mutex_t`

Mutexes protect all shared mutable state:

| Shared resource | Protection |
|---|---|
| Dongle state, queue, cooldown timestamp | One mutex per dongle |
| Coder state and compilation counters | One mutex per coder |
| Global simulation status | Simulation mutex |
| Terminal output | Log mutex |

For example, a coder checks whether both dongles are available only after locking both dongle mutexes. This makes the availability check and the transition to `PLUGGED` atomic from the perspective of other coders.

### `pthread_cond_t`

Each dongle has a condition variable. A coder that cannot obtain both dongles releases the appropriate mutex and sleeps with `pthread_cond_wait`.

When a coder releases dongles or the monitor finishes their cooldown, `pthread_cond_broadcast` wakes all waiting coders. Every awakened thread locks the resources again and re-checks the condition before proceeding, which is essential because a wake-up does not guarantee ownership.

### Monitor-to-coder communication

The monitor updates the shared simulation status to `SHUTDOWN_SIGNAL` under the simulation mutex. Coders check this state during their routine and while sleeping. When shutdown occurs, the monitor also broadcasts all dongle condition variables, ensuring that no thread remains blocked forever.

---

## Project structure

```text
.
├── includes/
│   └── codexion.h
├── src/
│   ├── core/        # Coder routine, dongles, safe lock helpers
│   ├── init/        # Allocation, initialization, cleanup, thread creation
│   ├── monitor/     # Burnout and cooldown monitor
│   ├── sheduler/    # FIFO and EDF queue policies
│   ├── utils/       # Argument parsing, timing, logging, sleeps
│   └── main.c
├── Makefile
└── tester.sh
```

---

## Testing

The repository includes a small test runner:

```bash
./tester.sh 1
./tester.sh 2
./tester.sh big
./tester.sh starvation
```

Example stress test:

```bash
./codexion 199 60000 61 61 61 10 61 edf
```

For memory and thread analysis on Linux:

```bash
./tester.sh 1 mem
./tester.sh 1 helgrind
```

Recommended commands:

```bash
valgrind --leak-check=full ./codexion 4 800 200 200 200 5 10 fifo
valgrind --tool=helgrind ./codexion 4 800 200 200 200 5 10 fifo
```

---

## Resources

### References

- [POSIX Threads Programming](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [`pthread_mutex_lock(3)`](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [`pthread_cond_wait(3)`](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html)
- [The Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Coffman's deadlock conditions](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- [Valgrind: Helgrind](https://valgrind.org/docs/manual/hg-manual.html)

### AI usage

AI was used as a learning and review assistant during development. It helped with:

- Reviewing the synchronization design and identifying possible deadlock and cooldown issues.
- Explaining POSIX mutex and condition-variable behavior.
- Discussing test cases for starvation, high thread counts, and shutdown behavior.
- Drafting and improving this README.

All design choices, implementation, testing, and final code decisions were reviewed and applied by the author.
