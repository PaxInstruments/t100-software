#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "TableModel.h"
#include "t100Helper.h"
#include "movAvg.h"

#include <QTabWidget>

QTimer *timer;
QFile m_logFile;
bool m_logRunning;
QTimer *timer_1sec;
QDateTime logStart;
MovAvg graphFilter(8);
QVector<t100*> t100_list;
TableModel myTableModel(0);
int m_plotHistoryLength;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    timer = new QTimer(this);
    timer_1sec = new QTimer(this);

    ui->setupUi(this);

    /* Call this at the start once */
    t100Helper_initHardware();

    /* Do the initial scan */
    t100Helper_fillDeviceList(t100_list);   

    /* Set the data source */
    myTableModel.setDeviceList(t100_list);
    myTableModel.setCurrentRowCounts();
    myTableModel.setCurrentColumnCounts(3);

    ui->tabWidget->setCurrentIndex(0);    

    /* Set the model and show */
    ui->myTableView->setModel(&myTableModel);
    ui->myTableView->show();    
    ui->myTableView->resizeColumnsToContents();
    ui->myTableView->resizeRowsToContents();
    ui->myTableView->horizontalHeader()->setStretchLastSection(true);

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

    /* Set filter length to 8 for the graph update */
    graphFilter.setFilterLen(8);

    for(int i=0;i<t100Helper_getDeviceCount();i++)
    {
        ui->logTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
        ui->graphTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
    }

    ui->logTab_comboBox->setCurrentIndex(0);
    ui->graphTab_comboBox->setCurrentIndex(0);

    /* Log UI settings */
    ui->logTab_lcdNumber->display("00:00:00");
    ui->logTab_textEdit->moveCursor (QTextCursor::End);
    ui->logTab_textEdit->insertPlainText ("Ready ...\n");
    ui->logTab_textEdit->moveCursor (QTextCursor::End);

    m_logRunning = false;
    m_plotHistoryLength = 150;

    /* Update timer */
    connect(timer, SIGNAL(timeout()), this, SLOT(updateEvent()));
    timer->start(300);

    /* Log counter timer */
    connect(timer_1sec, SIGNAL(timeout()), this, SLOT(timer1sec_event()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLogEvent()
{
    if(m_logRunning)
    {
        QTextStream out(&m_logFile);
        int deviceIndex = ui->logTab_comboBox->currentIndex();
        QString timeData = QDateTime::currentDateTime().toString("hh:mm:ss,dd:MM:yyyy");
        float value = t100_list.at(deviceIndex)->getThermocoupleTemperature();

        out << timeData + "," + QString::number(value) + "\n" ;
    }
}

void MainWindow::timer1sec_event()
{
    int diff_ms;
    QTime diff_time(0,0,0);
    timer_1sec->start(1000);

    diff_ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - logStart.toMSecsSinceEpoch();
    diff_time = diff_time.addMSecs(diff_ms);
    ui->logTab_lcdNumber->display(diff_time.toString("hh:mm:ss"));
}

void MainWindow::updateEvent()
{
    timer->start(300);

    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    if(t100Helper_getDeviceCount() > 0)
    {        
        t100Helper_periodicUpdate(t100_list);

        myTableModel.updateData();

        handleLogEvent();

        int deviceIndex = ui->graphTab_comboBox->currentIndex();

        if(t100_list.at(deviceIndex)->isAlive())
        {
            double temperature = t100_list.at(deviceIndex)->getThermocoupleTemperature();

            ui->myTableView->resizeColumnsToContents();
            ui->myTableView->resizeRowsToContents();
            ui->myTableView->horizontalHeader()->setStretchLastSection(true);

            /* Do the rescaling and add +- 0.25 offset to the Y axis of the graph */
            ui->myPlot->rescaleAxes();
            QCPRange rangeVal = ui->myPlot->yAxis->range();
            ui->myPlot->yAxis->setRange(rangeVal.lower - 0.25,rangeVal.upper + 0.25);

            /* Add the new data and replot */
            ui->myPlot->graph(0)->addData(key,graphFilter.execute(temperature));
            ui->myPlot->graph(0)->removeDataBefore(key-m_plotHistoryLength);
            ui->myPlot->replot();

            ui->logTab_statusLabel->setText("Status: Plotting ...");
            ui->logTab_lastReadLabel->setText("Last read: " + QString::number(temperature));
        }
        else
        {
            ui->logTab_statusLabel->setText("Status: Device disconnected?");
        }
    }       
}

void MainWindow::on_rescan_pushButton_clicked()
{    
    /* Do the rescan */
    t100Helper_fillDeviceList(t100_list);

    /* Update row counts based on new devices */
    myTableModel.updateRowCounts();

    ui->logTab_comboBox->clear();
    ui->graphTab_comboBox->clear();

    for(int i=0;i<t100Helper_getDeviceCount();i++)
    {
        ui->logTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
        ui->graphTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
    }
}

void MainWindow::on_logTab_radioButton_2_toggled(bool checked)
{
    ui->logTab_comboBox->setEnabled(checked);
}

void MainWindow::on_graphTab_comboBox_currentIndexChanged(int index)
{
    graphFilter.reset();
    ui->myPlot->graph(0)->clearData();
}

void MainWindow::on_logTab_pushButton_clicked()
{
    if(m_logRunning)
    {
        m_logFile.close();
        m_logRunning = false;
        timer_1sec->stop();
        ui->logTab_textEdit->moveCursor (QTextCursor::End);
        ui->logTab_textEdit->insertPlainText ("Log ended.\n");
        ui->logTab_textEdit->moveCursor (QTextCursor::End);
        ui->logTab_pushButton->setText("Start Logging");
    }
    else
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   "T100_log_" + QDateTime::currentDateTime().toString("hh.mm.ss_dd.MM.yyyy"),
                                   "Text files (*.txt)");

        m_logFile.setFileName(fileName);

        if(!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ui->logTab_textEdit->moveCursor (QTextCursor::End);
            ui->logTab_textEdit->insertPlainText ("File I/O problem!\n");
            ui->logTab_textEdit->moveCursor (QTextCursor::End);
        }
        else
        {
            m_logRunning = true;

            ui->logTab_textEdit->moveCursor (QTextCursor::End);
            ui->logTab_textEdit->insertPlainText ("Log started.\n");
            ui->logTab_textEdit->moveCursor (QTextCursor::End);
            ui->logTab_pushButton->setText("Stop Logging");

            ui->logTab_lcdNumber->display("00:00:00");
            logStart = QDateTime::currentDateTime();

            timer_1sec->start(1000);
        }
    }
}

void MainWindow::on_config_fahr_radioButton_toggled(bool checked)
{
    if(checked)
    {
        for(int i=0;i<t100Helper_getDeviceCount();i++)
        {
            t100_list.at(i)->setTemperatureUnit(T100_FAHRENHEIT);
        }

    }
}

void MainWindow::on_config_kelvin_radioButton_toggled(bool checked)
{
    if(checked)
    {
        for(int i=0;i<t100Helper_getDeviceCount();i++)
        {
            t100_list.at(i)->setTemperatureUnit(T100_KELVIN);
        }

    }
}

void MainWindow::on_config_celcius_radioButton_toggled(bool checked)
{
    if(checked)
    {
        for(int i=0;i<t100Helper_getDeviceCount();i++)
        {
            t100_list.at(i)->setTemperatureUnit(T100_CELCIUS);
        }

    }
}

void MainWindow::on_config_showADC_checkBox_toggled(bool checked)
{
    if(checked)
    {
        myTableModel.setCurrentColumnCounts(4);
        ui->myTableView->resizeColumnsToContents();
        ui->myTableView->resizeRowsToContents();
        ui->myTableView->horizontalHeader()->setStretchLastSection(true);

    }
    else
    {
        myTableModel.setCurrentColumnCounts(3);
        ui->myTableView->resizeColumnsToContents();
        ui->myTableView->resizeRowsToContents();
        ui->myTableView->horizontalHeader()->setStretchLastSection(true);
    }
}

void MainWindow::on_config_movAvg_spinBox_valueChanged(int arg1)
{
    /* Set filter length to 8 for the graph update */
    graphFilter.setFilterLen(arg1);
}

void MainWindow::on_config_movAvg_spinBox_2_valueChanged(int arg1)
{
    m_plotHistoryLength = arg1;
    ui->myPlot->graph(0)->clearData();
}
