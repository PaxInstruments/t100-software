#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "TableModel.h"
#include "t100Helper.h"
#include <QTabWidget>

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
    ui->tabWidget->setTabText(2,"Graph");

    ui->tabWidget->setCurrentIndex(0);

    /* Set the model and show */
    ui->myTableView->setModel(&myTableModel);
    ui->myTableView->show();
    ui->myTableView->resizeColumnsToContents();

    /* Don't allow resize */
    this->setFixedSize(this->size());

    ui->myPlot->addGraph();
    ui->myPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->myPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->myPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->myPlot->graph(0)->setAntialiased(true);
    ui->myPlot->graph(0)->setAntialiasedFill(true);

    QFont labelFont("cmu serif", 10, QFont::Light);
    ui->myPlot->xAxis->setLabelFont(labelFont);
    ui->myPlot->yAxis->setLabelFont(labelFont);
    ui->myPlot->xAxis->setTickLabelFont(labelFont);
    ui->myPlot->yAxis->setTickLabelFont(labelFont);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateEvent()));
    timer->start(300);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateEvent()
{
    timer->start(300);

    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    if(t100Helper_getDeviceCount() > 0)
    {
        myTableModel.updateData();

        ui->myTableView->resizeColumnsToContents();

        /* Do the rescaling and add +- 0.25 offset to the Y axis of the graph */
        ui->myPlot->rescaleAxes();        
        QCPRange rangeVal = ui->myPlot->yAxis->range();
        ui->myPlot->yAxis->setRange(rangeVal.lower - 0.25,rangeVal.upper + 0.25);

        /* Add the new data and replot */
        ui->myPlot->graph(0)->addData(key,t100_list.at(0)->getThermocoupleTemperature());
        ui->myPlot->graph(0)->removeDataBefore(key-150);
        ui->myPlot->replot();
    }       
}

void MainWindow::rescan_pushButton_clicked()
{
    /* Do the rescan */
    t100Helper_fillDeviceList(t100_list);

    /* Update row counts based on new devices */
    myTableModel.updateRowCounts();
}
