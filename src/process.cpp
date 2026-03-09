#include"process.h"

std::vector<int> ProcessMonitor::getAllPids(){
    std::vector<int> pids;
    DIR* dir = opendir("/proc");
    struct dirent* entry;
    if(!dir) return pids;
    while((entry = readdir(dir))!=nullptr){
        if(isdigit(entry->d_name[0])){
            pids.push_back(atoi(entry->d_name));
        }
    }

    closedir(dir);
    return pids;
}

unsigned long long ProcessMonitor::readProcessCPU(int pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file) return 0;

    std::string tmp;
    long utime, stime;

    for (int i = 1; i <= 13; ++i)
        file >> tmp;

    file >> utime >> stime;

    return utime + stime; //utime → CPU time in user mode
                         //stime → CPU time in kernel mode
}

long ProcessMonitor::readProcessMemory(int pid){
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");
    std::string line;
    long memory = 0;

    while (getline(file, line)) {
        if (line.find("VmRSS:") == 0) {
            std::stringstream ss(line);
            std::string key, unit;
            ss >> key >> memory >> unit;
            break;
        }
    }

    return memory; // in KB
}

std::string ProcessMonitor::readProcessName(int pid){
    std::ifstream file("/proc/" + std::to_string(pid) + "/comm");
    if(!file) return "";
    std::string name;
    std::getline(file, name);
    return name;
}

std::string ProcessMonitor::readProcessState(int pid) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file) return "?";

    std::string token;
    // Skip PID
    file >> token;

    // Read full name — handles spaces e.g. "(Web Content)"
    std::string fullLine;
    std::getline(file, fullLine);

    // Find last ')' — state is the char after ") "
    size_t pos = fullLine.rfind(')');
    if (pos == std::string::npos || pos + 2 >= fullLine.size())
        return "?";

    // State is the single char after ") "
    return std::string(1, fullLine[pos + 2]);
}
void ProcessMonitor::update() {

    processes.clear();

    std::vector<int> pids = getAllPids();

    std::unordered_map<int, unsigned long long> cpuA;
    std::unordered_map<int, unsigned long long> cpuB;

    CPUStats totalA = readCPUStats();

    for (int pid : pids)
        cpuA[pid] = readProcessCPU(pid); //Save CPU time for each process (snapshot A)

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    CPUStats totalB = readCPUStats();

    for (int pid : pids)
        cpuB[pid] = readProcessCPU(pid); //Save CPU time for each process (snapshot B)

        //This calculates how much total CPU time passed during the interval.
    unsigned long long totalDelta =
        (totalB.user + totalB.nice + totalB.system + totalB.idle +
         totalB.iowait + totalB.irq + totalB.softirq + totalB.steal)
      - (totalA.user + totalA.nice + totalA.system + totalA.idle +
         totalA.iowait + totalA.irq + totalA.softirq + totalA.steal);

    for (int pid : pids) {

        unsigned long long procDelta = cpuB[pid] - cpuA[pid];

        double cpuPercent = 0;
        //This is CPU usage during the 500ms window.
        if (totalDelta != 0)
            cpuPercent = (double)procDelta * 100.0 / totalDelta;

        Process p;

        p.pid = pid;
        p.name = readProcessName(pid);
        p.state = readProcessState(pid);
        p.memoryKB = readProcessMemory(pid);
        p.cpuPercent = cpuPercent;

        processes.push_back(p);
    }
    //This sorts processes descending by CPU usage.
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.cpuPercent > b.cpuPercent;
              });

    if (processes.size() > 10)
        processes.resize(10);
}

const std::vector<Process> &ProcessMonitor::getTopProcesses() const{
    return processes;
}