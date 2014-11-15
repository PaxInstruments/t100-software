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
	/*-----------------------------------------------------------------------*/	
	int rval;
	t100 myT100;
	unsigned char buf[32];	
	
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

	/*-----------------------------------------------------------------------*/

	uint16_t serial;
	char serialChars[8];
	srand(time(NULL));
	serial = rand() % 899;
	serial += 100;

	printf("> Serial number is set to %d\n",serial);

	sprintf(serialChars,"%d",serial);

	/* Change serial number command */
	buf[0] = 3; 

	buf[1] = serialChars[0];
	buf[2] = serialChars[1];
	buf[3] = serialChars[2];

	rval = myT100.sendData(buf,32);

	return 0;	
}
