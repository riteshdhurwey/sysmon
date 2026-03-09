#pragma once
#include <string>
#include<fstream>
#include<sys/statvfs.h>
#include<iostream>

struct MemoryUsage {
    long totalKB;
    long usedKB;
    long availableKB;
    double percent;
};

struct DiskUsage {
    double totalGB;
    double usedGB;
    double freeGB;
    double percent;
};

class ResourceMonitor {
private:
    MemoryUsage memory;
    DiskUsage disks;

    MemoryUsage readMemInfo();   // reads /proc/meminfo
    DiskUsage readDiskInfo();  // reads disk stats via statvfs

public:
    void update();        // updates both memory and disk
    const MemoryUsage& getMemory() const;
    const DiskUsage& getDisks() const;
};