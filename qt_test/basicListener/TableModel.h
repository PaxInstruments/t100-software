#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>

#include "t100Helper.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        TableModel(QObject *parent);
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;        
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        void updateData();
        void updateRowCounts();
        void setCurrentRowCounts();
        void setDeviceList(QVector<t100*> &deviceList);

    private:        
        QVector<t100*> *m_deviceList;
        int m_currentRowCount;
};

#endif // MAINWINDOW_H
