#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct {
    int id;           // Mã tiến trình 
    int arrival;      // Arrival Time  
    int burst;        // Burst Time    
    int start;        // Start Time   
    int finish;       // Finish Time  
    int waiting;      // Waiting Time 
    int turnaround;   // Turnaround   
} Process;
 
typedef struct {
    Process *procs;   // Mảng các tiến trình
    int n;            // Số tiến trình
    char algo[16];    // Tên thuật toán: "FCFS" | "SJF"
    float avg_wt;     // Waiting time trung bình
    float avg_tat;    // Turnaround time trung bình
} Scheduler;
#












