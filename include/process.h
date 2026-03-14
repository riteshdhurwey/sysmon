#pragma once
#include<dirent.h>
#include <string>
#include <vector>
#include<unordered_map>
#include<thread>
#include<algorithm>
#include "CPUUtils.h"  // for CPUStats and readCPUStats()

enum class SortBy{
    CPU,MEM,PID
};
struct Process {
    int pid;             // Process ID
    std::string name;    // Process name
    std::string state;   // Process state (R, S, D, Z, etc.)
    long memoryKB;       // Resident memory in KB
    double cpuPercent;   // CPU usage %
};

class ProcessMonitor {
private:
    std::vector<Process> processes;  // All processes

    SortBy currentSort = SortBy::CPU;
    bool sortAscending = false;

    std::vector<int> getAllPids();             // Scan /proc for all PIDs
    unsigned long long readProcessCPU(int pid); // utime + stime from /proc/[pid]/stat
    long readProcessMemory(int pid);           // Resident memory in KB from /proc/[pid]/statm
    std::string readProcessName(int pid);      // Process name from /proc/[pid]/comm
    std::string readProcessState(int pid);     // Process state from /proc/[pid]/stat
    void applySort();
public:
    void update();                              // Updates CPU %, memory, name, state for top processes
    const std::vector<Process>& getProcesses() const; 
    void sortBy(SortBy criterion);
};