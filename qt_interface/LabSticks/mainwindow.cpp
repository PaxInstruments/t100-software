#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "movAvg.h"
#include <QTabWidget>
#include <QMessageBox>
#include "TableModel.h"
#include "t100Helper.h"
#include <QStandardPaths>

QTimer *timer;
QFile m_logFile;
bool m_logRunning;
bool m_readyToLog;
QTimer *timer_1sec;
QString m_previousMessage;
QDateTime logStart;
bool logADC = false;
bool logColdJunction = false;
MovAvg graphFilter(8);
QString m_logDirectory;
int m_currentTempUnit;
int m_blinkIndex;
int m_blinkState = 0;
QVector<t100*> t100_list;
QString m_logMessage;
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

#ifdef LINUX
    QFont font = ui->centralWidget->font();
    font.setPointSize(10);
    ui->centralWidget->setFont(font);
#endif

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

    ui->indicationLed_comboBox->addItem("N/A");

    for(int i=0;i<t100Helper_getDeviceCount();i++)
    {
        ui->logTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
        ui->graphTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
        ui->indicationLed_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
    }

    ui->logTab_comboBox->setCurrentIndex(0);
    ui->graphTab_comboBox->setCurrentIndex(0);
    ui->indicationLed_comboBox->setCurrentIndex(0);

    /* Log UI settings */
    ui->logTab_lcdNumber->display("00:00:00");
    ui->logTab_textEdit->moveCursor (QTextCursor::End);
    ui->logTab_textEdit->insertPlainText ("Ready ...\n");
    ui->logTab_textEdit->moveCursor (QTextCursor::End);

    m_readyToLog = false;
    m_blinkIndex = -1;
    m_logRunning = false;
    m_currentTempUnit = T100_CELCIUS;
    m_plotHistoryLength = 150;
    m_previousMessage = "...";

    QStringList documentLocations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    m_logDirectory = documentLocations.first();

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
        QString logText;
        QTextStream out(&m_logFile);
        int deviceIndex = ui->logTab_comboBox->currentIndex();
        QString timeData = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz");

        float t_tmp = t100_list.at(deviceIndex)->getThermocoupleTemperature();
        float j_tmp = t100_list.at(deviceIndex)->getColdJunctionTemperature();
        float adc_mv = t100_list.at(deviceIndex)->getAdcVoltage();

        if(logADC && logColdJunction)
        {
            logText = timeData + ", " + QString::number(t_tmp) + ", " + QString::number(j_tmp) + ", " + QString::number(adc_mv) + "\n";
        }
        else if(logADC)
        {
            logText = timeData + ", " + QString::number(t_tmp) + ", " + QString::number(adc_mv) + "\n";
        }
        else if(logColdJunction)
        {
            logText = timeData + ", " + QString::number(t_tmp) + ", " + QString::number(j_tmp) + "\n";
        }
        else
        {
            logText = timeData + ", " + QString::number(t_tmp) + "\n";
        }

        out << logText;

        ui->logTab_textEdit->moveCursor (QTextCursor::End);
        ui->logTab_textEdit->insertPlainText (logText);
        ui->logTab_textEdit->moveCursor (QTextCursor::End);

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

        if(m_blinkIndex >= 0)
        {
            if(t100_list.at(m_blinkIndex)->isAlive())
            {
                m_blinkState = !m_blinkState;
                t100_list.at(m_blinkIndex)->setSparePin(m_blinkState);
            }
        }


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
    if(m_logRunning)
    {
        QMessageBox::warning(this,"Warning","You must stop recording before rescan",QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        /* Stop the update timer */
        timer->stop();

        /* Do the rescan */
        t100Helper_fillDeviceList(t100_list);

        /* Update row counts based on new devices */
        myTableModel.updateRowCounts();

        ui->logTab_comboBox->clear();
        ui->graphTab_comboBox->clear();
        ui->indicationLed_comboBox->clear();
        ui->indicationLed_comboBox->addItem("N/A");

        for(int i=0;i<t100Helper_getDeviceCount();i++)
        {
            ui->logTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
            ui->graphTab_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
            ui->indicationLed_comboBox->addItem(QString::number(t100_list.at(i)->getMySerialNumber()));
        }

        ui->indicationLed_comboBox->setCurrentIndex(0);

        /* Restart update timer */
        timer->start();
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
        m_readyToLog = false;
        m_logRunning = false;
        timer_1sec->stop();
        ui->logTab_textEdit->moveCursor (QTextCursor::End);
        ui->logTab_textEdit->insertPlainText ("Log ended.\n");
        ui->logTab_textEdit->moveCursor (QTextCursor::End);
        ui->logTab_pushButton->setText("Prepare for Logging");
    }
    else
    {
        if(m_readyToLog == false)
        {
            if(t100Helper_getDeviceCount() > 0)
            {
                QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), m_logDirectory +
                                           "/T100_log_" + QDateTime::currentDateTime().toString("hh.mm.ss_dd.MM.yyyy") + ".csv",
                                           "Text files (*.csv)");

                int last_slash = 0;
                last_slash = fileName.lastIndexOf('/');
                m_logDirectory = fileName.left(last_slash);

                m_logFile.setFileName(fileName);

                if(!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    ui->logTab_textEdit->moveCursor (QTextCursor::End);
                    ui->logTab_textEdit->insertPlainText ("File I/O problem!\n");
                    ui->logTab_textEdit->moveCursor (QTextCursor::End);
                }
                else
                {
                    bool ok;

                    m_logMessage = QInputDialog::getText(this, tr("Log Message"),
                                                            tr("Log message:"), QLineEdit::Normal,
                                                            m_previousMessage, &ok);

                    m_previousMessage = m_logMessage;

                    ui->logTab_pushButton->setText("Start Logging");

                    m_readyToLog = true;
                }
            }
            else
            {
                ui->logTab_textEdit->moveCursor (QTextCursor::End);
                ui->logTab_textEdit->insertPlainText ("No device to log!\n");
                ui->logTab_textEdit->moveCursor (QTextCursor::End);
            }

        }
        else
        {
            if(t100Helper_getDeviceCount() > 0)
            {
                m_logRunning = true;

                ui->logTab_textEdit->moveCursor (QTextCursor::End);
                ui->logTab_textEdit->insertPlainText ("Log started.\n");
                ui->logTab_textEdit->moveCursor (QTextCursor::End);
                ui->logTab_pushButton->setText("Stop Logging");

                ui->logTab_lcdNumber->display("00:00:00");
                logStart = QDateTime::currentDateTime();

                QTextStream out(&m_logFile);
                QString tempUnitString;
                int deviceIndex = ui->logTab_comboBox->currentIndex();

                logADC = ui->config_logADC_checkBox->isChecked();
                logColdJunction = ui->config_logColdJunction_checkBox->isChecked();

                if(m_currentTempUnit == T100_KELVIN)
                {
                    tempUnitString = " (K)";
                }
                if(m_currentTempUnit == T100_FAHRENHEIT)
                {
                    tempUnitString = " (F)";
                }
                else
                {
                    tempUnitString = " (C)";
                }

                out << "[Log message]: " + m_logMessage + "\n";
                out << "/ *Some informative text about hardware/software version */\n";
                out << "YYYY-MM-DDThh:mm:ss.sss, ";

                if(logADC && logColdJunction)
                {
                    out << "T<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << ", JT<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << ", ADC<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + "> (mV)";
                    out << "\n";
                }
                else if(logADC)
                {
                    out << "T<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << ", ADC<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + "> (mV)";
                    out << "\n";
                }
                else if(logColdJunction)
                {
                    out << "T<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << ", JT<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << "\n";
                }
                else
                {
                    out << "T<" + QString::number(t100_list.at(deviceIndex)->getMySerialNumber()) + ">";
                    out << tempUnitString;
                    out << "\n";
                }

                timer_1sec->start(1000);
            }
            else
            {
                ui->logTab_textEdit->moveCursor (QTextCursor::End);
                ui->logTab_textEdit->insertPlainText ("No device to log!\n");
                ui->logTab_textEdit->moveCursor (QTextCursor::End);
            }
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

        m_currentTempUnit = T100_FAHRENHEIT;
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

        m_currentTempUnit = T100_KELVIN;
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

        m_currentTempUnit = T100_CELCIUS;
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
    /* Set filter length to _something_ for the graph update */
    graphFilter.setFilterLen(arg1);
}

void MainWindow::on_config_movAvg_spinBox_2_valueChanged(int arg1)
{
    m_plotHistoryLength = arg1;
    ui->myPlot->graph(0)->clearData();
}

void MainWindow::on_indicationLed_comboBox_currentIndexChanged(int index)
{
    if((m_blinkIndex >= 0) && (t100_list.size() > 0))
    {
        if(t100_list.at(m_blinkIndex)->isAlive())
        {
            t100_list.at(m_blinkIndex)->setSparePin(0);
        }
    }

    m_blinkIndex = index - 1;
}
