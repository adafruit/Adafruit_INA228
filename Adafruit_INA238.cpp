/*!
 *  @file Adafruit_INA238.cpp
 *
 *  @section ina238_intro Introduction
 *
 * 	I2C Driver for the INA238 I2C Current and Power sensor
 *
 * 	This is a library for the Adafruit INA238 breakout:
 * 	http://www.adafruit.com/products/xxxx
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section ina238_dependencies Dependencies
 *
 *  This library depends on the Adafruit BusIO library
 *
 *  @section ina238_author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section ina238_license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section ina238_history HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_INA238.h"

#include <Wire.h>

#include "Arduino.h"

/*!
 *    @brief  Instantiates a new INA238 class
 */
Adafruit_INA238::Adafruit_INA238(void) {}

/*!
 *    @brief  Sets up the HW
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  theWire
 *            The Wire object to be used for I2C connections.
 *    @param  skipReset
 *            When set to true, will omit resetting all registers to
 *            their default values. Default: false.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_INA238::begin(uint8_t i2c_address, TwoWire* theWire,
                            bool skipReset) {
  if (!Adafruit_INA2xx::begin(i2c_address, theWire, skipReset)) {
    return false;
  }

  // make sure we're talking to the right chip (must be INA238)
  if (_device_id != INA238_DEVICE_ID) {
    return false;
  }

  return true;
}