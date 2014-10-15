/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "t100_lib.h"
/*---------------------------------------------------------------------------*/
uint8_t tempBuffer[256];
/*---------------------------------------------------------------------------*/
t100::t100()
{	
	this->t100_handle = NULL;

	this->t100_totalDevices = 0;
}
/*---------------------------------------------------------------------------*/
int t100::connectBasic()
{
	this->t100_handle = hid_open(VID, PID, NULL);
	
	if(!(this->t100_handle))
	{		
 		return -1;
	}	
	else
	{
		return 0;
	}
}
/*---------------------------------------------------------------------------*/
int t100::sendData(uint8_t* buf, uint8_t len)
{
	int rval;

	/* Reset the buffer */
	memset(tempBuffer,0x00,256);

	/* Dummy report ID */
	tempBuffer[0] = 1;

	/* Store the data */
	for(int i = 0; i < len; ++i)
	{
		tempBuffer[i+1] = buf[i];
	}

	/* Send the data */
	rval = hid_send_feature_report(this->t100_handle, tempBuffer, len+1);

	if(rval < 0)
	{
		return -1;
	}

	return (rval-1);
}
/*---------------------------------------------------------------------------*/
int t100::readData(uint8_t* buf,uint8_t len)
{
	int rval;

	/* Reset the buffer */
	memset(tempBuffer,0x00,256);

	/* Read the data */
	rval = hid_get_feature_report(this->t100_handle, tempBuffer, len+1);

	if(rval < 0)
	{
		return -1;
	}
	
	/* Store the data */
	for(int i = 1; i < rval; ++i)
	{
		buf[i-1] = tempBuffer[i];
	}

	return (rval-1);
}
/*---------------------------------------------------------------------------*/
