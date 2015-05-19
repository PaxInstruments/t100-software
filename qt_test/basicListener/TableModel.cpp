
#include "TableModel.h"

TableModel::TableModel(QObject *parent):QAbstractTableModel(parent)
{

}

void TableModel::updateData()
{
    emit dataChanged(index(0,0),index(t100Helper_getDeviceCount(),3));
}

void TableModel::setDeviceList(QVector<t100*> &deviceList)
{
    m_deviceList = &deviceList;
}

void TableModel::setCurrentRowCounts()
{
    m_currentRowCount = t100Helper_getDeviceCount();
}

void TableModel::updateRowCounts()
{
    int newCount = t100Helper_getDeviceCount();

    if(newCount < m_currentRowCount)
    {
        beginRemoveRows(QModelIndex(),newCount,m_currentRowCount-1);
        endRemoveRows();
    }
    else if(newCount > m_currentRowCount)
    {
        beginInsertRows(QModelIndex(),m_currentRowCount,newCount-1);
        endInsertRows();
    }

    m_currentRowCount = newCount;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return t100Helper_getDeviceCount();
}

void TableModel::setCurrentColumnCounts(int columnCount)
{
    bool removed = false;

    if(m_currentColumnCount > columnCount)
    {
        removed = true;
    }

    m_currentColumnCount = columnCount;

    if(removed)
    {
        beginRemoveColumns(QModelIndex(),0,0);
        removeColumn(m_currentColumnCount-1);
        endRemoveColumns();
    }
    else
    {
        beginInsertColumns(QModelIndex(),0,0);
        insertColumn(m_currentColumnCount-1);
        endInsertColumns();
    }
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
      
    return m_currentColumnCount;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        QString tmp;

        switch(section)
        {
            case 0: /* ID */
            {
                 tmp = "ID";
                 break;
            }
            case 1: /* Cold Junction Temperature */
            {
                tmp = "Cold Junction";
                break;
            }
            case 2: /* Thermocouple Temperature*/
            {
                tmp = "Thermocouple";
                break;
            }
            case 3: /* Raw ADC reading */
            {
                tmp = "ADC (mV)";
                break;
            }
        }

        return tmp;
    }
    else if((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
    {
        return QString::number(section);
    }

    return QVariant();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
       QString tmp = "...";

       t100* currentDevice = m_deviceList->at(index.row());

       if(currentDevice->periodicUpdate() == 0)
       {
           switch(index.column())
           {
               case 0: /* ID */
               {
                    tmp = QString::number(currentDevice->getMySerialNumber());
                    break;
               }
               case 1: /* Cold Junction Temperature */
               {
                    tmp = QString::number(currentDevice->getColdJunctionTemperature());
                    break;
               }
               case 2: /* Thermocouple Temperature */
               {
                    double temperature = currentDevice->getThermocoupleTemperature();

                    if(temperature == T100_TEMP_MAX)
                    {
                        tmp = "Highest!";
                    }
                    else if(temperature == T100_TEMP_MIN)
                    {
                        tmp = "Lowest!";
                    }
                    else
                    {
                        tmp = QString::number(temperature);
                    }
                    break;
               }
               case 3: /* Raw ADC reading */
               {
                    tmp = QString::number(currentDevice->getAdcVoltage());
                    break;
               }
           }
       }
       else
       {
           tmp = "N/A";
       }

       return tmp;
    }

    return QVariant();
}
