#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Scheduler.h"
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

private:
    Ui::MainWindow *ui;

    // Lưu trữ dữ liệu
    std::vector<Process> processList;
    int currentPid;
};
#endif // MAINWINDOW_H
