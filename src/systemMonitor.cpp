#include"systemMonitor.h"
const CPUUsage& SystemMonitor::getCPUUsage()const{
        return cpu.getUsage();
}

const MemoryUsage& SystemMonitor::getMemoryInfo() const{
    return resource.getMemory();
}

const DiskUsage& SystemMonitor::getDiskInfo() const{
    return resource.getDisks();
}

const std::vector<Process> & SystemMonitor::getProcesses() const{
    return processMonitor.getProcesses();
}

void SystemMonitor::update(){
    cpu.update();
    resource.update();
    processMonitor.update();
}

void SystemMonitor::sortProcessBy(SortBy criterion){
    processMonitor.sortBy(criterion);
}