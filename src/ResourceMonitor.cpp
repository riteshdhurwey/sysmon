#include"ResourceMonitor.h"
MemoryUsage ResourceMonitor::readMemInfo(){
    MemoryUsage data{};
    std::ifstream file("/proc/meminfo");
    std::string line;
    while (std::getline(file, line)) {
        if(line.find("MemTotal:") == 0) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &data.totalKB);
        }
        if(line.find("MemAvailable:") == 0) {
            sscanf(line.c_str(), "MemAvailable: %ld kB", &data.availableKB);
            break; 
        }
    }

    if(data.totalKB == 0) return {}; // safety check

    data.usedKB = data.totalKB - data.availableKB;
    
    data.percent = 100.0 * data.usedKB / data.totalKB;
    return data;
}

DiskUsage ResourceMonitor::readDiskInfo(){
    DiskUsage data{};
    struct statvfs buf;

    if(statvfs("/", &buf) != 0){
        std::cerr << "Failed to get filesystem stats\n";
        return data;
    }

    unsigned long long total = buf.f_blocks * buf.f_frsize;
    unsigned long long free  = buf.f_bfree  * buf.f_frsize;
    unsigned long long used  = total - free;

    data.totalGB = total / (1024.0 * 1024 * 1024);
    data.freeGB  = free  / (1024.0 * 1024 * 1024);
    data.usedGB  = used  / (1024.0 * 1024 * 1024);
    data.percent = (double)used / total * 100.0;

    return data;
}

void ResourceMonitor::update(){
    memory = readMemInfo();
    disks = readDiskInfo();
}

const DiskUsage &ResourceMonitor::getDisks()const{
    return disks;
}

const MemoryUsage &ResourceMonitor::getMemory()const{
    return memory;
}

