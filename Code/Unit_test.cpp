#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include "Scheduler.h"
#include "FCFS.h"
#include "SJF.h"

using namespace std;

#define CLR_RESET  "\033[0m"
#define CLR_GREEN  "\033[32m"
#define CLR_RED    "\033[31m"
#define CLR_YELLOW "\033[33m"
#define CLR_BOLD   "\033[1m"

struct ExpectedProcess { int pid, completion, turnaround, waiting; };

static int totalTests=0, passedTests=0, failedTests=0;

void printGanttChart(const vector<Process>& result) {
    if (result.empty()) return;
    cout << "\n  Gantt Chart:\n  ";
    for (const auto& p : result) {
        int total = max(5, p.burst * 2);
        string name = "P" + to_string(p.pid);
        int left  = (total - (int)name.size()) / 2;
        int right = total - (int)name.size() - left;
        cout << "|" << string(left,' ') << name << string(right,' ');
    }
    cout << "|\n  ";
    cout << (result[0].completion - result[0].burst);
    for (const auto& p : result) {
        int total = max(5, p.burst * 2);
        string num = to_string(p.completion);
        cout << string(total - (int)num.size() + 1,' ') << num;
    }
    cout << "\n";
}

void printResultTable(const vector<Process>& result) {
    cout << "\n  " << left
         << setw(6)<<"PID" << setw(10)<<"Arrival" << setw(8)<<"Burst"
         << setw(13)<<"Completion" << setw(13)<<"Turnaround" << setw(10)<<"Waiting" << "\n";
    cout << "  " << string(60,'-') << "\n";
    for (const auto& p : result)
        cout << "  " << left
             << setw(6)<<("P"+to_string(p.pid)) << setw(10)<<p.arrival
             << setw(8)<<p.burst << setw(13)<<p.completion
             << setw(13)<<p.turnaround << setw(10)<<p.waiting << "\n";
}

bool checkField(const string& label, int got, int expected) {
    totalTests++;
    bool ok = (got == expected);
    if (ok) { cout<<"    "<<CLR_GREEN<<"PASS"<<CLR_RESET<<" | "<<label<<" = "<<got<<"\n"; passedTests++; }
    else    { cout<<"    "<<CLR_RED<<"FAIL"<<CLR_RESET<<" | "<<label<<" got "<<got<<" expected "<<expected<<"\n"; failedTests++; }
    return ok;
}

bool checkDouble(const string& label, double got, double expected) {
    totalTests++;
    bool ok = fabs(got-expected) < 0.01;
    if (ok) { cout<<"    "<<CLR_GREEN<<"PASS"<<CLR_RESET<<" | "<<label<<" = "<<fixed<<setprecision(2)<<got<<"\n"; passedTests++; }
    else    { cout<<"    "<<CLR_RED<<"FAIL"<<CLR_RESET<<" | "<<label<<" got "<<fixed<<setprecision(2)<<got<<" expected "<<expected<<"\n"; failedTests++; }
    return ok;
}

void runTestCase(const string& name, const string& bookRef,
                 const vector<Process>& input,
                 const vector<ExpectedProcess>& expected,
                 double expAvgWT, double expAvgTAT, bool isSJF)
{
    cout << "\n" << CLR_BOLD << CLR_YELLOW
         << "==========================================\n"
         << "  TEST: " << name << "\n"
         << "  Ref : " << bookRef << "\n"
<< "==========================================" << CLR_RESET << "\n";

    cout << "\n  Input:\n  " << left << setw(6)<<"PID" << setw(10)<<"Arrival" << setw(8)<<"Burst" << "\n";
    cout << "  " << string(24,'-') << "\n";
    for (const auto& p : input)
        cout << "  " << left << setw(6)<<("P"+to_string(p.pid)) << setw(10)<<p.arrival << setw(8)<<p.burst << "\n";

    vector<Process> result;
    double avgWT, avgTAT;
    if (!isSJF) {
        FCFSScheduler fcfs(input);
        fcfs.execute();
        result = fcfs.getProcesses();
        avgWT  = fcfs.getAverageWT();
        avgTAT = fcfs.getAverageTAT();
    } else {
        SJFScheduler sjf(input);
        sjf.execute();
        result = sjf.getProcesses();
        avgWT  = sjf.getAverageWT();
        avgTAT = sjf.getAverageTAT();
    }

    printResultTable(result);
    printGanttChart(result);

    cout << "\n  Verification:\n";
    for (const auto& exp : expected)
        for (const auto& p : result)
            if (p.pid == exp.pid) {
                cout << "  Process P" << exp.pid << ":\n";
                checkField("Completion ", p.completion, exp.completion);
                checkField("Turnaround ", p.turnaround, exp.turnaround);
                checkField("Waiting    ", p.waiting,    exp.waiting);
                break;
            }

    cout << "  Averages:\n";
    checkDouble("Avg Waiting Time   ", avgWT,  expAvgWT);
    checkDouble("Avg Turnaround Time", avgTAT, expAvgTAT);
}

void test_FCFS_Basic() {
    runTestCase("FCFS Basic (All arrive t=0)",
                "Silberschatz OSC 10e, Ch.5, Figure 5.3 (p.205)",
                { Process(1,0,24), Process(2,0,3), Process(3,0,3) },
                { {1,24,24,0}, {2,27,27,24}, {3,30,30,27} },
                17.0, 27.0, false);
}

void test_FCFS_DifferentArrival() {
    runTestCase("FCFS Different Arrival (Convoy Effect)",
                "Silberschatz OSC 10e, Ch.5 Convoy Effect (p.206)",
                { Process(1,0,8), Process(2,1,4), Process(3,2,2) },
                { {1,8,8,0}, {2,12,11,7}, {3,14,12,10} },
                5.67, 10.33, false);
}

void test_FCFS_IdleGap() {
    runTestCase("FCFS CPU Idle Gap",
                "Silberschatz OSC 10e, Ch.5 idle CPU concept",
                { Process(1,0,3), Process(2,8,2), Process(3,10,4) },
                { {1,3,3,0}, {2,10,2,0}, {3,14,4,0} },
                0.0, 3.0, false);
}

void test_FCFS_SingleProcess() {
    runTestCase("FCFS Edge Case Single Process",
                "Derived boundary condition",
                { Process(1,3,5) },
                { {1,8,5,0} },
                0.0, 5.0, false);
}

void test_SJF_Basic() {
    runTestCase("SJF Non-Preemptive Basic (All arrive t=0)",
                "Silberschatz OSC 10e, Ch.5, Figure 5.4 (p.207)",
                { Process(1,0,6), Process(2,0,8), Process(3,0,7), Process(4,0,3) },
                { {4,3,3,0}, {1,9,9,3}, {3,16,16,9}, {2,24,24,16} },
                7.0, 13.0, true);
}
void test_SJF_DifferentArrival() {
    runTestCase("SJF Non-Preemptive Different Arrival Times",
                "Silberschatz OSC 10e, Ch.5 SJF with arrivals (p.207-208)",
                { Process(1,0,7), Process(2,2,4), Process(3,4,1), Process(4,5,4) },
                { {1,7,7,0}, {3,8,4,3}, {2,12,10,6}, {4,16,11,7} },
                4.0, 8.0, true);
}

void test_SJF_SingleProcess() {
    runTestCase("SJF Edge Case Single Process",
                "Derived boundary condition",
                { Process(1,5,10) },
                { {1,15,10,0} },
                0.0, 10.0, true);
}

int main() {
    cout << CLR_BOLD
         << "\n==========================================\n"
         << "  CPU SCHEDULING - UNIT TEST SUITE\n"
         << "  FCFS & SJF (Non-Preemptive)\n"
         << "  Ref: Silberschatz OSC 10th Edition\n"
         << "==========================================" << CLR_RESET << "\n";

    test_FCFS_Basic();
    test_FCFS_DifferentArrival();
    test_FCFS_IdleGap();
    test_FCFS_SingleProcess();
    test_SJF_Basic();
    test_SJF_DifferentArrival();
    test_SJF_SingleProcess();

    cout << "\n" << CLR_BOLD
         << "==========================================\n"
         << "  TEST SUMMARY\n"
         << "==========================================" << CLR_RESET << "\n";
    cout << CLR_GREEN << "  Passed : " << passedTests << CLR_RESET << "\n";
    cout << CLR_RED   << "  Failed : " << failedTests << CLR_RESET << "\n";
    cout << CLR_BOLD  << "  Total  : " << totalTests  << "\n"
         << "==========================================" << CLR_RESET << "\n\n";

    return (failedTests == 0) ? 0 : 1;
}
