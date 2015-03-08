
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

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
      
    return 4;
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
                tmp = "Cold Junction (°C)";
                break;
            }
            case 2: /* Thermocouple Temperature (°C)*/
            {
                tmp = "Thermocouple (°C)";
                break;
            }
            case 3: /* Raw ADC reading */
            {
                tmp = "ADC Reading (mV)";
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
                    tmp = QString::number(currentDevice->getThermocoupleTemperature());
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
