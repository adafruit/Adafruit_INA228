/*!
 *  @file Adafruit_INA237.cpp
 *
 *  @section ina237_intro Introduction
 *
 * 	I2C Driver for the INA237 I2C Current and Power sensor
 *
 * 	This is a library for the Adafruit INA237 breakout:
 * 	http://www.adafruit.com/products/xxxx
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section ina237_dependencies Dependencies
 *
 *  This library depends on the Adafruit BusIO library
 *
 *  @section ina237_author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section ina237_license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section ina237_history HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_INA237.h"

#include <Wire.h>

#include "Arduino.h"

/*!
 *    @brief  Instantiates a new INA237 class
 */
Adafruit_INA237::Adafruit_INA237(void) {}

/*!
 *    @brief  Sets up the HW
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  theWire
 *            The Wire object to be used for I2C connections.
 *    @param  skipReset
 *            When set to true, will omit resetting all INA237 registers to
 *            their default values. Default: false.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_INA237::begin(uint8_t i2c_address, TwoWire* theWire,
                            bool skipReset) {
  if (!Adafruit_INA2xx::begin(i2c_address, theWire, skipReset)) {
    return false;
  }

  // make sure we're talking to the right chip
  if (_device_id != INA237_DEVICE_ID) {
    return false;
  }

  return true;
}

/**************************************************************************/
/*!
    @brief Updates the shunt calibration value to the INA237 register.
    The INA237 uses a different calculation than the INA228.
*/
/**************************************************************************/
void Adafruit_INA237::_updateShuntCalRegister() {
  // Formula from INA237 datasheet (SBOSA20A)
  // SHUNT_CAL = 819.2 x 10^6 x CURRENT_LSB x RSHUNT

  float scale = 1;
  if (getADCRange()) {
    scale = 4; // For lower range (+/-40.96mV)
  }

  // Correct calculation for INA237
  float shunt_cal = 819.2e6 * _current_lsb * _shunt_res * scale;

  Adafruit_I2CRegister shunt =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_SHUNTCAL, 2, MSBFIRST);
  shunt.write(shunt_cal);
}

/**************************************************************************/
/*!
    @brief Returns the current alert type
    @return The current alert type
*/
/**************************************************************************/
INA237_AlertType Adafruit_INA237::getAlertType(void) {
  // The alert bits are in different positions in the INA237
  Adafruit_I2CRegisterBits alert_type =
      Adafruit_I2CRegisterBits(Diag_Alert, 7, 5);
  return (INA237_AlertType)alert_type.read();
}

/**************************************************************************/
/*!
    @brief Sets a new alert type
    @param alert
          The new alert type to be set
*/
/**************************************************************************/
void Adafruit_INA237::setAlertType(INA237_AlertType alert) {
  // The alert bits are in different positions in the INA237
  Adafruit_I2CRegisterBits alert_type =
      Adafruit_I2CRegisterBits(Diag_Alert, 7, 5);
  alert_type.write(alert);
}

/**************************************************************************/
/*!
    @brief Reads the die temperature with the INA237-specific conversion factor
    @return The current die temp in deg C
*/
/**************************************************************************/
float Adafruit_INA237::readDieTemp(void) {
  Adafruit_I2CRegister temp =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_DIETEMP, 2, MSBFIRST);
  int16_t t = temp.read();
  // INA237 uses 12 bits for temperature (bits 15:4) with 125 m°C/LSB
  // Shift by 4 to get the actual value from register bits 15:4
  return (float)(t >> 4) * 125.0 / 1000.0;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Bus Voltage register
           using INA237-specific conversion factor.
    @return The current bus voltage measurement in V
*/
/**************************************************************************/
float Adafruit_INA237::readBusVoltage(void) {
  Adafruit_I2CRegister bus_voltage =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_VBUS, 2, MSBFIRST);
  // INA237 uses 3.125 mV/LSB for bus voltage
  // Bus voltage is a 16-bit value in the INA237 (unlike INA228 which is 24-bit)
  return (float)((uint16_t)bus_voltage.read()) * 3.125 / 1000.0;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Shunt Voltage register
           using INA237-specific conversion factor.
    @return The current shunt voltage measurement in V
*/
/**************************************************************************/
float Adafruit_INA237::readShuntVoltage(void) {
  float scale = 5.0; // 5 µV/LSB in normal mode
  if (getADCRange()) {
    scale = 1.25; // 1.25 µV/LSB in low range mode
  }

  Adafruit_I2CRegister shunt_voltage =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_VSHUNT, 2, MSBFIRST);
  int16_t v = shunt_voltage.read();
  return (float)v * scale / 1000000.0; // Convert µV to V
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Current register
           using INA237-specific handling.
    @return The current measurement in mA
*/
/**************************************************************************/
float Adafruit_INA237::readCurrent(void) {
  Adafruit_I2CRegister current =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_CURRENT, 2, MSBFIRST);
  int16_t i = current.read();
  return (float)i * _current_lsb * 1000.0; // Convert A to mA
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Power register
           using INA237-specific handling.
    @return The current Power calculation in mW
*/
/**************************************************************************/
float Adafruit_INA237::readPower(void) {
  Adafruit_I2CRegister power =
      Adafruit_I2CRegister(i2c_dev, INA2XX_REG_POWER, 2, MSBFIRST);
  // INA237 power LSB = 20 * current_lsb
  return (float)power.read() * 20.0 * _current_lsb * 1000.0; // Convert W to mW
}

/**************************************************************************/
/*!
    @brief Sets the shunt calibration by resistor for INA237.
    @param shunt_res Resistance of the shunt in ohms (floating point)
    @param max_current Maximum expected current in A (floating point)
*/
/**************************************************************************/
void Adafruit_INA237::setShunt(float shunt_res, float max_current) {
  _shunt_res = shunt_res;
  // INA237 uses 2^15 as the divisor
  _current_lsb = max_current / (float)(1UL << 15);
  _updateShuntCalRegister();
}
