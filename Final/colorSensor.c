#include "colorSensor.h"

// Initialize - returns true if successful
// Starts Wire/I2C Communication
// Verifies sensor is there by checking its device ID
// Resets all registers/configurations to factory default
// Sets configuration registers for the common use case

bool init()
{
  bool ret = true;
  uint8_t data = 0x00;

  // Start I2C
  //Wire.begin();
  fd = wiringPiI2CSetup(ISL_I2C_ADDR);

  // Check device ID
  data = read8(DEVICE_ID);
  if (data != 0x7D)
  {
    ret &= false;
  }

  // Reset registers
  ret &= reset();

  // Set to RGB mode, 10k lux, and high IR compensation
  ret &= config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG_DEFAULT);

  return ret;
}

// Reset all registers - returns true if successful
bool reset()
{
  uint8_t data = 0x00;
  // Reset registers
  write8(DEVICE_ID, 0x46);
  // Check reset
  data = read8(CONFIG_1);
  data |= read8(CONFIG_2);
  data |= read8(CONFIG_3);
  data |= read8(STATUS);
  if (data != 0x00)
  {
    return false;
  }
  return true;
}

// Setup Configuration registers (three registers) - returns true if successful
// Use CONFIG1 variables from SFE_ISL29125.h for first parameter config1, CONFIG2 for config2, 3 for 3
// Use CFG_DEFAULT for default configuration for that register
bool config(uint8_t config1, uint8_t config2, uint8_t config3)
{
  bool ret = true;
  uint8_t data = 0x00;

  // Set 1st configuration register
  write8(CONFIG_1, config1);
  // Set 2nd configuration register
  write8(CONFIG_2, config2);
  // Set 3rd configuration register
  write8(CONFIG_3, config3);

  // Check if configurations were set correctly
  data = read8(CONFIG_1);
  if (data != config1)
  {
    ret &= false;
  }
  data = read8(CONFIG_2);
  if (data != config2)
  {
    ret &= false;
  }
  data = read8(CONFIG_3);
  if (data != config3)
  {
    ret &= false;
  }
  return ret;
}

// Sets upper threshold value for triggering interrupts
void setUpperThreshold(uint16_t data)
{
  write16(THRESHOLD_HL, data);
}

// Sets lower threshold value for triggering interrupts
void setLowerThreshold(uint16_t data)
{
  write16(THRESHOLD_LL, data);
}

// Check what the upper threshold is, 0xFFFF by default
uint16_t readUpperThreshold()
{
  return read16(THRESHOLD_HL);
}

// Check what the upper threshold is, 0x0000 by default
uint16_t readLowerThreshold()
{
  return read16(THRESHOLD_LL);
}

// Read the latest Sensor ADC reading for the color Red
uint16_t readRed()
{
  return read16(RED_L);
}

// Read the latest Sensor ADC reading for the color Green
uint16_t readGreen()
{
  return read16(GREEN_L);
}

// Read the latest Sensor ADC reading for the color Blue
uint16_t readBlue()
{
  return read16(BLUE_L);
}

// Check status flag register that allows for checking for interrupts, brownouts, and ADC conversion completions
uint8_t readStatus()
{
  return read8(STATUS);
}

// Generic I2C read register (single byte)
uint8_t read8(uint8_t reg)
{
/*
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();
//  Wire.beginTransmission(_addr);
  Wire.requestFrom(_addr,(uint8_t)1);
  uint8_t data = Wire.read();
//  Wire.endTransmission();
*/
  uint8_t data = wiringPiI2CReadReg8 (fd, reg);

  return data;
}

// Generic I2C write data to register (single byte)
void write8(uint8_t reg, uint8_t data)
{
/*
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
*/
  wiringPiI2CWriteReg8 (fd, reg, data);

  return;
}

// Generic I2C read registers (two bytes, LSB first)
uint16_t read16(uint8_t reg)
{
/*
  uint16_t data = 0x0000;

  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();

//  Wire.beginTransmission(_addr);
  Wire.requestFrom(_addr, (uint8_t)2); // request 2 bytes of data
  data = Wire.read();
  data |= (Wire.read() << 8);
//  Wire.endTransmission();
*/
  uint16_t data = wiringPiI2CReadReg16 (fd, reg);

  return data;
}

// Generic I2C write data to registers (two bytes, LSB first)
void write16(uint8_t reg, uint16_t data)
{
/*
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.write(data>>8);
  Wire.endTransmission();
  */
  wiringPiI2CWriteReg16 (fd, reg, data);
}
