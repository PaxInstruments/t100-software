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
	int i;
	int rval;
	int t100_count;
	int tmp_id;
	t100 t100_coordinator;	
	
	if(hid_init())
	{
		return -1;
	}

	t100_count = t100_coordinator.searchDevices();

	if(t100_count > 0)
	{
		printf("> %d t100 device(s) found!\r\n",t100_count);
	}
	else
	{
		printf("> No t100 device found!\n");
		return 0;
	}

	t100* t100_list = new t100[t100_count];

	for(i=0;i<t100_count;i++)
	{
		printf("> Connecting to #%d\n",t100_coordinator.getSerialNumber(i));
		rval = t100_list[i].connectBySerial(t100_coordinator.getSerialNumber(i));
		if(rval < 0)
		{
			printf("> Connection fail for #%d\n",t100_coordinator.getSerialNumber(i));
		}
		else
		{
			t100_list[i].setThermocoupleType(KType);
		}
	}

	while(1)
	{
		for(i=0;i<t100_count;i++)
		{
			rval = t100_list[i].periodicUpdate();

			if(rval < 0)
			{
				printf("> periodicUpdate err!\n");
				return -1;
			}

			printf(">--------------------------------------------------------\n");
			printf("> Readings from: #%d\n",t100_list[i].getMySerialNumber());
			printf("> Cold junction temperature: %.3f C\n",t100_list[i].getColdJunctionTemperature());		
			printf("> ADC voltage reading:       %.3f mV\n",t100_list[i].getAdcVoltage());
			printf("> Thermocouple temperature:  %.3f C\r\n",t100_list[i].getThermocoupleTemperature());		
		}

		/* 500 miliseconds delay */
		usleep(500 * 1000);
	}

	return 0;	
}
