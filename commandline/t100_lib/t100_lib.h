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

#define VID 0x16c0
#define PID 0x05df

#define MAXIMUM_T100_DEVICES 64

class t100
{
	private:	
	
	int t100_totalDevices;

	hid_device* t100_handle;	

	int t100_deviceSerials[MAXIMUM_T100_DEVICES];	

	public:
	
	/**
	 * Simple Constructor
	 *
	 * @param (none)
	 * @return (none)
	 */
	t100();

	/**
	  * Try to connect to first t100 device it could find, without any serial 
	  * number matching.
	  * 
	  * @param (none)
	  * @return 0 for success and -1 for error
	  */
	int connectBasic();

	/**
	  * Try to connect to specific t100 device with special serial number.	  
	  * 
	  * @param serial Specific serial number you want 
	  * @return 0 for success and -1 for error
	  */
	int connectBySerial(int serial);

	/**
	  * Search t100 devices around USB bus and fill t100_deviceSerials array.
	  * 
	  * @param (none)
	  * @return Number of devices found for success and -1 for error
	  */
	int searchDevices();

	/**
	  * Low level data sending function
	  * 
	  * @param buf Data buffer to be sent
	  * @param len Amount of bytes to be sent
	  * @return This function returns the actual number of bytes written and -1 on error
	  */
	int sendData(uint8_t* buf, uint8_t len);

	/**
	  * Low level data receiving function
	  * 
	  * @param buf Data buffer to be filled
	  * @param len Amount of bytes to be read
	  * @return This function returns the actual number of bytes read or -1 on error
	  */
	int readData(uint8_t* buf, uint8_t len);

};
