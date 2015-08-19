#ifndef T100_HELPER_H
#define T100_HELPER_H

#include <QtCore>
#include <QtGui>

#include "t100_lib.h"

void t100Helper_initHardware();
int t100Helper_getDeviceCount();
void t100Helper_periodicUpdate(QVector<t100*> &deviceList);
void t100Helper_fillDeviceList(QVector<t100*> &deviceList);

#endif // T100_HELPER_H
