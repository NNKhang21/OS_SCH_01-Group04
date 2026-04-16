#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>

using namespace std;

// ==========================================
// LỚP PROCESS
// ==========================================
class Process {
public:
    int pid;
    int arrival;
    int burst;
    int waiting;
    int turnaround;
    int completion;

    Process(int id = 0, int a = 0, int b = 0)
        : pid(id), arrival(a), burst(b), waiting(0), turnaround(0), completion(0) {}
};

// ==========================================
// LỚP CƠ SỞ SCHEDULER (abstract)
// ==========================================
class Scheduler {
protected:
    vector<Process> processes;
    double totalWT;
    double totalTAT;
    string algoName;

public:
    Scheduler(string name, const vector<Process>& p)
        : processes(p), totalWT(0), totalTAT(0), algoName(name) {}

    virtual ~Scheduler() = default;

    virtual void execute() = 0;

    const vector<Process>& getProcesses() const { return processes; }
    double getAverageWT()  const { return processes.empty() ? 0.0 : totalWT  / processes.size(); }
    double getAverageTAT() const { return processes.empty() ? 0.0 : totalTAT / processes.size(); }
    string getAlgoName()   const { return algoName; }
};

#endif // SCHEDULER_H
