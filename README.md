# Linux System Monitor (sysmon)

A lightweight **Linux system monitoring tool written in C++** that reads system information directly from the Linux **/proc** filesystem.
The program displays real-time **CPU usage, memory usage, disk usage, and running processes** without using external libraries.

This project demonstrates **Linux system programming**, **file parsing**, and **resource monitoring** using standard C++.

---

## Features

* Real-time **CPU usage monitoring**
* **RAM usage statistics**
* **Disk storage usage**
* **Running process list**
* Displays:

  * PID
  * Process name
  * Process state
  * Memory usage
  * CPU usage
* Refreshes automatically at a fixed interval
* Uses **Linux system files** such as `/proc` and `statvfs`

---

## System Information Sources

The program reads data from the following Linux interfaces:

| Resource            | Source                  |
| ------------------- | ----------------------- |
| CPU Usage           | `/proc/stat`            |
| Memory Usage        | `/proc/meminfo`         |
| Process Information | `/proc/[PID]/`          |
| Disk Usage          | `statvfs()` system call |

---

## Project Structure

```
linux-system-monitor/

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

## How It Works

### CPU Monitoring

The program reads CPU statistics from `/proc/stat`.

CPU usage is calculated using the difference between two snapshots:

```
usage = (1 - idle_time / total_time) * 100
```

---

### Memory Monitoring

Memory statistics are read from `/proc/meminfo`.

```
Used Memory = MemTotal - MemAvailable
```

---

### Disk Monitoring

Disk information is obtained using the `statvfs()` system call.

```
Total Space = f_blocks * f_frsize
Free Space  = f_bavail * f_frsize
Used Space  = Total - Free
```

---

### Process Monitoring

Processes are discovered by scanning numeric directories in `/proc`.

For each process the program reads:

```
/proc/[pid]/stat
/proc/[pid]/status
```

Displayed information includes:

* Process ID
* Process name
* Process state
* Memory usage
* CPU usage

---

## Build Instructions

Compile using **g++**:

```
g++ -std=c++17 -Wall -Wextra src/*.cpp -o sysmon
```

---

## Run

```
./sysmon
```

The program will start displaying real-time system statistics.

---

## Example Output

```
========== CPU ==========
User: 15%
System: 3%
Idle: 80%

========== MEMORY ==========
Total: 15382 MB
Used : 9123 MB
Free : 6259 MB
Usage: 59%

========== DISK ==========
Total: 512 GB
Used : 210 GB
Free : 302 GB
Usage: 41%

========== TOP PROCESSES ==========
PID     NAME           STATE   MEM(MB)   CPU%
1778    plasmashell    S       453       0.66
1543    kwin_wayland   S       364       0.49
43316   sysmon         R       3         0.16
```

---

## Requirements

* Linux operating system
* C++17 compatible compiler
* Access to `/proc` filesystem

---

## Learning Objectives

This project helps understand:

* Linux **/proc filesystem**
* **System resource monitoring**
* Parsing system files in C++
* CPU usage calculation
* Process management in Linux
* Clean modular project structure

---

## Future Improvements

Possible enhancements:

* Per-core CPU usage
* Network monitoring
* Process sorting by CPU usage
* Interactive interface
* Terminal UI (similar to `top` or `htop`)
* Configurable refresh rate

---

## License

This project is open for educational and learning purposes.
