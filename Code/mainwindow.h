#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "Scheduler.h"   // Base class + Process
#include "FCFS.h"        // FCFSScheduler
#include "SJF.h"         // SJFScheduler
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAdd_clicked();
    void on_btnSimulate_clicked();
    void on_btnLoadCSV_clicked();
    void on_btnExportCSV_clicked();
    void on_btnClear_clicked();
    void on_btnStressTest_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<Process> processList;
    int currentPid;

    void drawGanttChart(QVBoxLayout* layout, const std::vector<Process>& result, const QString& label);
    void fillResultTable(const std::vector<Process>& result, double avgWT, double avgTAT, int tableIndex);
    void clearGanttArea();
    void appendLog(const QString& msg);
};

#endif // MAINWINDOW_H