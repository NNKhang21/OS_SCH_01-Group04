# OS_SCH_01-Group04
# OS_SCH_01 — CPU Scheduling Algorithm Demo

> **Môn:** Hệ Điều Hành  
> **Project:** OS_SCH_01 — Mô phỏng thuật toán lập lịch CPU  
> **Thuật toán:** FCFS (First Come First Served) & SJF Non-Preemptive (Shortest Job First)  
> **Ngôn ngữ:** C++ với Qt 6.11 (MinGW 64-bit)  
> **Nền tảng:** Windows  

---

## 📋 Mục lục

- [Giới thiệu](#giới-thiệu)
- [Tính năng](#tính-năng)
- [Cấu trúc project](#cấu-trúc-project)
- [Cách chạy](#cách-chạy)
- [Định dạng file CSV](#định-dạng-file-csv)
- [Kết quả & Proof](#kết-quả--proof)
- [Stress Test & Performance Test](#stress-test--performance-test)
- [Phân công nhóm](#phân-công-nhóm)
- [Video Demo](#video-demo)

---

## Giới thiệu

Ứng dụng GUI mô phỏng và so sánh hai thuật toán lập lịch CPU cơ bản:

- **FCFS** — Tiến trình đến trước, được phục vụ trước. Đơn giản nhưng có thể gây Convoy Effect.
- **SJF Non-Preemptive** — Mỗi khi CPU rảnh, chọn tiến trình có Burst Time ngắn nhất. Tối ưu Average Waiting Time nhưng có thể gây Starvation.

Kết quả được kiểm chứng (proof) với ví dụ từ sách **Silberschatz — Operating System Concepts 10th Edition**.

---

## Tính năng

- Nhập tiến trình thủ công hoặc load từ file **CSV**
- Chạy **FCFS**, **SJF**, hoặc **Run Both** cùng lúc để so sánh
- Hiển thị bảng kết quả: Completion Time, Turnaround Time, Waiting Time, Average WT, Average TAT
- Vẽ **Gantt Chart** trực quan theo thời gian thực
- **Export** kết quả chi tiết ra file CSV vào thư mục `output/`
- **Stress Test** tích hợp: tự động tạo 100 → 10.000 tiến trình ngẫu nhiên và đo thời gian xử lý
- Log hoạt động theo thời gian thực

---

## Cấu trúc project

```
OS_SCH_01/
├── Scheduler.h          # Lớp Process & Scheduler (abstract base)
├── FCFS.h               # Thuật toán FCFS
├── SJF.h                # Thuật toán SJF Non-Preemptive
├── main.cpp             # Điểm khởi động app
├── mainwindow.h         # Khai báo lớp MainWindow
├── mainwindow.cpp       # Logic giao diện & xử lý sự kiện
├── mainwindow.ui        # Thiết kế UI (Qt Designer)
├── untitled1.pro        # Cấu hình build Qt
├── UNIT_TEST.cpp        # Bộ unit test (7 test cases, ref: Silberschatz)
├── FCFS INPUT.csv       # File CSV đầu vào mẫu cho FCFS
├── SJF INPUT.csv        # File CSV đầu vào mẫu cho SJF
└── output/              # Thư mục chứa kết quả xuất ra
    ├── FCFS_<timestamp>.csv
    ├── SJF_<timestamp>.csv
    └── StressTest_<timestamp>.csv
```

---

## Cách chạy

### Yêu cầu
- Windows 10/11 (64-bit)
- Qt 6.11 với MinGW 64-bit (nếu build từ source)

### Chạy trực tiếp (không cần cài Qt)
1. Vào thư mục `build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug/debug/`
2. Double-click `untitled1.exe`

### Build từ source
```bash
# Mở Qt Creator → Open Project → chọn untitled1.pro
# Nhấn Ctrl+R để build và chạy
```

### Sử dụng app
1. **Nhập dữ liệu:** Nhấn `Load CSV` để load file hoặc nhập thủ công rồi nhấn `Add`
2. **Chọn thuật toán:** FCFS / SJF / Run Both trong dropdown
3. **Chạy:** Nhấn `Simulate` → xem kết quả bảng và Gantt Chart
4. **Xuất kết quả:** Nhấn `Export CSV` → file lưu vào thư mục `output/`
5. **Xóa:** Nhấn `Clear` để reset và nhập dữ liệu mới

---

## Định dạng file CSV

### File đầu vào (Input)

```csv
ArrivalTime,BurstTime
0,8
1,4
2,9
3,5
```

> App tự động đánh PID tăng dần từ 1. Không cần cột PID trong file input.

### File đầu ra (Output)

```csv
Algorithm,FCFS
Exported,2026-04-09 11:45:31
Total Processes,20
Average Waiting Time,37.75
Average Turnaround Time,42.50

PID,Arrival Time,Burst Time,Completion Time,Turnaround Time,Waiting Time
1,0,8,8,8,0
2,1,4,12,11,7
...
```

---

## Kết quả & Proof

Kết quả được kiểm chứng với ví dụ từ sách **Silberschatz OSC 10e**.

### FCFS — Hình 5.3 (trang 205)

| PID | Arrival | Burst | Completion | Turnaround | Waiting |
|-----|---------|-------|------------|------------|---------|
| P1  | 0       | 24    | 24         | 24         | 0       |
| P2  | 0       | 3     | 27         | 27         | 24      |
| P3  | 0       | 3     | 30         | 30         | 27      |

**Avg WT = 17.0 | Avg TAT = 27.0** ✅ Khớp với sách

### SJF Non-Preemptive — Hình 5.4 (trang 207)

| PID | Arrival | Burst | Completion | Turnaround | Waiting |
|-----|---------|-------|------------|------------|---------|
| P4  | 0       | 3     | 3          | 3          | 0       |
| P1  | 0       | 6     | 9          | 9          | 3       |
| P3  | 0       | 7     | 16         | 16         | 9       |
| P2  | 0       | 8     | 24         | 24         | 16      |

**Avg WT = 7.0 | Avg TAT = 13.0** ✅ Khớp với sách

### So sánh FCFS vs SJF (dataset 20 processes)

| Thuật toán | Avg Waiting Time | Avg Turnaround Time |
|------------|------------------|---------------------|
| FCFS       | 37.75            | 42.50               |
| SJF        | 25.40            | 30.15               |

SJF giảm Average Waiting Time ~33% so với FCFS trên cùng dataset.

---

## Stress Test & Performance Test

App tích hợp nút **Stress Test** tự động tạo ngẫu nhiên và đo hiệu năng:

| Số process | FCFS (µs) | SJF (µs) |
|------------|-----------|----------|
| 100        | < 1       | < 1      |
| 500        | < 1       | 1–2      |
| 1,000      | 1–2       | 3–5      |
| 5,000      | 5–10      | 60–80    |
| 10,000     | 10–20     | 250–300  |

> Kết quả đo trên máy Windows 10, CPU Intel Core i5. Có thể khác tùy phần cứng.  
> FCFS: O(n log n) — SJF: O(n²) do vòng lặp tìm min burst mỗi bước.

Kết quả stress test được tự động xuất ra file `output/StressTest_<timestamp>.csv`.

---

## Phân công nhóm

| Thành viên | File phụ trách | Vai trò |
|------------|---------------|---------|
| **N.Khang** (Nhóm trưởng) | `Scheduler.h`, `FCFS.h`, `untitled1.exe` | Thiết kế kiến trúc, implement FCFS, quản lý repo |
| **Hải Sơn** | `UNIT_TEST.cpp` | Viết unit test |
| **P.Khang** |`SJF.h`, `main.cpp`, `mainwindow.cpp`| Implement SJF, Logic giao diện, stress test, Gantt chart |
| **P.Khánh** | `mainwindow.h`, `mainwindow.ui`, `untitled1.pro` | Thiết kế UI, cấu hình Qt project |
| **T.Thủy** | `SJF INPUT.csv`, `SJF OUTPUT.csv` | Chuẩn bị dữ liệu SJF, phân tích kết quả |
| **K.Băng** | `FCFS INPUT.csv`, `FCFS OUTPUT.csv` | Chuẩn bị dữ liệu FCFS, so sánh kết quả |

---

## Video Demo

> 🎬 **Link video:** _[Cập nhật sau khi upload lên YouTube]_

Video trình bày toàn bộ tính năng của app, mỗi thành viên trình bày phần mình phụ trách.

---

## Tài liệu tham khảo

- Silberschatz, Galvin, Gagne — *Operating System Concepts*, 10th Edition, Chapter 5
- Qt 6 Documentation — https://doc.qt.io/qt-6/

---

*ClassCode-GroupXX-OS_SCH_01*
