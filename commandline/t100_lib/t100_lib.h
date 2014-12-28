/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#ifndef T100_LIB
#define T100_LIB

#include <math.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include "thermocoupleCoefficients.h"

#define VID 0x16c0
#define PID 0x05df

#define MAXIMUM_T100_DEVICES 64

class t100
{
  private:  

  int mcp3421_pgaSet;
  
  int t100_totalDevices;

  hid_device* t100_handle;  

  int thermocoupleTypeInd;

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
    * Access t100_deviceSerials array from your application. 
    * 
    * @param t100_deviceSerials array index
    * @return Serial number of the device. If there isn't any device serial 
    * at that location, function returns -1
    */
  int getSerialNumber(uint8_t arrayIndex);

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

  /**
    * Periodic update function. Call this method to fetch the current temperature 
    * values to internal buffer. Use other methods to create 'human readable' data 
    * from that buffer.
    *
    * @param (none)
    * @return 0 for success and -1 for error
    */
  int periodicUpdate();

  /**
    * Use this to check the MCP9800 temperature data
    *
    * @param (none)
    * @return Cold junction temperature value in celsius
    */
  float getColdJunctionTemperature();

  /**
    * Use this to check the MCP3421 voltage reading. This methods converts the raw
    * data to voltage based on the current PGA setting.
    *
    * @param (none)
    * @return ADC value in milivolts
    */
  float getAdcVoltage();

  /**
    * Sets the PGA value for MCP3421 
    *
    * @param gain PGA set. Valid values are 1, 2, 4 and 8.
    * @return 0 for success and -1 for error
    */
  int setPgaGain(uint8_t gain);

  /**
    * Sets the thermocouple type. ADC Voltage will be interpreted based on the 
    * thermocouple type, so it is important to set the correct one before
    * start measuring.
    *
    * @param type Thermocouple type.
    * @return 0 for success and -1 for wrong thermocouple type.
    */
  int setThermocoupleType(int type);

  /**
    * Main end-user function. Use this to query the temperature of the thermocouple. 
    * Value is calculated using cold junction temperature, ADC readout, ADC gain
    * and thermocouple type.
    *
    * @return Thermocouple temperature in celsius
    */
  float getThermocoupleTemperature();

};

#endif