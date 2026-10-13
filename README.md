*This project has been created as part of the 42 curriculum by brel-bou.*

# Codexion

## Description

Codexion is a multithreaded simulation in C using POSIX threads. Each coder repeatedly **compiles**, **debugs**, and **refactors**. To compile, a coder must acquire two adjacent shared dongles. Each dongle becomes unavailable for a configurable cooldown after release.

The project implements per-dongle request queues, configurable scheduling, synchronized resource acquisition, burnout monitoring, and coordinated shutdown. A coder burns out if the time since the start of its last compilation reaches `time_to_burnout` before another compilation begins. The simulation also stops when all coders complete their required number of compilations.

Execution flow: `main()` → argument parsing → simulation and resource initialization → monitor and coder threads → queued two-dongle acquisition → compile/debug/refactor cycle → completion or burnout → thread joins and cleanup.

**Scheduling policies:**
- **FIFO (First In, First Out):** lower request arrival order receives priority.
- **EDF (Earliest Deadline First):** earlier burnout deadline receives priority; arrival order breaks ties.

## Instructions

**Requirements:** C compiler, `make`, and POSIX threads (`pthread`).

Build from the repository root:

```sh
make
```

Run:

```sh
./codexion <coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <compile_required> <dongle_cooldown> <fifo|edf>
```

Example:

```sh
./codexion 5 10000 200 200 200 10 400 fifo
```

All durations are in milliseconds. `coders` and `compile_required` must be positive integers; burnout, compile, debug, and refactor times must be positive; dongle cooldown may be zero. The scheduler argument must match exactly `fifo` or `edf`.

Output format: `<milliseconds_since_start> <coder_id> <event>`, where events include `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, and `burned out`.

```sh
make clean   # Remove object files
make fclean  # Remove object files and executable
make re      # Rebuild from scratch
```
## Resources

- [POSIX threads overview — Linux man-pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [`pthread_create(3)` — Thread creation](https://man7.org/linux/man-pages/man3/pthread_create.3.html)
- [`pthread_mutex_lock(3p)` — Mutex locking](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [`pthread_cond_wait(3p)` — Condition variables and timed waits](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [Operating Systems: Three Easy Pieces — Concurrency chapters](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [Deadlock and Coffman conditions](https://en.wikipedia.org/wiki/Deadlock)

**AI use:** AI assistance was used to explain and review the execution flow, POSIX synchronization, FIFO/EDF queue scheduling, cooldowns, and burnout behavior; investigate intermittent starvation; help develop the coordinated pair-acquisition approach and its related queue, wait, release, initialization, and cleanup code; produce educational visualizations. The implementation and behavior remain subject to compilation, runtime testing, and evaluation.

## Blocking cases handled

- **Deadlock and Coffman's conditions:** Dongles require mutual exclusion, but a coder never holds one dongle while waiting for the other, eliminating *hold and wait* during pair acquisition. Both dongle mutexes are locked in increasing ID order to avoid circular lock acquisition. The pair is granted only when both dongles can be acquired together.
- **Starvation and queue contention:** FIFO/EDF comparators establish request priority per dongle. A waiting coder releases the coordination mutex and holds no dongle, allowing other coders to proceed. **Starvation is not formally eliminated:** contention, head-of-line blocking across two queues, or missed deadlines can still cause burnout.
- **Dongle cooldown:** Release sets `in_use = 0` and `available_at = release_time + dongle_cooldown`. Acquisition checks both timestamps; a timed condition-variable wait allows rechecking when cooldown expires.
- **Burnout detection:** The monitor reads each unfinished coder's `last_compile` under its mutex and checks `now - last_compile >= time_to_burnout`. `last_compile` is updated at the start of compilation. The monitor polls approximately every 1 ms; detection is not a hard real-time guarantee.
- **Log serialization:** `log_mutex` prevents concurrent coder and monitor messages from interleaving.
- **Failed requests and shutdown:** If insertion into the second dongle queue fails, the first insertion is rolled back. Stopping the simulation wakes waiting coders, removes outstanding requests, and allows threads to exit before synchronization objects and memory are destroyed.
- **Single-coder case:** A coder cannot acquire two distinct dongles when only one exists; the monitor eventually detects burnout.

## Thread synchronization mechanisms

| Primitive | Location / protected state | Coordination |
|---|---|---|
| `pthread_mutex_t` (`pair_mutex`) | Pair-acquisition coordination | Serializes registration, checks, and release notifications; released atomically while a coder waits on `pair_condition`. |
| `pthread_mutex_t` (each `dongle.mutex`) | Dongle queue, `in_use`, `available_at` | Both mutexes are acquired in ID order while checking and granting a pair; protects against double ownership and queue races. |
| `pthread_mutex_t` (each `coder.mutex`) | `last_compile`, `compile_count`, `finished` | Protects coder state shared with the monitor and completion checks. |
| `pthread_mutex_t` (`simulation.mutex`) | `running` | Synchronizes stop requests and worker/monitor reads. |
| `pthread_mutex_t` (`counter_mutex`) | `request_counter` | Gives concurrent requests unique arrival-order values. |
| `pthread_mutex_t` (`finished_mutex`) | `finished_coders` | Protects the total-completion count and final stop decision. |
| `pthread_mutex_t` (`log_mutex`) | Terminal logging | Serializes complete event messages. |
| `pthread_cond_t` (`pair_condition`) | Pair-acquisition waiters | `pthread_cond_wait` releases `pair_mutex` while sleeping and reacquires it on wakeup; timed waits handle cooldown expiry. Release and shutdown broadcast notifications. |
| `pthread_cond_t` (each `dongle.condition`) | Per-dongle notifications | Broadcast on release/shutdown; the current two-dongle acquisition path primarily waits on `pair_condition`. |

**Thread-safe communication:** Coder threads publish compilation timestamps under their own mutexes; the monitor reads those timestamps using the same mutexes. The monitor or the last finishing coder sets `running = 0` under `simulation.mutex`, then broadcasts to wake blocked threads. The main thread joins coder and monitor threads before cleanup.

**Custom events:** No separate custom event object is implemented. Condition-variable broadcasts and the shared `running` flag provide event-style wakeup and stop propagation.

