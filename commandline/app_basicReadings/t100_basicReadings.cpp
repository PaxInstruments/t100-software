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
	t100 myT100;
	
	if(hid_init())
	{
		return -1;
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

	while(1)
	{
		rval = myT100.periodicUpdate();

		if(rval < 0)
		{
			printf("> periodicUpdate err!\n");
			return -1;
		}

		printf("\n");
		printf("> Cold junction temperature: %.3f C\n",myT100.getColdJunctionTemperature());
		printf("> ADC voltage reading:       %.3f mV\n",myT100.getAdcVoltage());

		/* 500 miliseconds delay */
		usleep(500 * 1000);
	}

	return 0;	
}
