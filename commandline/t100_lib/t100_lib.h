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

class t100
{
	private:	
	
	hid_device* t100_handle;

	public:
	
	/**
	 * Simple Constructor
	 *
	 * @param (none)
	 * @return (none)
	 */
	t100();

	/**
	  * Tries to connect to first t100 device it could find, without any serial 
	  * number matching.
	  * 
	  * @param (none)  
	  * @return 0 for success and -1 for error
	  */
	int connectBasic();

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
