#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "TableModel.h"
#include "t100Helper.h"

QTimer *timer;
QVector<t100*> t100_list;
TableModel myTableModel(0);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    timer = new QTimer(this);
    ui->setupUi(this);

    /* Call this at the start once */
    t100Helper_initHardware();

    /* Do the initial scan */
    t100Helper_fillDeviceList(t100_list);

    /* Set the data source */
    myTableModel.setDeviceList(t100_list);
    myTableModel.setCurrentRowCounts();

    ui->tabWidget->setTabText(0,"Reading");
    ui->tabWidget->setTabText(1,"Config");

    /* Set the model and show */
    ui->myTableView->setModel(&myTableModel);
    ui->myTableView->show();
    ui->myTableView->resizeColumnsToContents();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateEvent()));
    timer->start(500);   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateEvent()
{
    timer->start(500);

    if(t100Helper_getDeviceCount() > 0)
    {
        myTableModel.updateData();

        ui->myTableView->resizeColumnsToContents();
    }
}

void MainWindow::rescan_pushButton_clicked()
{
    /* Do the rescan */
    t100Helper_fillDeviceList(t100_list);

    /* Update row counts based on new devices */
    myTableModel.updateRowCounts();
}
