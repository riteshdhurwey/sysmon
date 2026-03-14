# SysMon 🖥️
A lightweight, real-time Linux system monitoring tool written in **modern C++** that reads system metrics directly from the Linux `/proc` filesystem and `statvfs` — **no external dependencies except ncurses**.

![Demo](https://img.shields.io/badge/platform-Linux-blue) ![C++17](https://img.shields.io/badge/C++-17-green) ![ncurses](https://img.shields.io/badge/TUI-ncurses-cyan)

---

[![SysMon Demo](https://asciinema.org/a/UJdGYqylhvy7JUit.svg)](https://asciinema.org/a/UJdGYqylhvy7JUit)
## Features

### System Monitoring
- **CPU usage** — real-time total%, user, system, idle, iowait, IRQ, softIRQ via `/proc/stat`
- **Memory usage** — used/total GB with usage bar via `/proc/meminfo`
- **Disk usage** — used/total GB with usage bar via `statvfs()`
- **System uptime** — displayed in header via `/proc/uptime`
- Refreshes every **1 second**

### Process Management
- **Full process list** — PID, name, CPU%, state, memory
- **Scrollable list** — navigate with ↑↓ arrow keys
- **Sort processes** — by CPU%, Memory, or PID (toggle ascending/descending)
- **Kill process** — press K on selected process, confirm with Y/N
- **Stable selection** — tracks process by PID across re-sorts and updates
- **Kernel thread filtering** — hides zero-memory kernel threads

### TUI (ncurses)
- **Window-based layout** — separate window per stat section
- **Color coded bars** — green/yellow/red based on usage thresholds
- **Pause on scroll** — list freezes while navigating, resumes after 1s idle
- **Help overlay** — press H for keyboard shortcuts
- **Terminal resize** — windows rebuild on resize

---

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| ↑ / ↓ | Scroll process list |
| C | Sort by CPU% |
| M | Sort by Memory |
| P | Sort by PID |
| K | Kill selected process |
| H | Toggle help window |
| Q | Quit |

---

## How It Works

SysMon reads directly from the Linux kernel's virtual filesystem — no `top`, no `ps`, no shell commands.

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

### Stable Process Selection
Process list re-sorts every second. Instead of tracking by index (which changes on every sort), SysMon tracks the **selected process by PID** — re-syncing the index after each update so the highlight follows the correct process.

---

## Project Structure

```
sysmon/
├── include/
│   ├── CPUUtils.h        # /proc/stat reader (shared)
│   ├── cpu.h             # CPUMonitor class
│   ├── ResourceMonitor.h # RAM & Disk combined
│   ├── process.h         # ProcessMonitor class
│   ├── systemMonitor.h   # Top-level monitor
|   ├── sysinfo.h 
│   └── ui.h              # ncurses TUI class
     
└── src/
    ├── main.cpp
    ├── cpu.cpp
    ├── ResourceMonitor.cpp
    ├── process.cpp
    ├── systemMonitor.cpp
    ├── sysinfo.cpp 
    └── ui.cpp
```

---

## Build & Run

### Requirements
- Linux (kernel 2.6+)
- g++ with C++17 support
- ncurses (`libncurses-dev`)

### Install ncurses
```bash
# Debian/Ubuntu
sudo apt install libncurses-dev

# Arch
sudo pacman -S ncurses
```

### Build
```bash
git clone https://github.com/riteshdhurwey/SysMon.git
cd SysMon
make
```

### Run
```bash
./sysmon
```

Press `Q` to exit.

### Clean
```bash
make clean
```

---

## /proc Files Used

| File | Purpose |
|------|---------|
| `/proc/stat` | System-wide CPU tick counters |
| `/proc/meminfo` | Memory totals and availability |
| `/proc/uptime` | System uptime in seconds |
| `/proc/[pid]/stat` | Per-process CPU time (utime + stime) |
| `/proc/[pid]/status` | Per-process resident memory (VmRSS) |
| `/proc/[pid]/comm` | Process name |
| `statvfs()` syscall | Disk block counts and sizes |

---

## Roadmap

- [x] Real-time CPU, Memory, Disk monitoring
- [x] ncurses TUI with windowed layout
- [x] Scrollable, sortable process list
- [x] Kill process feature
- [x] System uptime
- [ ] Network I/O from `/proc/net/dev`
- [ ] Per-core CPU breakdown
- [ ] User column in process list
- [ ] `--interval` and `--top N` CLI flags
- [ ] Metric logging to CSV

---

## Author

**Ritesh Dhurwey**
[LinkedIn](https://linkedin.com/in/ritesh-dhurwey-100383216) · [GitHub](https://github.com/riteshdhurwey)
