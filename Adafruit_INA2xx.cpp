/*!
 *  @file Adafruit_INA228.cpp
 *
 *  @mainpage Adafruit INA228 I2C Current and Power sensor
 *
 *  @section intro_sec Introduction
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
 *  @section dependencies Dependencies
 *
 *  This library depends on the Adafruit BusIO library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_INA228.h"

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
bool Adafruit_INA228::begin(uint8_t i2c_address, TwoWire *theWire,
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