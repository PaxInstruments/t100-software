#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:    
    void updateEvent();
    void handleLogEvent();
    void timer1sec_event();
    void on_rescan_pushButton_clicked();
    void on_logTab_pushButton_clicked();
    void on_logTab_radioButton_2_toggled(bool checked);
    void on_graphTab_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
