#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>      // Thêm để vẽ Gantt
#include <QSpacerItem> // Thêm để vẽ Gantt
#include <QLayoutItem> // Thêm để vẽ Gantt

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentPid = 1; // Bắt đầu đếm PID từ 1

    ui->tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// NÚT: THÊM TIẾN TRÌNH
void MainWindow::on_btnAdd_clicked()
{
    int arrival = ui->spinArrival->value();
    int burst = ui->spinBurst->value();

    processList.push_back(Process(currentPid, arrival, burst));

    int row = ui->tableResult->rowCount();
    ui->tableResult->insertRow(row);

    ui->tableResult->setItem(row, 0, new QTableWidgetItem(QString::number(currentPid)));
    ui->tableResult->setItem(row, 1, new QTableWidgetItem(QString::number(arrival)));
    ui->tableResult->setItem(row, 2, new QTableWidgetItem(QString::number(burst)));

    currentPid++;
}

// NÚT: MÔ PHỎNG (CHẠY THUẬT TOÁN + VẼ GANTT)
void MainWindow::on_btnSimulate_clicked()
{
    if (processList.empty()) return;

    int choice = ui->comboAlgo->currentIndex();
    std::vector<Process> result;

    // 1. CHẠY THUẬT TOÁN
    if (choice == 0) {
        FCFSScheduler fcfs(processList);
        fcfs.execute();
        result = fcfs.getProcesses();
    }
    else if (choice == 1) {
        SJFScheduler sjf(processList);
        sjf.execute();
        result = sjf.getProcesses();
    }

    // 2. CẬP NHẬT KẾT QUẢ VÀO BẢNG
    for (size_t i = 0; i < result.size(); i++) {
        for(int r = 0; r < ui->tableResult->rowCount(); r++) {
            int pid_in_table = ui->tableResult->item(r, 0)->text().toInt();
            if (pid_in_table == result[i].pid) {
                ui->tableResult->setItem(r, 3, new QTableWidgetItem(QString::number(result[i].completion)));
                ui->tableResult->setItem(r, 4, new QTableWidgetItem(QString::number(result[i].turnaround)));
                ui->tableResult->setItem(r, 5, new QTableWidgetItem(QString::number(result[i].waiting)));
                break;
            }
        }
    }

    // ==========================================
    // 3. VẼ BIỂU ĐỒ GANTT
    // ==========================================
    // Xóa biểu đồ cũ
    QLayoutItem *child;
    while ((child = ui->ganttLayout->takeAt(0)) != nullptr) { // Sửa ở đây
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    // Vẽ các khối mới
    for (size_t i = 0; i < result.size(); i++) {
        QLabel* label = new QLabel("P" + QString::number(result[i].pid));
        label->setAlignment(Qt::AlignCenter);
        label->setFixedWidth(result[i].burst * 30); // Độ rộng = burst time * 30 pixel
        label->setFixedHeight(40);

        // Style cho khối màu
        label->setStyleSheet("background-color: #87CEEB; border: 1px solid black; font-weight: bold;");

        ui->ganttLayout->addWidget(label); // Sửa ở đây
    }

    // Đẩy các khối sang trái
    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->ganttLayout->addItem(spacer); // Sửa ở đây
    }
