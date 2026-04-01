#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <string>

using namespace std;
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

class InputHandler {
public:
    static bool readInt(int& x) {
        cin >> x;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return false;
        }
        return true;
    }
};

class Scheduler {
protected:
    vector<Process> processes;
    double totalWT;
    double totalTAT;
    string algoName;

public:
    Scheduler(string name, const vector<Process>& p) 
        : algoName(name), processes(p), totalWT(0), totalTAT(0) {}
    virtual ~Scheduler() = default;

    virtual void execute() = 0;

    void display() const {
        cout << "\n===== " << algoName << " =====\n";
        cout << left << setw(10) << "PID"
             << setw(15) << "Arrival"
             << setw(15) << "Burst"
             << setw(15) << "Completion"
             << setw(15) << "Turnaround"
             << setw(15) << "Waiting" << "\n";

        for (const auto& p : processes) {
            cout << left << setw(10) << p.pid
                 << setw(15) << p.arrival
                 << setw(15) << p.burst
                 << setw(15) << p.completion
                 << setw(15) << p.turnaround
                 << setw(15) << p.waiting << "\n";
        }

        cout << fixed << setprecision(2);
        if (!processes.empty()) {
            cout << "Average WT: " << totalWT / processes.size() << endl;
            cout << "Average TAT: " << totalTAT / processes.size() << endl;
        }
    }
};

// ==========================================
// Thực hiện bởi: Phạm Thái Khang
// ==========================================
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
