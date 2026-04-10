#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <algorithm>
#include <limits>
#include <string>

using namespace std;

// LỚP PROCESS
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

// LỚP CƠ SỞ SCHEDULER
class Scheduler {
protected:
    vector<Process> processes;
    double totalWT;
    double totalTAT;
    string algoName;

public:
    // Đã sửa lại thứ tự khởi tạo cho khớp với thứ tự khai báo (Hết báo lỗi vàng)
    Scheduler(string name, const vector<Process>& p)
        : processes(p), totalWT(0), totalTAT(0), algoName(name) {}

    virtual ~Scheduler() = default;

    virtual void execute() = 0;

    const vector<Process>& getProcesses() const { return processes; }
    double getAverageWT() const { return processes.empty() ? 0.0 : totalWT / processes.size(); }
    double getAverageTAT() const { return processes.empty() ? 0.0 : totalTAT / processes.size(); }
    string getAlgoName() const { return algoName; }
};

// LỚP FCFS SCHEDULER
class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(const vector<Process>& p) : Scheduler("FCFS", p) {}

    void execute() override {
        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.arrival < b.arrival;
        });

        int currentTime = 0;

        for (auto& p : processes) {
            if (currentTime < p.arrival) currentTime = p.arrival;

            p.waiting = currentTime - p.arrival;
            currentTime += p.burst;
            p.completion = currentTime;
            p.turnaround = p.completion - p.arrival;

            totalWT += p.waiting;
            totalTAT += p.turnaround;
        }

        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.completion < b.completion;
        });
    }
};

// LỚP SJF SCHEDULER (NON-PREEMPTIVE)
class SJFScheduler : public Scheduler {
public:
    SJFScheduler(const vector<Process>& p) : Scheduler("SJF (Non-Preemptive)", p) {}

    void execute() override {
        int n = processes.size();
        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.arrival < b.arrival;
        });

        int currentTime = 0;
        vector<bool> completed(n, false);
        int completedCount = 0;

        while (completedCount < n) {
            int idx = -1;
            int minBurst = numeric_limits<int>::max();

            for (int i = 0; i < n; i++) {
                if (!completed[i] && processes[i].arrival <= currentTime) {
                    if (processes[i].burst < minBurst) {
                        minBurst = processes[i].burst;
                        idx = i;
                    }
                }
            }

            if (idx == -1) {
                currentTime++;
                continue;
            }

            currentTime += processes[idx].burst;
            processes[idx].completion = currentTime;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;

            totalWT += processes[idx].waiting;
            totalTAT += processes[idx].turnaround;

            completed[idx] = true;
            completedCount++;
        }

        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.completion < b.completion;
        });
    }
};

#endif // SCHEDULER_H
