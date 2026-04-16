#ifndef FCFS_H
#define FCFS_H
#include "Scheduler.h"
#include <algorithm>

class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(const vector<Process>& p)
        : Scheduler("FCFS", p) {}

    void execute() override {
        // Sắp xếp theo arrival time
        std::sort(processes.begin(), processes.end(),
             [](const Process& a, const Process& b) {
                 return a.arrival < b.arrival;
             });

        int currentTime = 0;

        for (auto& p : processes) {
            // Nếu CPU rảnh thì nhảy đến thời điểm process đến
            if (currentTime < p.arrival)
                currentTime = p.arrival;

            p.waiting    = currentTime - p.arrival;
            currentTime += p.burst;
            p.completion = currentTime;
            p.turnaround = p.completion - p.arrival;

            totalWT  += p.waiting;
            totalTAT += p.turnaround;
        }
    }
};

#endif // FCFS_H
