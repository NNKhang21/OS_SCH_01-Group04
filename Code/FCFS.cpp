#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <string>

using namespace std;

// ===== LỚP PROCESS =====
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

// ===== LỚP XỬ LÝ ĐẦU VÀO =====
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

// ===== LỚP CƠ SỞ SCHEDULER =====
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

// ===== LỚP DẪN XUẤT: FCFS SCHEDULER =====
class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(const vector<Process>& p) : Scheduler("FCFS", p) {}

    void execute() override {
        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.arrival < b.arrival;
        });

        int currentTime = 0;

        for (auto& p : processes) {
            if (currentTime < p.arrival)
                currentTime = p.arrival;

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

// ===== HÀM MAIN CHO FCFS =====
int main() {
    int n;
cout << "--- FCFS SCHEDULING ---\n";
    cout << "Enter number of processes: ";
    if (!InputHandler::readInt(n) || n <= 0) {
        cout << "Invalid number of processes.\n";
        return 1;
    }

    vector<Process> processes;
    processes.reserve(n);

    for (int i = 0; i < n; i++) {
        int arrival, burst;
        cout << "\nProcess " << i + 1 << endl;
        cout << "Arrival Time: ";
        if (!InputHandler::readInt(arrival)) return 1;
        cout << "Burst Time: ";
        if (!InputHandler::readInt(burst)) return 1;

        processes.emplace_back(i + 1, arrival, burst);
    }

    FCFSScheduler fcfs(processes);
    fcfs.execute();
    fcfs.display();

    return 0;
}
