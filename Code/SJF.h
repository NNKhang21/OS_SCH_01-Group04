#ifndef SJF_H
#define SJF_H
#include "Scheduler.h"
#include <algorithm>
#include <limits>
#include <vector>

class SJFScheduler : public Scheduler {
public:
    SJFScheduler(const vector<Process>& p)
        : Scheduler("SJF (Non-Preemptive)", p) {}

    void execute() override {
        int n = processes.size();

        // Sắp xếp ban đầu theo arrival time
        sort(processes.begin(), processes.end(),
             [](const Process& a, const Process& b) {
                 return a.arrival < b.arrival;
             });

        int currentTime  = 0;
        int completedCount = 0;
        vector<bool> completed(n, false);

        while (completedCount < n) {
            int idx      = -1;
            int minBurst = numeric_limits<int>::max();

            // Tìm process ngắn nhất đã arrived
            for (int i = 0; i < n; i++) {
                if (!completed[i] && processes[i].arrival <= currentTime) {
                    // Tie-breaking: cùng burst → ưu tiên arrival nhỏ hơn
                    if (processes[i].burst < minBurst ||
                        (processes[i].burst == minBurst && idx != -1 &&
                         processes[i].arrival < processes[idx].arrival)) {
                        minBurst = processes[i].burst;
                        idx = i;
                    }
                }
            }

            // Không có process nào ready → CPU idle, nhảy thời gian
            if (idx == -1) {
                currentTime++;
                continue;
            }

            currentTime          += processes[idx].burst;
            processes[idx].completion = currentTime;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            processes[idx].waiting    = processes[idx].turnaround  - processes[idx].burst;

            totalWT  += processes[idx].waiting;
            totalTAT += processes[idx].turnaround;

            completed[idx] = true;
            completedCount++;
        }

        // Sắp xếp lại theo thứ tự hoàn thành để hiển thị
        sort(processes.begin(), processes.end(),
             [](const Process& a, const Process& b) {
                 return a.completion < b.completion;
             });
    }
};

#endif // SJF_H