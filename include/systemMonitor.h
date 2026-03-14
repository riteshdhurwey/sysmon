#pragma once

#include "cpu.h"
#include "ResourceMonitor.h"
#include "process.h"

class SystemMonitor {
private:
    CPUMonitor cpu;
    ResourceMonitor resource;
    ProcessMonitor processMonitor;

public:
    void update();

    const CPUUsage& getCPUUsage() const;

    const MemoryUsage& getMemoryInfo() const;

    const DiskUsage& getDiskInfo() const;

    const std::vector<Process>& getProcesses() const;

    void sortProcessBy(SortBy criterion);
};