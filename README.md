# SysMon 🖥️

A lightweight, real-time Linux system monitoring tool written in **modern C++** that reads system metrics directly from the Linux `/proc` filesystem and `statvfs` — **zero external dependencies**.

```
========== CPU ==========
Total Usage: 12.4%
User: 8.1%   System: 3.2%   Idle: 87.6%
IOWait: 0.8%   IRQ: 0.1%   SoftIRQ: 0.2%

========== MEMORY ==========
Total: 7812 MB
Used : 4921 MB
Free : 2891 MB
Usage: 63.0%

========== DISK ==========
Total: 124.76 GB
Used : 90.63 GB
Free : 27.45 GB
Usage: 77.99%

========== TOP PROCESSES ==========
PID     NAME                STATE   MEM(MB)     CPU%
1234    firefox             S       512         4.21
5678    clang++             R       210         2.10
...
```

---

## Features

- **CPU usage** — real-time user, system, idle, iowait, IRQ, softIRQ breakdown via `/proc/stat`
- **Memory usage** — total, used, available via `/proc/meminfo`
- **Disk usage** — total, used, free, usage% via `statvfs()` (matches `df -h` output)
- **Top 10 processes** — sorted by CPU%, with PID, name, state, and resident memory via `/proc/[pid]/stat` and `/proc/[pid]/status`
- Refreshes every **1 second** with screen clear

---

## How It Works

SysMon reads directly from the Linux kernel's virtual filesystem — no `top`, no `ps`, no external libraries.

### CPU Usage (Delta Sampling)
Linux exposes cumulative CPU ticks in `/proc/stat`. A single snapshot is meaningless — SysMon takes **two snapshots with a time delta** and calculates the percentage change:

```
cpu_usage% = (1 - idle_delta / total_delta) * 100
```

Where `idle_delta` includes both `idle` and `iowait` ticks — the same method used by `htop`.

### Memory
Reads `MemTotal` and `MemAvailable` from `/proc/meminfo`:
```
used = MemTotal - MemAvailable
```
Uses `MemAvailable` (not `MemFree`) because it accounts for reclaimable cache — a more accurate picture of actual available memory.

### Disk
Uses `statvfs()` with `f_bavail` (available to non-root users) and `f_bfree` (total free including reserved):
```
used  = (f_blocks - f_bfree) * f_frsize
free  = f_bavail * f_frsize
total = used + free
```
This matches `df -h` behaviour exactly. Linux reserves ~5% of blocks for root — using `f_bavail` vs `f_bfree` is intentional.

### Process CPU%
Each process's CPU usage is calculated using the same delta method — two reads of `/proc/[pid]/stat` with a **500ms interval** between them:
```
process_cpu% = (proc_delta / total_cpu_delta) * 100
```

---

## Project Structure
```
sysmon/
include/
    cpu.h
    ResouceMonitor.h // RAM & Disk Combined
    process.h
    CPUUtils.h
    display.h
    systemMonitor.h

src/
    main.cpp
    cpu.cpp
    ResourceMonitorcpp
    process.cpp
    display.cpp
    systemMonitor.cpp

build/
    sysmon
```
---

## Build & Run

### Requirements
- Linux (kernel 2.6+)
- g++ with C++17 support
- CMake 3.10+

### Build
```bash
git clone https://github.com/riteshdhurwey/SysMon.git
cd SysMon
mkdir build && cd build
cmake ..
make
```

### Run
```bash
./sysmon
```

Press `Ctrl+C` to exit.

---

## /proc Files Used

| File | Purpose |
|---|---|
| `/proc/stat` | System-wide CPU tick counters |
| `/proc/meminfo` | Memory totals and availability |
| `/proc/[pid]/stat` | Per-process CPU time (utime + stime) |
| `/proc/[pid]/status` | Per-process resident memory (VmRSS) |
| `/proc/[pid]/comm` | Process name |
| `statvfs()` syscall | Disk block counts and sizes |

---

## Roadmap

- [ ] Per-core CPU breakdown
- [ ] ANSI color output (red when CPU > 80%)
- [ ] ncurses TUI
- [ ] Network I/O from `/proc/net/dev`
- [ ] `--interval` and `--top N` CLI flags
- [ ] Metric logging to CSV

---

## Author

**Ritesh Dhurwey**
[LinkedIn](https://linkedin.com/in/ritesh-dhurwey-100383216) · [GitHub](https://github.com/riteshdhurwey)
