#include "widget.h"
#include "ui_widget.h"
#include "Scheduler.h" // Import code thuật toán của bạn
#include <vector>

std::vector<Process> processList;
int currentPid = 1; // Biến đếm PID tự động tăng
