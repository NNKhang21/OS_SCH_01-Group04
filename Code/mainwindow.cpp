#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QFrame>
#include <QSpacerItem>
#include <QLayoutItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>
#include <QScrollBar>
#include <QDateTime>
#include <QColor>
#include <QRandomGenerator>

// ─────────────────────────────────────────────
// CONSTRUCTOR / DESTRUCTOR
// ─────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentPid = 1;

    // Bảng kết quả không cho sửa tay
    ui->tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableResult->horizontalHeader()->setStretchLastSection(true);

    // Nếu có tableResult2 (cho "Run Both")
    if (ui->tableResult2) {
        ui->tableResult2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableResult2->horizontalHeader()->setStretchLastSection(true);
        ui->tableResult2->hide(); // Ẩn mặc định
        ui->labelAvg2->hide();
        ui->labelGantt2->hide();
    }

    appendLog("App started. Add processes or load a CSV file.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─────────────────────────────────────────────
// NÚT: THÊM TIẾN TRÌNH THỦ CÔNG
// ─────────────────────────────────────────────
void MainWindow::on_btnAdd_clicked()
{
    int arrival = ui->spinArrival->value();
    int burst   = ui->spinBurst->value();

    if (burst <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Burst Time phải > 0.");
        return;
    }

    processList.push_back(Process(currentPid, arrival, burst));

    // Thêm vào tableInput (bảng nhập liệu)
    int row = ui->tableInput->rowCount();
    ui->tableInput->insertRow(row);
    ui->tableInput->setItem(row, 0, new QTableWidgetItem(QString::number(currentPid)));
    ui->tableInput->setItem(row, 1, new QTableWidgetItem(QString::number(arrival)));
    ui->tableInput->setItem(row, 2, new QTableWidgetItem(QString::number(burst)));

    appendLog(QString("Added P%1 | Arrival=%2 | Burst=%3")
                  .arg(currentPid).arg(arrival).arg(burst));
    currentPid++;

    // Reset spinbox
    ui->spinArrival->setValue(0);
    ui->spinBurst->setValue(1);
}

// ─────────────────────────────────────────────
// NÚT: TẢI CSV ĐẦU VÀO
// ─────────────────────────────────────────────
void MainWindow::on_btnLoadCSV_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Open CSV File", "", "CSV Files (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file: " + path);
        return;
    }

    QTextStream in(&file);
    QString header = in.readLine(); // Bỏ dòng header

    int loadedCount = 0;
    int lineNum = 1;

    while (!in.atEnd()) {
        lineNum++;
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList cols = line.split(",");
        if (cols.size() < 2) {
            appendLog(QString("Line %1 skipped (not enough columns): %2")
                          .arg(lineNum).arg(line));
            continue;
        }

        bool ok1, ok2;
        int arrival = cols[0].trimmed().toInt(&ok1);
        int burst   = cols[1].trimmed().toInt(&ok2);

        if (!ok1 || !ok2 || burst <= 0) {
            appendLog(QString("Line %1 skipped (invalid values): %2")
                          .arg(lineNum).arg(line));
            continue;
        }

        processList.push_back(Process(currentPid, arrival, burst));

        int row = ui->tableInput->rowCount();
        ui->tableInput->insertRow(row);
        ui->tableInput->setItem(row, 0, new QTableWidgetItem(QString::number(currentPid)));
        ui->tableInput->setItem(row, 1, new QTableWidgetItem(QString::number(arrival)));
        ui->tableInput->setItem(row, 2, new QTableWidgetItem(QString::number(burst)));

        currentPid++;
        loadedCount++;
    }

    file.close();
    appendLog(QString("Loaded %1 processes from: %2").arg(loadedCount).arg(path));
    QMessageBox::information(this, "CSV Loaded",
                             QString("Loaded %1 processes successfully.").arg(loadedCount));
}

// ─────────────────────────────────────────────
// NÚT: MÔ PHỎNG (RUN ALGORITHM)
// ─────────────────────────────────────────────
void MainWindow::on_btnSimulate_clicked()
{
    if (processList.empty()) {
        QMessageBox::warning(this, "No Data", "Please add processes first.");
        return;
    }

    int choice = ui->comboAlgo->currentIndex();  // 0=FCFS, 1=SJF, 2=Run Both

    // Xóa kết quả cũ
    ui->tableResult->setRowCount(0);
    if (ui->tableResult2) ui->tableResult2->setRowCount(0);
    clearGanttArea();

    // ── FCFS ──────────────────────────────────
    if (choice == 0 || choice == 2) {
        FCFSScheduler fcfs(processList);

        QElapsedTimer timer;
        timer.start();
        fcfs.execute();
        qint64 elapsed = timer.nsecsElapsed();

        std::vector<Process> fcfsResult = fcfs.getProcesses();
        double avgWT  = fcfs.getAverageWT();
        double avgTAT = fcfs.getAverageTAT();

        fillResultTable(fcfsResult, avgWT, avgTAT, 0);  // bảng 1
        drawGanttChart(ui->ganttLayout1, fcfsResult, "FCFS");

        appendLog(QString("▶ FCFS | Avg WT=%.2f | Avg TAT=%.2f | Time=%1 ns")
                      .arg(elapsed)
                      .arg(avgWT)
                      .arg(avgTAT));
    }

    // ── SJF ───────────────────────────────────
    if (choice == 1 || choice == 2) {
        SJFScheduler sjf(processList);

        QElapsedTimer timer;
        timer.start();
        sjf.execute();
        qint64 elapsed = timer.nsecsElapsed();

        std::vector<Process> sjfResult = sjf.getProcesses();
        double avgWT  = sjf.getAverageWT();
        double avgTAT = sjf.getAverageTAT();

        int tableIdx = (choice == 2) ? 1 : 0;  // "Run Both" → bảng 2
        fillResultTable(sjfResult, avgWT, avgTAT, tableIdx);
        drawGanttChart(ui->ganttLayout2, sjfResult, "SJF");

        appendLog(QString("▶ SJF  | Avg WT=%.2f | Avg TAT=%.2f | Time=%1 ns")
                      .arg(elapsed)
                      .arg(avgWT)
                      .arg(avgTAT));
    }

    // Hiện bảng 2 nếu Run Both
    if (ui->tableResult2) {
        if (choice == 2) {
            ui->tableResult2->show();
            ui->labelAvg2->show();
            ui->labelGantt2->show();
        } else {
            ui->tableResult2->hide();
            ui->labelAvg2->hide();
            ui->labelGantt2->hide();
        }
    }
}

// ─────────────────────────────────────────────
// NÚT: XUẤT CSV ĐẦU RA
// ─────────────────────────────────────────────
void MainWindow::on_btnExportCSV_clicked()
{
    if (processList.empty()) {
        QMessageBox::warning(this, "No Data", "No processes to export.");
        return;
    }

    // Tạo thư mục output/
    QDir().mkpath("output");

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    int choice = ui->comboAlgo->currentIndex();

    auto exportOne = [&](const QString& algoName,
                         const std::vector<Process>& result,
                         double avgWT, double avgTAT) {
        QString filename = QString("output/%1_%2.csv").arg(algoName).arg(timestamp);
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            appendLog("Cannot write to: " + filename);
            return;
        }
        QTextStream out(&file);

        // Header
        out << "Algorithm," << algoName << "\n";
        out << "Exported," << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
        out << "Total Processes," << result.size() << "\n";
        out << "Average Waiting Time," << QString::number(avgWT, 'f', 2) << "\n";
        out << "Average Turnaround Time," << QString::number(avgTAT, 'f', 2) << "\n\n";

        // Data
        out << "PID,Arrival Time,Burst Time,Completion Time,Turnaround Time,Waiting Time\n";
        for (const auto& p : result) {
            out << p.pid << ","
                << p.arrival << ","
                << p.burst << ","
                << p.completion << ","
                << p.turnaround << ","
                << p.waiting << "\n";
        }

        file.close();
        appendLog("Exported: " + filename);
    };

    // Chạy lại để lấy kết quả xuất
    if (choice == 0 || choice == 2) {
        FCFSScheduler fcfs(processList);
        fcfs.execute();
        exportOne("FCFS", fcfs.getProcesses(), fcfs.getAverageWT(), fcfs.getAverageTAT());
    }
    if (choice == 1 || choice == 2) {
        SJFScheduler sjf(processList);
        sjf.execute();
        exportOne("SJF", sjf.getProcesses(), sjf.getAverageWT(), sjf.getAverageTAT());
    }

    QMessageBox::information(this, "Export Done",
                             "Results exported to output/ folder successfully.");
}

// ─────────────────────────────────────────────
// NÚT: XÓA TẤT CẢ
// ─────────────────────────────────────────────
void MainWindow::on_btnClear_clicked()
{
    processList.clear();
    currentPid = 1;

    ui->tableInput->setRowCount(0);
    ui->tableResult->setRowCount(0);
    if (ui->tableResult2) ui->tableResult2->setRowCount(0);

    clearGanttArea();
    ui->labelAvg1->clear();
    if (ui->labelAvg2) ui->labelAvg2->clear();

    appendLog("Cleared all data.");
}

// ─────────────────────────────────────────────
// NÚT: STRESS TEST
// ─────────────────────────────────────────────
void MainWindow::on_btnStressTest_clicked()
{
    appendLog("═══════════ STRESS TEST STARTED ═══════════");

    QList<int> sizes = {100, 500, 1000, 5000, 10000};

    for (int n : sizes) {
        // Tạo n tiến trình ngẫu nhiên
        std::vector<Process> testList;
        testList.reserve(n);
        for (int i = 1; i <= n; i++) {
            int arrival = QRandomGenerator::global()->bounded(0, 100);
            int burst   = QRandomGenerator::global()->bounded(1, 50);
            testList.emplace_back(i, arrival, burst);
        }

        // Test FCFS
        {
            FCFSScheduler fcfs(testList);
            QElapsedTimer t;
            t.start();
            fcfs.execute();
            qint64 ns = t.nsecsElapsed();
            appendLog(QString("  FCFS | n=%1 | Time=%2 µs | AvgWT=%.2f | AvgTAT=%.2f")
                          .arg(n)
                          .arg(ns / 1000)
                          .arg(fcfs.getAverageWT())
                          .arg(fcfs.getAverageTAT()));
        }

        // Test SJF
        {
            SJFScheduler sjf(testList);
            QElapsedTimer t;
            t.start();
            sjf.execute();
            qint64 ns = t.nsecsElapsed();
            appendLog(QString("  SJF  | n=%1 | Time=%2 µs | AvgWT=%.2f | AvgTAT=%.2f")
                          .arg(n)
                          .arg(ns / 1000)
                          .arg(sjf.getAverageWT())
                          .arg(sjf.getAverageTAT()));
        }
    }

    appendLog("═══════════ STRESS TEST DONE ═══════════");

    // Xuất kết quả stress test ra CSV
    QDir().mkpath("output");
    QString filename = QString("output/StressTest_%1.csv")
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Algorithm,N,Time(us),AvgWT,AvgTAT\n";

        for (int n : sizes) {
            std::vector<Process> testList;
            for (int i = 1; i <= n; i++) {
                int a = QRandomGenerator::global()->bounded(0, 100);
                int b = QRandomGenerator::global()->bounded(1, 50);
                testList.emplace_back(i, a, b);
            }

            {
                FCFSScheduler fcfs(testList);
                QElapsedTimer t; t.start();
                fcfs.execute();
                out << "FCFS," << n << "," << t.nsecsElapsed()/1000
                    << "," << fcfs.getAverageWT()
                    << "," << fcfs.getAverageTAT() << "\n";
            }
            {
                SJFScheduler sjf(testList);
                QElapsedTimer t; t.start();
                sjf.execute();
                out << "SJF," << n << "," << t.nsecsElapsed()/1000
                    << "," << sjf.getAverageWT()
                    << "," << sjf.getAverageTAT() << "\n";
            }
        }
        file.close();
        appendLog("Stress test exported: " + filename);
    }
}

// ─────────────────────────────────────────────
// HELPER: ĐIỀN KẾT QUẢ VÀO BẢNG
// ─────────────────────────────────────────────
void MainWindow::fillResultTable(const std::vector<Process>& result,
                                 double avgWT, double avgTAT, int tableIndex)
{
    QTableWidget* table = (tableIndex == 0) ? ui->tableResult : ui->tableResult2;
    if (!table) return;

    table->setRowCount(0);

    for (const auto& p : result) {
        int row = table->rowCount();
        table->insertRow(row);

        auto item = [](int val) {
            auto* it = new QTableWidgetItem(QString::number(val));
            it->setTextAlignment(Qt::AlignCenter);
            return it;
        };

        table->setItem(row, 0, item(p.pid));
        table->setItem(row, 1, item(p.arrival));
        table->setItem(row, 2, item(p.burst));
        table->setItem(row, 3, item(p.completion));
        table->setItem(row, 4, item(p.turnaround));
        table->setItem(row, 5, item(p.waiting));
    }

    // Hiện Average WT / TAT
    QString avgText = QString("  Avg Waiting Time: <b>%1</b> &nbsp;&nbsp;&nbsp; "
                              "Avg Turnaround Time: <b>%2</b>")
                          .arg(avgWT, 0, 'f', 2)
                          .arg(avgTAT, 0, 'f', 2);

    if (tableIndex == 0)
        ui->labelAvg1->setText(avgText);
    else if (ui->labelAvg2)
        ui->labelAvg2->setText(avgText);
}

// ─────────────────────────────────────────────
// HELPER: VẼ GANTT CHART
// ─────────────────────────────────────────────
void MainWindow::drawGanttChart(QVBoxLayout* layout,
                                const std::vector<Process>& result,
                                const QString& algoLabel)
{
    if (!layout) return;

    // Xóa nội dung cũ
    while (QLayoutItem* child = layout->takeAt(0)) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    // Màu sắc cho mỗi tiến trình
    QList<QString> colors = {
        "#4FC3F7","#81C784","#FFB74D","#F06292",
        "#CE93D8","#80DEEA","#A5D6A7","#FFCC80",
        "#EF9A9A","#90CAF9"
    };

    // ── Hàng khối Gantt ──────────────────────
    QHBoxLayout* barRow = new QHBoxLayout();
    barRow->setSpacing(0);
    barRow->setContentsMargins(0, 0, 0, 0);

    int SCALE = 30; // pixel per time unit (tối đa)

    for (size_t i = 0; i < result.size(); i++) {
        const Process& p = result[i];
        int width = std::max(40, p.burst * SCALE);  // tối thiểu 40px

        QLabel* block = new QLabel(QString("P%1").arg(p.pid));
        block->setAlignment(Qt::AlignCenter);
        block->setFixedSize(width, 44);
        block->setStyleSheet(QString(
                                 "background-color: %1;"
                                 "border: 1px solid #444;"
                                 "font-weight: bold;"
                                 "font-size: 13px;"
                                 "color: #1a1a1a;"
                                 ).arg(colors[i % colors.size()]));
        block->setToolTip(QString("P%1 | Start: %2 | End: %3 | Burst: %4")
                              .arg(p.pid)
                              .arg(p.completion - p.burst)
                              .arg(p.completion)
                              .arg(p.burst));
        barRow->addWidget(block);
    }
    barRow->addStretch();

    // ── Hàng số thời gian ────────────────────
    QHBoxLayout* timeRow = new QHBoxLayout();
    timeRow->setSpacing(0);
    timeRow->setContentsMargins(0, 0, 0, 0);

    // Hiện số đầu (thời điểm bắt đầu mỗi tiến trình)
    int prevEnd = -1;
    for (size_t i = 0; i < result.size(); i++) {
        const Process& p = result[i];
        int start = p.completion - p.burst;
        int width = std::max(40, p.burst * SCALE);

        // Nếu có idle gap
        if (i == 0 || start != prevEnd) {
            QLabel* startLbl = new QLabel(QString::number(start));
            startLbl->setFixedWidth(30);
            startLbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            startLbl->setStyleSheet("font-size: 11px; color: #555;");
            timeRow->addWidget(startLbl);

            // Chiều rộng block trừ đi label số
            QLabel* spacer = new QLabel();
            spacer->setFixedWidth(width - 30);
            timeRow->addWidget(spacer);
        } else {
            QLabel* startLbl = new QLabel(QString::number(start));
            startLbl->setFixedWidth(30);
            startLbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            startLbl->setStyleSheet("font-size: 11px; color: #555;");
            timeRow->addWidget(startLbl);

            QLabel* sp = new QLabel();
            sp->setFixedWidth(width - 30);
            timeRow->addWidget(sp);
        }
        prevEnd = p.completion;
    }

    // Số cuối
    if (!result.empty()) {
        QLabel* endLbl = new QLabel(QString::number(result.back().completion));
        endLbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        endLbl->setStyleSheet("font-size: 11px; color: #555;");
        timeRow->addWidget(endLbl);
    }
    timeRow->addStretch();

    // ── Wrap vào layout chính ─────────────────
    QWidget* ganttWidget = new QWidget();
    QVBoxLayout* vbox = new QVBoxLayout(ganttWidget);
    vbox->setSpacing(2);
    vbox->setContentsMargins(5, 5, 5, 5);

    QLabel* title = new QLabel(QString("Gantt Chart — %1").arg(algoLabel));
    title->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    vbox->addWidget(title);

    QWidget* barWidget = new QWidget();
    barWidget->setLayout(barRow);
    vbox->addWidget(barWidget);

    QWidget* timeWidget = new QWidget();
    timeWidget->setLayout(timeRow);
    vbox->addWidget(timeWidget);

    layout->addWidget(ganttWidget);
}

// ─────────────────────────────────────────────
// HELPER: XÓA GANTT AREA
// ─────────────────────────────────────────────
void MainWindow::clearGanttArea()
{
    for (QVBoxLayout* layout : {ui->ganttLayout1, ui->ganttLayout2}) {
        if (!layout) continue;
        while (QLayoutItem* child = layout->takeAt(0)) {
            if (child->widget()) delete child->widget();
            delete child;
        }
    }
}

// ─────────────────────────────────────────────
// HELPER: GHI LOG VÀO TEXTBOX
// ─────────────────────────────────────────────
void MainWindow::appendLog(const QString& msg)
{
    if (ui->textLog) {
        ui->textLog->append(
            QString("[%1] %2")
                .arg(QTime::currentTime().toString("HH:mm:ss"))
                .arg(msg)
            );
        // Tự cuộn xuống cuối
        ui->textLog->verticalScrollBar()->setValue(
            ui->textLog->verticalScrollBar()->maximum()
            );
    }
}