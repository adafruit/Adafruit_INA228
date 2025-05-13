/*!
 *  @file Adafruit_INA238.h
 *
 * 	I2C Driver for INA238 Current and Power sensor
 *
 * 	This is a library for the Adafruit INA238 breakout:
 * 	http://www.adafruit.com/products/xxxx
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_INA238_H
#define _ADAFRUIT_INA238_H

#include "Adafruit_INA237.h"

#define INA238_I2CADDR_DEFAULT 0x40 ///< INA238 default i2c address
#define INA238_DEVICE_ID 0x238      ///< INA238 device ID

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            INA238 Current and Power Sensor (subclass of INA237)
 */
class Adafruit_INA238 : public Adafruit_INA237 {
 public:
  Adafruit_INA238();
  bool begin(uint8_t i2c_addr = INA238_I2CADDR_DEFAULT,
             TwoWire* theWire = &Wire, bool skipReset = false);
};

#endif