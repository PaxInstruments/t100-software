/*-----------------------------------------------------------------------------
/
/
/
/
/----------------------------------------------------------------------------*/
#include "t100_lib.h"
/*---------------------------------------------------------------------------*/
uint8_t tempBuffer[256];
uint8_t internalBuffer[32];
/*---------------------------------------------------------------------------*/
t100::t100()
{ 
  this->t100_handle = NULL;

  this->t100_totalDevices = 0;

  this->mcp3421_pgaSet = 1;
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
int t100::connectBySerial(int serial)
{
  wchar_t buf[16];
  swprintf(buf, sizeof(buf) / sizeof(*buf), L"%d", serial);

  this->t100_handle = hid_open(VID, PID, buf);

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
      t100_deviceSerials[t100_totalDevices] = wcstoul(cur_dev->serial_number,NULL,0);
      t100_totalDevices++;
    }   
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  return t100_totalDevices;
}
/*---------------------------------------------------------------------------*/
int t100::getSerialNumber(uint8_t arrayIndex)
{
  if(arrayIndex >= t100_totalDevices)
  {
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
int t100::periodicUpdate()
{
  int rval;

  /* Get update command */
  internalBuffer[0] = 1;     
  rval = sendData(internalBuffer,32);

  if(rval < 0)
  {  
    return -1;
  }   

  /* Get actual data */
  rval = readData(internalBuffer,32);

  if(rval < 0)
  {
    return -1;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
float t100::getColdJunctionTemperature()
{
  int16_t tmp16;
  float temperature;

  /* Convert raw data to celsius */
  tmp16 = (internalBuffer[4] << 8) | internalBuffer[5];
  tmp16 = tmp16 >> 4;
  temperature = tmp16 * 0.0625;

  return temperature;
}
/*---------------------------------------------------------------------------*/
float t100::getAdcVoltage()
{
  int32_t tmp32;
  float microvolts;
  float milivolts;

  /* Make 24bit raw data from 8bit packets */
  tmp32 = internalBuffer[2] + (internalBuffer[1] << 8) + (internalBuffer[0] << 16);

  /* Mask only relevant 18 bits */  
  tmp32 = tmp32 & 0x3FFFF;

  /* 2s compliment signed conversion */
  if (tmp32 & (1<<17))
  {
    tmp32 -= 0x3FFFF;
  }

  microvolts = tmp32 * 15.625 / (float)(this->mcp3421_pgaSet);
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
  internalBuffer[0] = 4;     
  internalBuffer[1] = bitVal;
  rval = sendData(internalBuffer,32);

  if(rval < 0)
  {  
    return -1;
  }

  this->mcp3421_pgaSet = gain;   

  return 0;
}
/*---------------------------------------------------------------------------*/
