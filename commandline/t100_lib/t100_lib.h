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

#define VID 0x2b51
#define PID 0x1002

#define MAXIMUM_T100_DEVICES 64

#define T100_KELVIN 0
#define T100_CELCIUS 1
#define T100_FAHRENHEIT 2

#define T100_TEMP_MAX 999999
#define T100_TEMP_MIN -999999

class t100
{

  bool problem;

  int mcp3421_pgaSet;

  int temperatureUnit;
  
  int t100_totalDevices;

  uint64_t mySerialNumber;

  hid_device* t100_handle;

  int thermocoupleTypeInd;

  uint64_t t100_deviceSerials[MAXIMUM_T100_DEVICES];

  uint8_t tempBuffer[256];

  uint8_t internalBuffer[32];

  private:

  /**
   * Private method for Celsius to Kelvin conversion
   *
   * @param Temperature in Celcius
   * @return Temperature in Kelvin
   */
  float celsiusToKelvin(float tCelcius);

  /**
   * Private method for Celsius to Fahreneit conversion
   *
   * @param Temperature in Celcius
   * @return Temperature in Fahreneit
   */
  float celsiusToFahreneit(float tCelcius);

  /**
    * Use this to check the MCP9800 temperature data
    *
    * @param (none)
    * @return Cold junction temperature value in celsius
    */
  float getColdJunctionTemperature_celsius();

  public:
  
  /**
   * Simple Constructor
   *
   * @param (none)
   * @return (none)
   */
  t100();

  /**
   * Initialise member variables
   *
   * @param (none)
   * @return (none)
   */
  void init();

  /**
   * Disconnect from the current device.
   *
   * @param (none)
   * @return (none)
   */
  void disconnect();

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
  int connectBySerial(uint64_t serial);

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
  uint64_t getSerialNumber(uint8_t arrayIndex);

  /**
    * Use this for personal identificaiton.
    * 
    * @param (none)
    * @return Serial number of the currently connected device. If there is no 
    * connection, function returns -1
    */
  uint64_t getMySerialNumber();

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
    * Use this to check whether device is connected.
    *
    * @param (none)
    * @return Connection health status
    */
  bool isAlive();

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
    * Sets the temperature unit.
    *
    * @param type Temperature unit
    * @return 0 for success and -1 for wrong thermocouple type.
    */
  int setTemperatureUnit(int type);

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
