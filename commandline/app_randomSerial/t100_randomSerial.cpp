/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include "t100_lib.h"

int main(int argc, char* argv[])
{
	/*-----------------------------------------------------------------------*/	
	int i;
	int rval;
	t100 myT100;
	unsigned char buf[32];	
	
	if(hid_init())
	{
		return -1;
	}

	myT100.init();

	time_t serial = time(NULL);

	/* This epoch translates to: 05/19/2015 @ 2:17pm (UTC) */
	serial = serial - 1432045076;

	/* Serial numbers is starting from 1000000000 */
	serial = serial + 1000000000;

	if(myT100.connectBasic() < 0)
	{
		printf("> T100 connection err!\n");
		
		return -1;		
	}
	else
	{
		printf("> T100 connection OK!\n");
	}

	/*-----------------------------------------------------------------------*/
	
	char serialChars[16];
	
	sprintf(serialChars,"%lu",serial);

	/* Change serial number command */
	buf[0] = 3; 
	
	for(i=0;i<10;i++)
	{
		buf[i+1] = serialChars[i];
	}	
	
	rval = myT100.sendData(buf,32);

	printf("> Serial number is set to %s\n",serialChars);

	return 0;	
}
