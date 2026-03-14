#pragma once

#include <ncurses.h>
#include <string>
#include <vector>
#include "systemMonitor.h"
#include"sysinfo.h"
#include<chrono>
#include<signal.h>

class UI {
private:
    // windows
    WINDOW* headerWin = nullptr;
    WINDOW* diskWin   = nullptr;
    WINDOW* memWin    = nullptr;
    WINDOW* cpuWin    = nullptr;
    WINDOW* procWin   = nullptr;
    WINDOW* statusWin = nullptr;
    WINDOW* helpWin   = nullptr;
    WINDOW* killWin = nullptr;
    // state
    int selectedPid  = -1;
    int selectedRow  = 0;
    int scrollOffset = 0;
    int termRows     = 0;
    int termCols     = 0;

    bool paused = false;
    std::chrono::steady_clock::time_point lastKeyTime;

    //Help window
    bool showHelp = false;
    bool killConfirm = false;
    // setup
    void initColors();
    void createWindows();
    void destroyWindows();

    // helpers
    void drawBar(WINDOW* win, int y, int x, int width, double percent);
    void drawWinTitle(WINDOW* win, const char* title);

    // sections
    void drawHeader();
    void drawDisk(const DiskUsage& disk);
    void drawMemory(const MemoryUsage& mem);
    void drawCPU(const CPUUsage& cpu);
    void drawProcessHeader();
    void drawProcessList(const std::vector<Process>& procs);
    void drawStatus(const Process& sel);
    void drawHelp();
    void drawKill(int pid,std::string procName);

    sysInfo sysinfo;

public:
    void init();
    void destroy();
    void draw(const SystemMonitor& mon);
    bool handleInput( SystemMonitor& mon);

    bool isPaused() const { return paused; }
    void setPaused(bool val) { paused = val; }
    std::chrono::steady_clock::time_point getLastKeyTime() const { return lastKeyTime; }
    
};