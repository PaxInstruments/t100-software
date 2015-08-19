
#include "t100Helper.h"

int deviceCount;
t100 t100_coordinator;

void t100Helper_initHardware()
{
    hid_init();
}

int t100Helper_getDeviceCount()
{
    return deviceCount;
}

t100* createT100()
{
    t100* pt = (t100*) malloc(sizeof(t100));

    return pt;
}

void t100Helper_periodicUpdate(QVector<t100*> &deviceList)
{
    for(int i=0;i<deviceList.size();i++)
    {
        deviceList.at(i)->periodicUpdate();
    }
}

void t100Helper_fillDeviceList(QVector<t100*> &deviceList)
{
    int rval;
    int loopCount;
    int errorCount = 0;

    loopCount = deviceList.size();

    for(int i=0;i<loopCount;i++)
    {
        t100* tmp = deviceList.at(0);

        tmp->disconnect();

        deviceList.remove(0);
    }

    /* Fill the device count in a global array for later disconnect and all. */
    deviceCount = t100_coordinator.searchDevices();    

    for(int i=0;i<deviceCount;i++)
    {
        t100* tmp = createT100();

        tmp->init();

        rval = tmp->connectBySerial(t100_coordinator.getSerialNumber(i));

        if(rval < 0)
        {
            delete tmp;
            errorCount++;
        }
        else
        {            
            /* Default is K thermocouple ... */
            tmp->setThermocoupleType(KType);
            tmp->setPgaGain(8);
            deviceList.append(tmp);
        }                
    }

    deviceCount -= errorCount;
}
