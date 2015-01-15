/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <unistd.h>
#include "t100_lib.h"

int main(int argc, char* argv[])
{
	int rval;
    int t100_count;
	t100 myT100;
	
	if(hid_init())
	{
		return -1;
	}
    
    t100_count = myT100.searchDevices();
    
    printf("> Total T100 device count accross USB bus: %d\r\n",t100_count);
    
    if(t100_count > 0)
    {
        printf("> Listing serial numbers for each device\r\n");
        for(int i=0; i<t100_count; i++)
        {
            printf("    [%d] Serial Number: %d\r\n",i,myT100.getSerialNumber(i));
        }
    }

	if(myT100.connectBasic() < 0)
	{
		printf("> T100 connection err!\n");
		
		return -1;		
	}
	else
	{
		printf("> T100 connection OK!\n");
	}

	myT100.setPgaGain(8);
	myT100.setThermocoupleType(KType);

	while(1)
	{
		rval = myT100.periodicUpdate();

		if(rval < 0)
		{
			printf("> periodicUpdate err!\n");
			return -1;
		}

		printf("\n\n");
		printf("> Cold junction temperature: %.3f C\n",myT100.getColdJunctionTemperature());		
		printf("> ADC voltage reading:       %.3f mV\n",myT100.getAdcVoltage());
		printf("> Thermocouple temperature:  %.3f C\r\n",myT100.getThermocoupleTemperature());		

		/* 500 miliseconds delay */
		usleep(500 * 3000);
	}

	return 0;	
}
