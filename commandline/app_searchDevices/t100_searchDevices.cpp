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

	return 0;	
}
