/*-----------------------------------------------------------------------------
/ Basic ADC reading application for T100 based on hid-data example of v-usb
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiddata.h"
#include <unistd.h>
/*---------------------------------------------------------------------------*/
#define VID 0x16c0
#define PID 0x05df
/*---------------------------------------------------------------------------*/
static char *usbErrorMessage(int errCode);
static usbDevice_t *openDevice(void);
/*---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    int err;
    int len;
    int32_t val;
    usbDevice_t *dev;
    uint8_t buffer[33];

    if((dev = openDevice()) == NULL)
    {
        exit(1);
    }

    memset(buffer,0x00,sizeof(buffer));    

    while(1)
    {        
        len = sizeof(buffer);

        /* Dummy report ID */
        buffer[0] = 1;

        /* Command */
        buffer[1] = 1;            

        if((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0)
        {       
             fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
        }   
        
        if((err = usbhidGetReport(dev, 0, buffer, &len)) != 0)
        {
            fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
            exit(1);
        }

        val = (buffer[2]<<16) + (buffer[3]<<8) + (buffer[4]<<0);
        
        if(buffer[2] & (1<<7))
        {
            val = val | (0xFF << 24);
        }

        printf("ch1:  %10d RAW\n",val);        
        printf("ch1:  %10.1f uV\n",val * 4096000.0 / (262144.0) / 8.0);

        val = (buffer[6]<<16) + (buffer[7]<<8) + (buffer[8]<<0);
        
        if(buffer[6] & (1<<7))
        {
            val = val | (0xFF << 24);
        }
        
        printf(".........................\n");
        printf("ch2:  %10d RAW\n",val);
        printf("ch2:  %10.1f uV\n",val * 4096000.0 / (262144.0));
        printf("-------------------------\n");
        printf("-------------------------\n");
        
        usleep(1000 * 100);    
    }
}
/*---------------------------------------------------------------------------*/
static char *usbErrorMessage(int errCode)
{
    static char buffer[80];

    switch(errCode)
    {
        case USBOPEN_ERR_ACCESS:      return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
        case USBOPEN_ERR_IO:          return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }

    return NULL;    /* not reached */
}
/*---------------------------------------------------------------------------*/
static usbDevice_t *openDevice(void)
{
    usbDevice_t *dev = NULL;    
    int err;

    if((err = usbhidOpenDevice(&dev, VID, NULL, PID, NULL, 0)) != 0)
    {
        printf("error finding the device!\n");
        return NULL;
    }

    return dev;
}
/*---------------------------------------------------------------------------*/
