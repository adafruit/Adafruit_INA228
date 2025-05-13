/*!
 *  @file Adafruit_INA237.h
 *
 * 	I2C Driver for INA237/INA238 Current and Power sensors
 *
 * 	This is a library for the Adafruit INA237/INA238 breakouts:
 * 	http://www.adafruit.com/products/xxxx
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_INA237_H
#define _ADAFRUIT_INA237_H

#include "Adafruit_INA2xx.h"

#define INA237_I2CADDR_DEFAULT 0x40 ///< INA237/INA238 default i2c address
#define INA237_DEVICE_ID 0x237      ///< INA237 device ID
#define INA238_DEVICE_ID 0x238      ///< INA238 device ID

/**
 * @brief Alert trigger options specific to INA237/INA238.
 *
 * Allowed values for setAlertType.
 */
typedef enum _ina237_alert_type {
  INA237_ALERT_CONVERSION_READY = 0x1, ///< Trigger on conversion ready
  INA237_ALERT_OVERTEMPERATURE = 0x2,  ///< Trigger on overtemperature
  INA237_ALERT_OVERPOWER = 0x4,        ///< Trigger on power over limit
  INA237_ALERT_UNDERVOLTAGE = 0x8,     ///< Trigger on bus voltage under limit
  INA237_ALERT_OVERVOLTAGE = 0x10,     ///< Trigger on bus voltage over limit
  INA237_ALERT_UNDERSHUNT = 0x20,      ///< Trigger on shunt voltage under limit
  INA237_ALERT_OVERSHUNT = 0x40,       ///< Trigger on shunt voltage over limit
  INA237_ALERT_NONE = 0x0,             ///< Do not trigger alert pin (Default)
} INA237_AlertType;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            INA237/INA238 Current and Power Sensors
 */
class Adafruit_INA237 : public Adafruit_INA2xx {
public:
  Adafruit_INA237();
  bool begin(uint8_t i2c_addr = INA237_I2CADDR_DEFAULT,
             TwoWire *theWire = &Wire, bool skipReset = false);
             
  // INA237/INA238 specific functions
  INA237_AlertType getAlertType(void);
  void setAlertType(INA237_AlertType alert);

protected:
  void _updateShuntCalRegister(void) override;
};

#endif