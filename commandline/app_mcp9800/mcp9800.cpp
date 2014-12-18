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
	t100 myT100;
	unsigned char buf[32];	
	int16_t tmp16;
	float temperature;
	
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
		/* Read temperature command */
		buf[0] = 1; 		
		rval = myT100.sendData(buf,32);

		if(rval < 0)
		{
			printf("> sendData err!\n");

			return -1;
		}		
		
		/* Get actual data */
		rval = myT100.readData(buf,32);

		if(rval < 0)
		{
			printf("> readData err!\n");

			return -1;
		}

		/* Convert raw data to celsius */
		tmp16 = (buf[4] << 8) | buf[5];
		tmp16 = tmp16 >> 4;
		temperature = tmp16 * 0.0625;

		printf("> Temperature: %.3f celsius\n",temperature);
		
		/* 1 seconds delay */
		sleep(1);
	}

	return 0;	
}
