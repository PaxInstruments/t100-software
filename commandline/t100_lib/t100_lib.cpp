/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "t100_lib.h"
/*---------------------------------------------------------------------------*/
t100::t100()
{ 

}
/*---------------------------------------------------------------------------*/
void t100::init()
{
  this->t100_handle = NULL;

  this->t100_totalDevices = 0;

  this->mcp3421_pgaSet = 1;

  this->mySerialNumber = -1;

  setThermocoupleType(KType);

  this->problem = false;

  this->temperatureUnit = T100_CELCIUS;

  memset(internalBuffer,0x00,32);
}
/*---------------------------------------------------------------------------*/
void t100::disconnect()
{
  hid_close(this->t100_handle);
}
/*---------------------------------------------------------------------------*/
int t100::connectBasic()
{
  this->t100_handle = hid_open(VID, PID, NULL);
  
  if(!(this->t100_handle))
  {   
    this->problem = true;
    return -1;
  } 
  else
  {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int t100::connectBySerial(uint64_t serial)
{
  wchar_t buf[16];
  swprintf(buf, sizeof(buf) / sizeof(*buf), L"%llu", serial);

  this->t100_handle = hid_open(VID, PID, buf);

  if(!(this->t100_handle))
  {   
    this->problem = true;
    return -1;
  } 
  else
  {
    this->mySerialNumber = serial;
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
uint64_t t100::getMySerialNumber()
{
    return this->mySerialNumber;
}
/*---------------------------------------------------------------------------*/
int t100::searchDevices()
{ 
  // Enumerate and print the HID devices on the system
  struct hid_device_info *devs, *cur_dev;

  t100_totalDevices = 0;
  
  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs; 
  while (cur_dev) 
  { 
    if((cur_dev->vendor_id == VID) && (cur_dev->product_id == PID))
    {     
      t100_deviceSerials[t100_totalDevices] = _wcstoui64(cur_dev->serial_number,NULL,0);
      t100_totalDevices++;
    }   
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  return t100_totalDevices;
}
/*---------------------------------------------------------------------------*/
uint64_t t100::getSerialNumber(uint8_t arrayIndex)
{
  if(arrayIndex >= t100_totalDevices)
  {
    this->problem = true;
    return -1;
  }
  else
  {
    return t100_deviceSerials[arrayIndex];
  } 
}
/*---------------------------------------------------------------------------*/
int t100::sendData(uint8_t* buf, uint8_t len)
{
  int rval;

  /* Reset the buffer */
  memset(tempBuffer,0x00,256);

#if WIN

  /* Dummy report ID */
  tempBuffer[0] = 0;
  tempBuffer[1] = 1;

  /* Store the data */
  for(int i = 0; i < len; ++i)
  {
    tempBuffer[i+2] = buf[i];
  }

  /* Send the data */
  rval = hid_send_feature_report(this->t100_handle, tempBuffer, len+2);

#else

  /* Dummy report ID */
  tempBuffer[0] = 1;

  /* Store the data */
  for(int i = 0; i < len; ++i)
  {
    tempBuffer[i+1] = buf[i];
  }

  /* Send the data */
  rval = hid_send_feature_report(this->t100_handle, tempBuffer, len+1);

#endif

  if(rval < 0)
  {
    this->problem = true;
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
    this->problem = true;
    return -1;
  }
  
#ifdef LINUX
  /* Store the data */
  for(int i = 2; i < (rval-1); ++i)
  {
    buf[i-2] = tempBuffer[i];
  }  
#endif

#ifdef OSX
  /* Store the data */
  for(int i = 1; i < rval; ++i)
  {
    buf[i-1] = tempBuffer[i];
  }
#endif

#if WIN
  /* Store the data */
  for(int i = 2; i < (rval-1); ++i)
  {
    buf[i-2] = tempBuffer[i];
  }
#endif

  return (rval-1);
}
/*---------------------------------------------------------------------------*/
int t100::periodicUpdate()
{
  int rval;

  /* Get update command */
  memset(internalBuffer,0x00,32);
  internalBuffer[0] = 1;
  rval = sendData(internalBuffer,32);

  if(rval < 0)
  {  
    this->problem = true;
    return -1;
  }   

  /* Get actual data */
  memset(internalBuffer,0x00,32);
  rval = readData(internalBuffer,32);

  if(rval < 0)
  {
    return -1;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
bool t100::isAlive()
{
    bool alive = not this->problem;

    return alive;
}
/*---------------------------------------------------------------------------*/
float t100::getColdJunctionTemperature_celsius()
{
  int16_t tmp16;
  float temperature;

  /* Convert raw data to celsius */
  /* TODO: Add additional eq function */
  tmp16 = (internalBuffer[5] << 8) | internalBuffer[6];
  tmp16 = tmp16 >> 4;
  temperature = tmp16 * 0.0625;

  return temperature;
}
/*---------------------------------------------------------------------------*/
float t100::getColdJunctionTemperature()
{
  int16_t tmp16;
  float temperature;

  /* Convert raw data to celsius */
  /* TODO: Add additional eq function */
  tmp16 = (internalBuffer[5] << 8) | internalBuffer[6];
  tmp16 = tmp16 >> 4;
  temperature = tmp16 * 0.0625;

  /* Return the temperature value in correct unit */
  if(this->temperatureUnit == T100_CELCIUS)
  {
    return temperature;
  }
  else if(this->temperatureUnit == T100_KELVIN)
  {
    return celsiusToKelvin(temperature);
  }
  else if(this->temperatureUnit == T100_FAHRENHEIT)
  {
    return celsiusToFahreneit(temperature);
  }
  else
  {
    return 99999.0; /* error ... */
  }
}
/*---------------------------------------------------------------------------*/
int t100::setTemperatureUnit(int type)
{
  if(type < 3)
  {
    this->temperatureUnit = type;
    return 0;
  }
  else
  {
    return -1;
  }
}
/*---------------------------------------------------------------------------*/
float t100::getAdcVoltage()
{
  uint32_t tmp32;
  int32_t tmpi32;
  float microvolts;
  float milivolts;
  
  /* Make 24bit raw data from 8bit packets */
  tmp32 = internalBuffer[3] + (internalBuffer[2] << 8) + (internalBuffer[1] << 16);

  /* Mask only relevant 18 bits */  
  tmp32 = tmp32 & 0x3FFFF;

  /* 2s compliment signed conversion */
  if (tmp32 & (1<<17))
  {
    tmpi32 = tmp32 - 0x3FFFF;
  }
  else
  {
    tmpi32 = tmp32;
  }

  microvolts = tmpi32 * 15.625 / (float)(this->mcp3421_pgaSet);
  milivolts = microvolts / 1000.0;

  return milivolts;
}
/*---------------------------------------------------------------------------*/
int t100::setPgaGain(uint8_t gain)
{
  int rval;
  uint8_t bitVal;

  if(gain == 1)
  {
    bitVal = 0;
  }
  else if(gain == 2)
  {
    bitVal = 1; 
  }
  else if(gain == 4)
  {
    bitVal = 2;
  }
  else if(gain == 8)
  {
    bitVal = 3;
  }
  else
  {
    return -1;
  }  

  /* Set PGA command */
  memset(internalBuffer,0x00,32);
  internalBuffer[0] = 4;     
  internalBuffer[1] = bitVal;
  rval = sendData(internalBuffer,32);

  if(rval < 0)
  {  
    this->problem = true;
    return -1;
  }

  this->mcp3421_pgaSet = gain;   

  return 0;
}
/*---------------------------------------------------------------------------*/
int t100::setSparePin(int state)
{
    int rval;

    /* GPIO command */
    memset(internalBuffer,0x00,32);
    internalBuffer[0] = 2;

    if(state)
    {
        internalBuffer[1] = 1;
    }
    else
    {
        internalBuffer[1] = 0;
    }

    rval = sendData(internalBuffer,32);

    if(rval < 0)
    {
      this->problem = true;
      return -1;
    }

    return 0;
}
/*---------------------------------------------------------------------------*/
int t100::setThermocoupleType(int type)
{
  /* TODO: Check whether this is a valid type or not! */
  this->thermocoupleTypeInd = type;

  return 0;
}
/*---------------------------------------------------------------------------*/
float t100::celsiusToFahreneit(float tCelcius)
{
  float tReturn;

  tReturn = ((tCelcius * 9.0)/5.0) + 32;

  return tReturn;   
}
/*---------------------------------------------------------------------------*/
float t100::celsiusToKelvin(float tCelcius)
{
  float tReturn;

  tReturn = tCelcius + 273;

  return tReturn;   
}
/*---------------------------------------------------------------------------*/
float t100::getThermocoupleTemperature()
{
  float temperature;
  int partialGroupCount; 
  float thermocouple_mv;
  TCCOEF_TYPEDEF partialCoeff;
  const TCCOEF_TYPEDEF* currentCoeffs;
  
  /* Get the correct data coefficients */
  /* NOTE: We expect a valid 'thermocoupleTypeInd' here. */
  currentCoeffs = ThermoCoeffArray[this->thermocoupleTypeInd].coeff;  
  partialGroupCount = ThermoCoeffArray[this->thermocoupleTypeInd].partialCoeffCount;

  /* Let's get the current thermocouple voltage */
  thermocouple_mv = getAdcVoltage();

  /* Let's check the boundaries of the current thermocouple data type */
  if(thermocouple_mv < currentCoeffs[0].mV_LOW)
  {
    /* Our current TC voltage is lower than the lowest */
    return T100_TEMP_MIN;
  }
  else if(thermocouple_mv > currentCoeffs[(partialGroupCount-1)].mV_HIGH)
  {
    /* Our current TC voltage is higher than the highest */
    return T100_TEMP_MAX;
  }

  /* OK, we are in limits but which partial group should we use? */
  for(int i=0; i<partialGroupCount; i++)
  {
    if((thermocouple_mv >= currentCoeffs[i].mV_LOW) && (thermocouple_mv <= currentCoeffs[i].mV_HIGH))
    {
      /* Store the final coeff we are going to use */
      partialCoeff = currentCoeffs[i]; 
      
      /* Break the loop */
      i = 999; 
    }
  }

  /* Calculate the polynomial result */
  temperature = getColdJunctionTemperature_celsius();
  temperature += partialCoeff.coeff[0];
  for(int i=1; i<10; i++)
  {
    temperature += partialCoeff.coeff[i] * pow(thermocouple_mv,i);
  }

  /* Convert the temperature if neccessary */
  if(this->temperatureUnit == T100_KELVIN)
  {
    temperature = celsiusToKelvin(temperature);
  }
  else if(this->temperatureUnit == T100_FAHRENHEIT)
  {
    temperature = celsiusToFahreneit(temperature);
  }

  return temperature;
}
/*---------------------------------------------------------------------------*/
