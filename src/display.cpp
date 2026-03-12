#include"display.h"
#include <iostream>
#include <iomanip>

void Display::show(const SystemMonitor& monitor)
{
    showCPU(monitor.getCPUUsage());
    showMemory(monitor.getMemoryInfo());
    showDisk(monitor.getDiskInfo());
    showProcesses(monitor.getTopProcesses());
}

void Display::showCPU(const CPUUsage &cpu){
     std::cout << "========== CPU ==========\n";
//printf is used to print a floating-point number with 2 digits after the decimal.
    printf("Total Usage: %.2f%%\n", cpu.total);

    printf("User: %.2f%%  ", cpu.user);
    printf("System: %.2f%%  ", cpu.system);
    printf("Idle: %.2f%%\n", cpu.idle);

    printf("IOWait: %.2f%%  ", cpu.iowait);
    printf("IRQ: %.2f%%  ", cpu.irq);
    printf("SoftIRQ: %.2f%%\n", cpu.softirq);

    printf("\n");
}

void Display::showDisk(const DiskUsage& disk)
{
    std::cout << "========== DISK ==========\n";

    std::cout << "Total: " << disk.totalGB << " GB\n";
    std::cout << "Used : " << disk.usedGB << " GB\n";
    std::cout << "Free : " << disk.freeGB << " GB\n";

    std::cout << "Usage: " << disk.percent << "%\n\n";
}

void Display::showProcesses(const std::vector<Process>& processes)
{
    std::cout << "========== TOP PROCESSES ==========\n";

    std::cout << std::left
              << std::setw(8) << "PID"
              << std::setw(20) << "NAME"
              << std::setw(8) << "STATE"
              << std::setw(12) << "MEM(MB)"
              << std::setw(8) << "CPU%" << "\n";

    for(const auto& p : processes)
    {
        std::cout << std::left
                  << std::setw(8) << p.pid
                  << std::setw(20) << p.name
                  << std::setw(8) << p.state
                  << std::setw(12) << p.memoryKB/1024
                  << std::setw(8) << p.cpuPercent
                  << "\n";
    }

    std::cout << "\n";
}


void Display::showMemory(const MemoryUsage& mem)
{
    std::cout << "========== MEMORY ==========\n";

    std::cout << "Total: " << mem.totalKB/1024 << " MB\n";
    std::cout << "Used : " << mem.usedKB/1024 << " MB\n";
    std::cout << "Free : " << mem.availableKB/1024 << " MB\n";

    std::cout << "Usage: " << mem.percent << "%\n\n";
}