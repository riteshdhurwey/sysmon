#include "ui.h"
#include <ctime>
#include <algorithm>

#define C_HEADER  1
#define C_STATUS  2
#define C_BAR_OK  3
#define C_BAR_WRN 4
#define C_BAR_CRT 5
#define C_TITLE   6
#define C_DIM     8



void UI::init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);
    getmaxyx(stdscr, termRows, termCols);
    initColors();
    createWindows();
}

void UI::destroy() {
    destroyWindows();
    endwin();
}

bool UI::handleInput( SystemMonitor& mon) {
    std::vector<Process> proc = mon.getProcesses();
    int total       = (int)proc.size();
    int visibleRows = getmaxy(procWin) - 3; // minus border + header row

    int ch = getch();
    if (ch == ERR) return true;

    if (killConfirm) {
    if (ch == 'y' || ch == 'Y') {
        kill(selectedPid, SIGTERM);
        killConfirm = false;
        delwin(killWin);
        killWin = nullptr;
    }
    if (ch == 'n' || ch == 'N') {
        killConfirm = false;
        delwin(killWin);
        killWin = nullptr;
    }
    return true;  // block all other input while confirming
}

    switch (ch) {
        case 'q': case 'Q':
            return false;

        case KEY_UP:
            paused = true;
            lastKeyTime = std::chrono::steady_clock::now();
            if (selectedRow > 0) {
                selectedRow--;
                if (selectedRow < scrollOffset)
                    scrollOffset--;
            }
            break;

        case KEY_DOWN:
            paused = true;
            lastKeyTime = std::chrono::steady_clock::now();
            if (selectedRow < total - 1) {
                selectedRow++;
                if (selectedRow >= scrollOffset + visibleRows)
                    scrollOffset++;
            }
            break;
        case 'c' :case 'C':
            mon.sortProcessBy(SortBy::CPU);
            break;
        case 'm' : case 'M':
            mon.sortProcessBy(SortBy::MEM);
            break;
        case 'p' : case 'P':
            mon.sortProcessBy(SortBy::PID);
            break;
        case 'h' : case 'H':
            showHelp = !showHelp;
            break;
        case 'k' : case 'K':
            killConfirm = true;
            break;
        case KEY_RESIZE:
            destroyWindows();
            getmaxyx(stdscr, termRows, termCols);
            createWindows();
            clear();
            refresh();
            break;
    }
     if (!proc.empty() && selectedRow < total)
        selectedPid = proc[selectedRow].pid;
    return true;
}



void UI::initColors() {
    start_color();
    use_default_colors();
    init_pair(C_HEADER,  COLOR_BLACK,  COLOR_CYAN);
    init_pair(C_STATUS,  COLOR_BLACK,  COLOR_CYAN);
    init_pair(C_BAR_OK,  COLOR_GREEN,  -1);
    init_pair(C_BAR_WRN, COLOR_YELLOW, -1);
    init_pair(C_BAR_CRT, COLOR_RED,    -1);
    init_pair(C_TITLE,   COLOR_CYAN,   -1);
    init_pair(C_DIM,     COLOR_WHITE,  -1);
}

void UI::createWindows() {
    headerWin = newwin(1,              termCols, 0,        0);
    diskWin   = newwin(3,              termCols, 1,        0);
    memWin    = newwin(3,              termCols, 4,        0);
    cpuWin    = newwin(4,              termCols, 7,        0);
    procWin   = newwin(termRows - 12,  termCols, 11,       0);
    statusWin = newwin(1,              termCols, termRows - 1, 0);
    
}

void UI::destroyWindows() {
    if (headerWin) { delwin(headerWin); headerWin = nullptr; }
    if (diskWin)   { delwin(diskWin);   diskWin   = nullptr; }
    if (memWin)    { delwin(memWin);    memWin    = nullptr; }
    if (cpuWin)    { delwin(cpuWin);    cpuWin    = nullptr; }
    if (procWin)   { delwin(procWin);   procWin   = nullptr; }
    if (statusWin) { delwin(statusWin); statusWin = nullptr; }
    if (helpWin) { delwin(helpWin); helpWin = nullptr; }
}



void UI::drawBar(WINDOW* win, int y, int x, int width, double percent) {
    int filled = (int)((percent / 100.0) * width);
    filled = std::max(0, std::min(filled, width));

    int pair = C_BAR_OK;
    if (percent >= 80) pair = C_BAR_CRT;
    else if (percent >= 50) pair = C_BAR_WRN;

    mvwaddch(win, y, x, '[');
    wattron(win, COLOR_PAIR(pair) | A_BOLD);
    for (int i = 0; i < filled; i++) waddch(win, '#');
    wattroff(win, COLOR_PAIR(pair) | A_BOLD);
    for (int i = filled; i < width; i++) waddch(win, ' ');
    mvwaddch(win, y, x + width + 1, ']');
}

void UI::drawWinTitle(WINDOW* win, const char* title) {
    box(win, 0, 0);
    wattron(win, COLOR_PAIR(C_TITLE) | A_BOLD);
    mvwprintw(win, 0, 2, " %s ", title);
    wattroff(win, COLOR_PAIR(C_TITLE) | A_BOLD);
}


void UI::drawHeader() {
    werase(headerWin);
    wbkgd(headerWin, COLOR_PAIR(C_HEADER));
    wattron(headerWin, COLOR_PAIR(C_HEADER) | A_BOLD);

    std::string t = sysinfo.currentTime();
    std::string uptime = sysinfo.getupTime();
    mvwprintw(headerWin, 0, 2, "SysMon v1.0");
    mvwprintw(headerWin, 0, (termCols - (int)t.size()) / 2, "%s", t.c_str());
    mvwprintw(headerWin,0,termCols-20,"Uptime %s",uptime.c_str());

    wattroff(headerWin, COLOR_PAIR(C_HEADER) | A_BOLD);
    wrefresh(headerWin);
}

void UI::drawDisk(const DiskUsage& disk) {
    werase(diskWin);
    drawWinTitle(diskWin, "DISK");

    int bw = (termCols / 2) - 10;
    mvwprintw(diskWin, 1, 2, "%5.1f / %5.1f GB", disk.usedGB, disk.totalGB);
    drawBar(diskWin, 1, 22, bw, disk.percent);
    mvwprintw(diskWin, 1, 22 + bw + 3, "%3.0f%%", disk.percent);

    wrefresh(diskWin);
}

void UI::drawMemory(const MemoryUsage& mem) {
    werase(memWin);
    drawWinTitle(memWin, "MEMORY");

    int bw = (termCols / 2) - 10;
    mvwprintw(memWin, 1, 2, "%5.1f / %5.1f GB",
              mem.usedKB / 1024.0 / 1024.0,
              mem.totalKB / 1024.0 / 1024.0);
    drawBar(memWin, 1, 22, bw, mem.percent);
    mvwprintw(memWin, 1, 22 + bw + 3, "%3.0f%%", mem.percent);

    wrefresh(memWin);
}

void UI::drawCPU(const CPUUsage& cpu) {
    werase(cpuWin);
    drawWinTitle(cpuWin, "CPU");

    int bw = (termCols / 2) - 10;
    mvwprintw(cpuWin, 1, 2, "%5.1f%%", cpu.total);
    drawBar(cpuWin, 1, 22, bw, cpu.total);
    mvwprintw(cpuWin, 1, 22 + bw + 3, "%3.0f%%", cpu.total);

    wattron(cpuWin, COLOR_PAIR(C_DIM));
    mvwprintw(cpuWin, 2, 2, "usr:%4.1f%%  sys:%4.1f%%  idl:%4.1f%%  iow:%4.1f%%",
              cpu.user, cpu.system, cpu.idle, cpu.iowait);
    wattroff(cpuWin, COLOR_PAIR(C_DIM));

    wrefresh(cpuWin);
}

void UI::drawProcessHeader() {
    werase(procWin);
    drawWinTitle(procWin, "PROCESSES");

    wattron(procWin, A_BOLD | A_UNDERLINE);
    mvwprintw(procWin, 1, 2, "%-7s %-16s %-9s %-6s %s",
          "PID", "NAME", "CPU%", "STATE", "MEM(MB)");

    wattroff(procWin, A_BOLD | A_UNDERLINE);
}

void UI::drawProcessList(const std::vector<Process>& procs) {
    int visibleRows = getmaxy(procWin) - 3;  // minus top border + header + bottom border
    int total       = (int)procs.size();
    int maxOffset   = std::max(0, total - visibleRows);

    scrollOffset = std::max(0, std::min(scrollOffset, maxOffset));

    for (int i = 0; i < visibleRows && (scrollOffset + i) < total; i++) {
        const Process& p = procs[scrollOffset + i];
        if(p.memoryKB == 0)continue;
        int absIdx = scrollOffset + i;
        bool sel   = (absIdx == selectedRow);
        int y      = i + 2;

        mvwhline(procWin, y, 1, ' ', termCols - 2); // clear line first

        if (sel) wattron(procWin, A_REVERSE | A_BOLD);
        char cpuStr[16];
        snprintf(cpuStr, sizeof(cpuStr), "%.1f%%", p.cpuPercent);

        mvwprintw(procWin, y, 2, "%-7d %-16s %-9s %-6s %ld",
                p.pid, p.name.c_str(), cpuStr,
                p.state.c_str(), p.memoryKB / 1024);

        if (sel) wattroff(procWin, A_REVERSE | A_BOLD);
    }

    wrefresh(procWin);
}

void UI::drawStatus(const Process& sel) {
    werase(statusWin);
    wbkgd(statusWin, COLOR_PAIR(C_STATUS));
    wattron(statusWin, COLOR_PAIR(C_STATUS) | A_BOLD);
    mvwprintw(statusWin, 0, 2,
              "Selected: PID %-6d  %-15s  |  [H] Help [Q] Quit",
              sel.pid, sel.name.c_str());
    wattroff(statusWin, COLOR_PAIR(C_STATUS) | A_BOLD);
    wrefresh(statusWin);
}

void UI::draw(const SystemMonitor& mon) {
    getmaxyx(stdscr, termRows, termCols);

    const auto& procs = mon.getProcesses();

    // sync selectedRow from pid
    std::string procName = "";
    for (int i = 0; i < (int)procs.size(); i++) {
        if (procs[i].pid == selectedPid) {
            procName = procs[i].name;
            selectedRow = i;
            break;
        }
    }

    if (showHelp) {
        drawHelp();
        return;   // ← skip everything else
    }

    // close help window if exists

    if (helpWin) {
        delwin(helpWin);
        helpWin = nullptr;
        touchwin(stdscr);
        refresh();
    }
    if (killConfirm){
        drawKill(selectedPid,procName);
        return;
    }

    drawHeader();
    drawDisk(mon.getDiskInfo());
    drawMemory(mon.getMemoryInfo());
    drawCPU(mon.getCPUUsage());
    drawProcessHeader();
    drawProcessList(procs);

    if (!procs.empty() && selectedRow < (int)procs.size())
        drawStatus(procs[selectedRow]);
}

void UI::drawHelp() {
    if (!helpWin) {
        int height = 20, width = 46;
        int y = (termRows - height) / 2;
        int x = (termCols - width)  / 2;
        helpWin = newwin(height, width, y, x);
    }

    werase(helpWin);
    box(helpWin, 0, 0);

    wattron(helpWin, COLOR_PAIR(C_TITLE) | A_BOLD);
    mvwprintw(helpWin, 0, 2, " Help ");
    wattroff(helpWin, COLOR_PAIR(C_TITLE) | A_BOLD);

    wattron(helpWin, A_BOLD);
    mvwprintw(helpWin, 2, 3, "Navigation");
    wattroff(helpWin, A_BOLD);
    mvwprintw(helpWin, 3, 3, "%-14s Scroll process list", "Up / Down");

    wattron(helpWin, A_BOLD);
    mvwprintw(helpWin, 5, 3, "Sorting");
    wattroff(helpWin, A_BOLD);
    mvwprintw(helpWin, 6, 3, "%-14s Sort by CPU%%", "c");
    mvwprintw(helpWin, 7, 3, "%-14s Sort by Memory", "m");
    mvwprintw(helpWin, 8, 3, "%-14s Sort by PID", "p");

    wattron(helpWin, A_BOLD);
    mvwprintw(helpWin, 10, 3, "General");
    wattroff(helpWin, A_BOLD);
    mvwprintw(helpWin,11,3,"%-14s Kill Process","k");
    mvwprintw(helpWin, 12, 3, "%-14s Toggle help", "h");
    mvwprintw(helpWin, 13, 3, "%-14s Quit", "q");

    wattron(helpWin, A_BOLD);
    mvwprintw(helpWin, 14, 3, "Info");
    wattroff(helpWin, A_BOLD);
    mvwprintw(helpWin, 15, 3, "%-14s Every 1 second", "Refresh");
    mvwprintw(helpWin, 16, 3, "%-14s Auto on scroll", "Pause");
    mvwprintw(helpWin, 17, 3, "%-14s 1s after last key", "Resume");

    mvwprintw(helpWin, 19, 3, "Press [H] to close");

    wrefresh(helpWin);
}

void UI::drawKill(int pid,std::string procName) {
    if (!killWin) {
        int height = 6, width = 46;
        int y = (termRows - height) / 2;
        int x = (termCols - width)  / 2;
        killWin = newwin(height, width, y, x);
    }
    werase(killWin);
    box(killWin, 0, 0);

    wattron(killWin, COLOR_PAIR(C_TITLE) | A_BOLD);
    mvwprintw(killWin, 0, 2, " Kill Process ");
    wattroff(killWin, COLOR_PAIR(C_TITLE) | A_BOLD);

    mvwprintw(killWin, 1, 2, "Kill PID %-3d %-4s?", pid,procName.c_str());
    mvwprintw(killWin, 3, 14, "[Y] Yes      [N] No");
    wrefresh(killWin);
}