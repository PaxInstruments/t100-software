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

	int i;
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

	buf[0] = 0; /* Version reading command */	

	rval = myT100.sendData(buf,32);

	if(rval < 0)
	{
		printf("> sendData err!\n");

		return -1;
	}
	else
	{
		printf("> %d bytes data sent.\n",rval);
	}

	/*-----------------------------------------------------------------------*/

	rval = myT100.readData(buf,32);

	if(rval < 0)
	{
		printf("> readData err!\n");

		return -1;
	}
	else
	{
		printf("> %d bytes data read.\n",rval);
	}

	/*-----------------------------------------------------------------------*/

	printf("> Data dump: \n\t");
	for(i = 0; i < rval; i++)
	{
		printf("%02hhx ", buf[i]);
		if(((i+1)%8)==0)
		{
			printf("\n\t");
		}
	}
	printf("\n");
	
	/*-----------------------------------------------------------------------*/

	return 0;	
}
