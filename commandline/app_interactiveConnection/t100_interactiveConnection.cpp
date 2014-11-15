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
#include "t100_lib.h"

int main(int argc, char* argv[])
{
	t100 myT100;
	int index;
	int t100_count;	
	
	
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

		printf("> Enter the device index you want to connect:  ");

		scanf("%d",&index);

		if((index < 0) || (index >= t100_count))
		{
			printf("> Invalid index entry ...\n");
		}
		else
		{
			printf("> Connecting to the T100 device with serial number %d ...\n",myT100.getSerialNumber(index));

			if(myT100.connectBySerial(myT100.getSerialNumber(index)) < 0)
			{
				printf("> T100 connection err!\n");
				
				return -1;		
			}
			else
			{
				printf("> T100 connection OK!\n");
			}
		}
	}
	else
	{
		printf("> No devices has been found!\n");
	}

	return 0;	
}
