/*!
 *  @file Adafruit_INA228.cpp
 *
 *  @section ina228_intro Introduction
 *
 * 	I2C Driver for the INA228 I2C Current and Power sensor
 *
 * 	This is a library for the Adafruit INA228 breakout:
 * 	http://www.adafruit.com/products/5832
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section ina228_dependencies Dependencies
 *
 *  This library depends on the Adafruit BusIO library
 *
 *  @section ina228_author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section ina228_license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section ina228_history HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_INA228.h"

#include <Wire.h>

#include "Arduino.h"

/*!
 *    @brief  Instantiates a new INA228 class
 */
Adafruit_INA228::Adafruit_INA228(void) {}

/*!
 *    @brief  Sets up the HW
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  theWire
 *            The Wire object to be used for I2C connections.
 *    @param  skipReset
 *            When set to true, will omit resetting all INA228 registers to
 *            their default values. Default: false.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_INA228::begin(uint8_t i2c_address, TwoWire* theWire,
                            bool skipReset) {
  if (!Adafruit_INA2xx::begin(i2c_address, theWire, skipReset)) {
    return false;
  }

  // make sure we're talking to the right chip
  if (_device_id != INA228_DEVICE_ID) {
    return false;
  }

  return true;
}

/**************************************************************************/
/*!
    @brief Resets the hardware specific to INA228
*/
/**************************************************************************/
void Adafruit_INA228::reset(void) {
  // Perform base class reset
  Adafruit_INA2xx::reset();
  // Any additional INA228 specific reset actions would go here
}

/**************************************************************************/
/*!
    @brief Updates the shunt calibration value to the INA228 register.
*/
/**************************************************************************/
void Adafruit_INA228::_updateShuntCalRegister() {
  float scale = 1;
  if (getADCRange()) {
    scale = 4;
  }
  float shunt_cal = 13107.2 * 1000000.0 * _shunt_res * _current_lsb * scale;

  Adafruit_I2CRegister shunt =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_SHUNTCAL, 2, MSBFIRST);
  shunt.write(shunt_cal);
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Energy register.
    @return The current Energy calculation in Joules
*/
/**************************************************************************/
float Adafruit_INA228::readEnergy(void) {
  Adafruit_I2CRegister energy =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_ENERGY, 5, MSBFIRST);
  uint8_t buff[5];
  energy.read(buff, 5);
  float e = 0;
  for (int i = 0; i < 5; i++) {
    e *= 256;
    e += buff[i];
  }
  return (float)e * 16 * 3.2 * _current_lsb;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Charge register.
    @return The current Charge calculation in Coulombs
*/
/**************************************************************************/
float Adafruit_INA228::readCharge(void) {
  Adafruit_I2CRegister charge =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_CHARGE, 5, MSBFIRST);
  uint8_t buff[5];
  charge.read(buff, 5);

  // Convert 40-bit two's complement value
  int64_t c = 0;
  for (int i = 0; i < 5; i++) {
    c = (c << 8) | buff[i];
  }

  // Handle sign extension for 40-bit two's complement
  if (c & ((int64_t)1 << 39)) {
    c |= 0xFFFFFF0000000000; // Sign extend to 64 bits
  }

  return (float)c * _current_lsb;
}

/**************************************************************************/
/*!
    @brief Returns the current alert type
    @return The current alert type
*/
/**************************************************************************/
INA228_AlertType Adafruit_INA228::getAlertType(void) {
  Adafruit_I2CRegisterBits alert_type =
      Adafruit_I2CRegisterBits(Diag_Alert, 6, 8);
  return (INA228_AlertType)alert_type.read();
}

/**************************************************************************/
/*!
    @brief Sets a new alert type
    @param alert
           The new alert type to be set
*/
/**************************************************************************/
void Adafruit_INA228::setAlertType(INA228_AlertType alert) {
  Adafruit_I2CRegisterBits alert_type =
      Adafruit_I2CRegisterBits(Diag_Alert, 6, 8);
  alert_type.write(alert);
}

/**************************************************************************/
/*!
    @brief Resets the energy and charge accumulators
*/
/**************************************************************************/
void Adafruit_INA228::resetAccumulators(void) {
  Adafruit_I2CRegisterBits reset_accumulators =
      Adafruit_I2CRegisterBits(Config, 1, 14);
  reset_accumulators.write(1);
}

/**************************************************************************/
/*!
    @brief Reads the die temperature with the INA228-specific conversion factor
    @return The current die temp in deg C
*/
/**************************************************************************/
float Adafruit_INA228::readDieTemp(void) {
  Adafruit_I2CRegister temp =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_DIETEMP, 2, MSBFIRST);
  int16_t t = temp.read();
  // INA228 uses 16 bits for temperature with 7.8125 m°C/LSB
  return (float)t * 7.8125 / 1000.0;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Bus Voltage register
           using INA228-specific conversion factor.
    @return The current bus voltage measurement in V
*/
/**************************************************************************/
float Adafruit_INA228::readBusVoltage(void) {
  Adafruit_I2CRegister bus_voltage =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_VBUS, 3, MSBFIRST);
  // INA228 uses 195.3125 µV/LSB for bus voltage
  return (float)((uint32_t)bus_voltage.read() >> 4) * 195.3125 / 1e6;
}

/**************************************************************************/
/*!
    @brief Sets the shunt calibration by resistor for INA228.
    @param shunt_res Resistance of the shunt in ohms (floating point)
    @param max_current Maximum expected current in A (floating point)
*/
/**************************************************************************/
void Adafruit_INA228::setShunt(float shunt_res, float max_current) {
  _shunt_res = shunt_res;
  // INA228 uses 2^19 as the divisor
  _current_lsb = max_current / (float)(1UL << 19);
  _updateShuntCalRegister();
}