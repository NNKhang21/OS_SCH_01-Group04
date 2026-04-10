/*
 * unit_test.cpp
 * ─────────────────────────────────────────────────────────
 * Unit Test — FCFS & SJF (Non-Preemptive)
 *
 * Nguồn ví dụ:
 *   Silberschatz, Galvin, Gagne — "Operating System Concepts"
 *   10th Edition, Chapter 5: CPU Scheduling
 *   - FCFS: Figure 5.3  (trang 205)
 *   - SJF : Figure 5.4  (trang 207)
 *
 * Cách build (g++ standalone, không cần Qt):
 *   g++ -std=c++17 -o unit_test unit_test.cpp
 *   ./unit_test
 * ─────────────────────────────────────────────────────────
 */

/*#include "Scheduler.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// ─────────────────────────────────────────────
// Màu terminal (Windows CMD hỗ trợ từ Win10)
// ─────────────────────────────────────────────
#define CLR_RESET  "\033[0m"
#define CLR_GREEN  "\033[32m"
#define CLR_RED    "\033[31m"
#define CLR_YELLOW "\033[33m"
#define CLR_CYAN   "\033[36m"
#define CLR_BOLD   "\033[1m"

// ─────────────────────────────────────────────
// Cấu trúc lưu kết quả mong đợi (từ sách)
// ─────────────────────────────────────────────
struct ExpectedProcess {
    int pid;
    int completion;
    int turnaround;
    int waiting;
};

// ─────────────────────────────────────────────
// Bộ đếm kết quả test
// ─────────────────────────────────────────────
static int totalTests  = 0;
static int passedTests = 0;
static int failedTests = 0;

// ─────────────────────────────────────────────
// HELPER: In Gantt Chart ra console
// ─────────────────────────────────────────────
void printGanttChart(const vector<Process>& result) {
    cout << CLR_CYAN << "\n  Gantt Chart:\n  " CLR_RESET;

    // Hàng trên: tên process
    for (const auto& p : result) {
        int w = max(4, p.burst * 2);
        cout << "|" << string((w - 2) / 2, ' ')
             << "P" << p.pid
             << string((w - 1) / 2, ' ');
    }
    cout << "|\n  ";

    // Hàng dưới: timeline
    int t = 0;
    // In số đầu tiên
    // Lấy thời điểm bắt đầu process đầu tiên
    if (!result.empty()) {
        t = result[0].completion - result[0].burst;
        cout << t;
    }
    for (const auto& p : result) {
        int w = max(4, p.burst * 2);
        // pad đến cuối block rồi in số kết thúc
        string numStr = to_string(p.completion);
        cout << string(w - (int)numStr.size() + 1, ' ') << p.completion;
    }
    cout << "\n";
}

// ─────────────────────────────────────────────
// HELPER: In bảng kết quả
// ─────────────────────────────────────────────
void printResultTable(const vector<Process>& result) {
    cout << "\n  " << left
         << setw(6)  << "PID"
         << setw(10) << "Arrival"
         << setw(8)  << "Burst"
         << setw(13) << "Completion"
         << setw(13) << "Turnaround"
         << setw(10) << "Waiting" << "\n";
    cout << "  " << string(60, '-') << "\n";
    for (const auto& p : result) {
        cout << "  " << left
             << setw(6)  << ("P" + to_string(p.pid))
             << setw(10) << p.arrival
             << setw(8)  << p.burst
             << setw(13) << p.completion
             << setw(13) << p.turnaround
             << setw(10) << p.waiting << "\n";
    }
}

// ─────────────────────────────────────────────
// HELPER: So sánh 1 trường và in kết quả
// ─────────────────────────────────────────────
bool checkField(const string& label, int got, int expected) {
    totalTests++;
    bool ok = (got == expected);
    if (ok) {
        cout << CLR_GREEN << "PASS" << CLR_RESET
             << " | " << label
             << " = " << got << "\n";
        passedTests++;
    } else {
        cout << CLR_RED << "FAIL" << CLR_RESET
             << " | " << label
             << " → Got: " << got
             << "  Expected: " << expected << "\n";
        failedTests++;
    }
    return ok;
}

bool checkDouble(const string& label, double got, double expected) {
    totalTests++;
    bool ok = (fabs(got - expected) < 0.01);
    if (ok) {
        cout << CLR_GREEN << "PASS" << CLR_RESET
             << " | " << label
             << " = " << fixed << setprecision(2) << got << "\n";
        passedTests++;
    } else {
        cout << CLR_RED << "FAIL" << CLR_RESET
             << " | " << label
             << " → Got: " << fixed << setprecision(2) << got
             << "  Expected: " << fixed << setprecision(2) << expected << "\n";
        failedTests++;
    }
    return ok;
}

// ─────────────────────────────────────────────
// HELPER: Chạy 1 test case
// ─────────────────────────────────────────────
void runTestCase(
    const string& testName,
    const string& bookRef,
    const vector<Process>& input,
    const vector<ExpectedProcess>& expected,
    double expectedAvgWT,
    double expectedAvgTAT,
    bool isSJF)
{
    cout << "\n" << CLR_BOLD << CLR_YELLOW
         << "══════════════════════════════════════════════\n"
         << "  TEST: " << testName << "\n"
         << "  Ref : " << bookRef << "\n"
         << "══════════════════════════════════════════════"
         << CLR_RESET << "\n";

    // In input
    cout << "\n  Input:\n  "
         << left << setw(6) << "PID"
         << setw(10) << "Arrival"
         << setw(8) << "Burst" << "\n";
    cout << "  " << string(24, '-') << "\n";
    for (const auto& p : input) {
        cout << "  " << left
             << setw(6) << ("P" + to_string(p.pid))
             << setw(10) << p.arrival
             << setw(8) << p.burst << "\n";
    }

    // Chạy thuật toán
    vector<Process> result;
    double avgWT, avgTAT;

    if (!isSJF) {
        FCFSScheduler fcfs(input);
        fcfs.execute();
        result  = fcfs.getProcesses();
        avgWT   = fcfs.getAverageWT();
        avgTAT  = fcfs.getAverageTAT();
    } else {
        SJFScheduler sjf(input);
        sjf.execute();
        result  = sjf.getProcesses();
        avgWT   = sjf.getAverageWT();
        avgTAT  = sjf.getAverageTAT();
    }

    // In kết quả tính được
    printResultTable(result);
    printGanttChart(result);

    // Kiểm tra từng process
    cout << "\n  Verification:\n";
    for (const auto& exp : expected) {
        // Tìm process trong result
        for (const auto& p : result) {
            if (p.pid == exp.pid) {
                cout << "  — P" << exp.pid << ":\n";
                checkField("    Completion ", p.completion, exp.completion);
                checkField("    Turnaround ", p.turnaround, exp.turnaround);
                checkField("    Waiting    ", p.waiting,    exp.waiting);
                break;
            }
        }
    }

    // Kiểm tra Average
    cout << "  — Averages:\n";
    checkDouble("    Avg Waiting Time   ", avgWT,  expectedAvgWT);
    checkDouble("    Avg Turnaround Time", avgTAT, expectedAvgTAT);
}

// ─────────────────────────────────────────────
// TEST 1 — FCFS, Figure 5.3 (Silberschatz 10e)
// All arrive at time 0
// P1=24, P2=3, P3=3
// Expected: P1 WT=0, P2 WT=24, P3 WT=27 → Avg WT=17
// ─────────────────────────────────────────────
void test_FCFS_Textbook_Basic() {
    vector<Process> input = {
        Process(1, 0, 24),
        Process(2, 0,  3),
        Process(3, 0,  3),
    };

    // From Silberschatz Fig 5.3:
    // P1: CT=24, TAT=24, WT=0
    // P2: CT=27, TAT=27, WT=24
    // P3: CT=30, TAT=30, WT=27
    // Avg WT = (0+24+27)/3 = 17.0
    // Avg TAT= (24+27+30)/3= 27.0
    vector<ExpectedProcess> expected = {
                                        {1, 24, 24,  0},
                                        {2, 27, 27, 24},
                                        {3, 30, 30, 27},
                                        };

    runTestCase(
        "FCFS — Basic (All arrive at t=0)",
        "Silberschatz OSC 10e, Chapter 5, Figure 5.3 (p.205)",
        input, expected,
        17.0, 27.0,
        false
        );
}

// ─────────────────────────────────────────────
// TEST 2 — FCFS, Convoy Effect (khác arrival)
// P1 arrive=0 burst=8
// P2 arrive=1 burst=4
// P3 arrive=2 burst=2
// ─────────────────────────────────────────────
void test_FCFS_DifferentArrival() {
    vector<Process> input = {
        Process(1, 0, 8),
        Process(2, 1, 4),
        Process(3, 2, 2),
    };

    // FCFS order: P1→P2→P3
    // P1: start=0, CT=8,  TAT=8,  WT=0
    // P2: start=8, CT=12, TAT=11, WT=7
    // P3: start=12,CT=14, TAT=12, WT=10
    // Avg WT = (0+7+10)/3 = 5.67
    // Avg TAT= (8+11+12)/3= 10.33
    vector<ExpectedProcess> expected = {
                                        {1,  8,  8,  0},
                                        {2, 12, 11,  7},
                                        {3, 14, 12, 10},
                                        };

    runTestCase(
        "FCFS — Different Arrival Times (Convoy Effect)",
        "Silberschatz OSC 10e, Chapter 5 — Convoy Effect discussion (p.206)",
        input, expected,
        5.67, 10.33,
        false
        );
}

// ─────────────────────────────────────────────
// TEST 3 — FCFS, CPU Idle gap
// P1 arrive=0  burst=3
// P2 arrive=8  burst=2   ← có khoảng idle từ t=3 đến t=8
// P3 arrive=10 burst=4
// ─────────────────────────────────────────────
void test_FCFS_IdleGap() {
    vector<Process> input = {
        Process(1, 0, 3),
        Process(2, 8, 2),
        Process(3,10, 4),
    };

    // P1: CT=3,  TAT=3,  WT=0
    // P2: CT=10, TAT=2,  WT=0  (CPU idle 3→8, P2 bắt đầu t=8)
    // P3: CT=14, TAT=4,  WT=0
    // Avg WT = 0, Avg TAT = (3+2+4)/3 = 3.0
    vector<ExpectedProcess> expected = {
                                        {1,  3, 3, 0},
                                        {2, 10, 2, 0},
                                        {3, 14, 4, 0},
                                        };

    runTestCase(
        "FCFS — CPU Idle Gap between processes",
        "Derived from Silberschatz OSC 10e, Chapter 5 — idle CPU concept",
        input, expected,
        0.0, 3.0,
        false
        );
}

// ─────────────────────────────────────────────
// TEST 4 — SJF Non-Preemptive, Figure 5.4
// All arrive at time 0
// P1=6, P2=8, P3=7, P4=3
// Shortest first: P4(3)→P1(6)→P3(7)→P2(8)
// ─────────────────────────────────────────────
void test_SJF_Textbook_Basic() {
    vector<Process> input = {
        Process(1, 0, 6),
        Process(2, 0, 8),
        Process(3, 0, 7),
        Process(4, 0, 3),
    };

    // SJF order: P4→P1→P3→P2
    // P4: CT=3,  TAT=3,  WT=0
    // P1: CT=9,  TAT=9,  WT=3
    // P3: CT=16, TAT=16, WT=9
    // P2: CT=24, TAT=24, WT=16
    // Avg WT = (0+3+9+16)/4 = 7.0
    // Avg TAT= (3+9+16+24)/4= 13.0
    vector<ExpectedProcess> expected = {
                                        {4,  3,  3,  0},
                                        {1,  9,  9,  3},
                                        {3, 16, 16,  9},
                                        {2, 24, 24, 16},
                                        };

    runTestCase(
        "SJF Non-Preemptive — Basic (All arrive at t=0)",
        "Silberschatz OSC 10e, Chapter 5, Figure 5.4 (p.207)",
        input, expected,
        7.0, 13.0,
        true
        );
}

// ─────────────────────────────────────────────
// TEST 5 — SJF Non-Preemptive, khác arrival
// P1 arrive=0  burst=7
// P2 arrive=2  burst=4
// P3 arrive=4  burst=1
// P4 arrive=5  burst=4
// ─────────────────────────────────────────────
void test_SJF_DifferentArrival() {
    vector<Process> input = {
        Process(1, 0, 7),
        Process(2, 2, 4),
        Process(3, 4, 1),
        Process(4, 5, 4),
    };

    // t=0: chỉ P1 → chạy P1 (burst=7), CT=7
    // t=7: P2(4), P3(1), P4(4) đều có mặt → chọn P3(1), CT=8
    // t=8: P2(4), P4(4) → tie → chọn P2 (arrival nhỏ hơn = 2), CT=12
    // t=12: P4, CT=16
    // P1: CT=7,  TAT=7,  WT=0
    // P3: CT=8,  TAT=4,  WT=3
    // P2: CT=12, TAT=10, WT=6
    // P4: CT=16, TAT=11, WT=7
    // Avg WT = (0+3+6+7)/4 = 4.0
    // Avg TAT= (7+4+10+11)/4= 8.0
    vector<ExpectedProcess> expected = {
                                        {1,  7,  7, 0},
                                        {3,  8,  4, 3},
                                        {2, 12, 10, 6},
                                        {4, 16, 11, 7},
                                        };

    runTestCase(
        "SJF Non-Preemptive — Different Arrival Times",
        "Silberschatz OSC 10e, Chapter 5 — SJF with different arrivals (p.207-208)",
        input, expected,
        4.0, 8.0,
        true
        );
}

// ─────────────────────────────────────────────
// TEST 6 — SJF, 1 process (edge case)
// ─────────────────────────────────────────────
void test_SJF_SingleProcess() {
    vector<Process> input = {
        Process(1, 5, 10),
    };
    vector<ExpectedProcess> expected = {
                                        {1, 15, 10, 0},
                                        };
    runTestCase(
        "SJF — Edge Case: Single Process",
        "Derived — boundary condition test",
        input, expected,
        0.0, 10.0,
        true
        );
}

// ─────────────────────────────────────────────
// TEST 7 — FCFS, 1 process (edge case)
// ─────────────────────────────────────────────
void test_FCFS_SingleProcess() {
    vector<Process> input = {
        Process(1, 3, 5),
    };
    vector<ExpectedProcess> expected = {
                                        {1, 8, 5, 0},
                                        };
    runTestCase(
        "FCFS — Edge Case: Single Process",
        "Derived — boundary condition test",
        input, expected,
        0.0, 5.0,
        false
        );
}

// ─────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────
int main() {
    cout << CLR_BOLD
         << "\n╔══════════════════════════════════════════════╗\n"
         << "║   CPU SCHEDULING — UNIT TEST SUITE           ║\n"
         << "║   FCFS & SJF (Non-Preemptive)                ║\n"
         << "║   Ref: Silberschatz OSC 10th Edition         ║\n"
         << "╚══════════════════════════════════════════════╝\n"
         << CLR_RESET;

    // ── FCFS Tests ──────────────────────────
    test_FCFS_Textbook_Basic();
    test_FCFS_DifferentArrival();
    test_FCFS_IdleGap();
    test_FCFS_SingleProcess();

    // ── SJF Tests ───────────────────────────
    test_SJF_Textbook_Basic();
    test_SJF_DifferentArrival();
    test_SJF_SingleProcess();

    // ── Tổng kết ────────────────────────────
    cout << "\n" << CLR_BOLD
         << "╔══════════════════════════════════════════════╗\n"
         << "║   TEST SUMMARY                               ║\n"
         << "╠══════════════════════════════════════════════╣\n"
         << CLR_RESET;
    cout << CLR_GREEN << "Passed : " << passedTests << CLR_RESET << "\n";
    cout << CLR_RED   << "Failed : " << failedTests << CLR_RESET << "\n";
    cout << CLR_BOLD  << "Total  : " << totalTests  << CLR_RESET << "\n";
    cout << CLR_BOLD
         << "╚══════════════════════════════════════════════╝\n"
         << CLR_RESET << "\n";

    return (failedTests == 0) ? 0 : 1;
}
*/
