/*!
 *  @file Adafruit_INA228.h
 *
 * 	I2C Driver for INA228 Current and Power sensor
 *
 * 	This is a library for the Adafruit INA228 breakout:
 * 	http://www.adafruit.com/products/5832
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_INA228_H
#define _ADAFRUIT_INA228_H

#include "Adafruit_INA2xx.h"

#define INA228_I2CADDR_DEFAULT 0x40 ///< INA228 default i2c address
#define INA228_DEVICE_ID 0x228      ///< INA228 device ID

// INA228 specific registers
#define INA228_REG_ENERGY 0x09      ///< Energy result register
#define INA228_REG_CHARGE 0x0A      ///< Charge result register (40-bit)
#define INA228_REG_SHUNTTEMPCO 0x03 ///< Shunt temperature coefficient register

///@{
/**
 * @name Legacy compatibility macros
 *
 * These macros provide backward compatibility with earlier versions of the
 * library
 */
#define INA228_MODE_SHUTDOWN \
  INA2XX_MODE_SHUTDOWN ///< Alias for INA2XX_MODE_SHUTDOWN
#define INA228_MODE_TRIG_BUS \
  INA2XX_MODE_TRIG_BUS ///< Alias for INA2XX_MODE_TRIG_BUS
#define INA228_MODE_TRIG_SHUNT \
  INA2XX_MODE_TRIG_SHUNT ///< Alias for INA2XX_MODE_TRIG_SHUNT
#define INA228_MODE_TRIG_BUS_SHUNT \
  INA2XX_MODE_TRIG_BUS_SHUNT ///< Alias for INA2XX_MODE_TRIG_BUS_SHUNT
#define INA228_MODE_TRIG_TEMP \
  INA2XX_MODE_TRIG_TEMP ///< Alias for INA2XX_MODE_TRIG_TEMP
#define INA228_MODE_TRIG_TEMP_BUS \
  INA2XX_MODE_TRIG_TEMP_BUS ///< Alias for INA2XX_MODE_TRIG_TEMP_BUS
#define INA228_MODE_TRIG_TEMP_SHUNT \
  INA2XX_MODE_TRIG_TEMP_SHUNT ///< Alias for INA2XX_MODE_TRIG_TEMP_SHUNT
#define INA228_MODE_TRIG_TEMP_BUS_SHUNT \
  INA2XX_MODE_TRIG_TEMP_BUS_SHUNT ///< Alias for INA2XX_MODE_TRIG_TEMP_BUS_SHUNT
#define INA228_MODE_SHUTDOWN2 \
  INA2XX_MODE_SHUTDOWN2 ///< Alias for INA2XX_MODE_SHUTDOWN2
#define INA228_MODE_CONT_BUS \
  INA2XX_MODE_CONT_BUS ///< Alias for INA2XX_MODE_CONT_BUS
#define INA228_MODE_CONT_SHUNT \
  INA2XX_MODE_CONT_SHUNT ///< Alias for INA2XX_MODE_CONT_SHUNT
#define INA228_MODE_CONT_BUS_SHUNT \
  INA2XX_MODE_CONT_BUS_SHUNT ///< Alias for INA2XX_MODE_CONT_BUS_SHUNT
#define INA228_MODE_CONT_TEMP \
  INA2XX_MODE_CONT_TEMP ///< Alias for INA2XX_MODE_CONT_TEMP
#define INA228_MODE_CONT_TEMP_BUS \
  INA2XX_MODE_CONT_TEMP_BUS ///< Alias for INA2XX_MODE_CONT_TEMP_BUS
#define INA228_MODE_CONT_TEMP_SHUNT \
  INA2XX_MODE_CONT_TEMP_SHUNT ///< Alias for INA2XX_MODE_CONT_TEMP_SHUNT
#define INA228_MODE_CONT_TEMP_BUS_SHUNT \
  INA2XX_MODE_CONT_TEMP_BUS_SHUNT ///< Alias for INA2XX_MODE_CONT_TEMP_BUS_SHUNT
#define INA228_MODE_TRIGGERED \
  INA2XX_MODE_TRIGGERED ///< Alias for INA2XX_MODE_TRIGGERED
#define INA228_MODE_CONTINUOUS \
  INA2XX_MODE_CONTINUOUS ///< Alias for INA2XX_MODE_CONTINUOUS

#define INA228_TIME_50_us INA2XX_TIME_50_us   ///< Alias for INA2XX_TIME_50_us
#define INA228_TIME_84_us INA2XX_TIME_84_us   ///< Alias for INA2XX_TIME_84_us
#define INA228_TIME_150_us INA2XX_TIME_150_us ///< Alias for INA2XX_TIME_150_us
#define INA228_TIME_280_us INA2XX_TIME_280_us ///< Alias for INA2XX_TIME_280_us
#define INA228_TIME_540_us INA2XX_TIME_540_us ///< Alias for INA2XX_TIME_540_us
#define INA228_TIME_1052_us \
  INA2XX_TIME_1052_us ///< Alias for INA2XX_TIME_1052_us
#define INA228_TIME_2074_us \
  INA2XX_TIME_2074_us ///< Alias for INA2XX_TIME_2074_us
#define INA228_TIME_4120_us \
  INA2XX_TIME_4120_us ///< Alias for INA2XX_TIME_4120_us

#define INA228_COUNT_1 INA2XX_COUNT_1       ///< Alias for INA2XX_COUNT_1
#define INA228_COUNT_4 INA2XX_COUNT_4       ///< Alias for INA2XX_COUNT_4
#define INA228_COUNT_16 INA2XX_COUNT_16     ///< Alias for INA2XX_COUNT_16
#define INA228_COUNT_64 INA2XX_COUNT_64     ///< Alias for INA2XX_COUNT_64
#define INA228_COUNT_128 INA2XX_COUNT_128   ///< Alias for INA2XX_COUNT_128
#define INA228_COUNT_256 INA2XX_COUNT_256   ///< Alias for INA2XX_COUNT_256
#define INA228_COUNT_512 INA2XX_COUNT_512   ///< Alias for INA2XX_COUNT_512
#define INA228_COUNT_1024 INA2XX_COUNT_1024 ///< Alias for INA2XX_COUNT_1024

#define INA228_ALERT_POLARITY_NORMAL \
  INA2XX_ALERT_POLARITY_NORMAL ///< Alias for INA2XX_ALERT_POLARITY_NORMAL
#define INA228_ALERT_POLARITY_INVERTED \
  INA2XX_ALERT_POLARITY_INVERTED ///< Alias for INA2XX_ALERT_POLARITY_INVERTED

#define INA228_ALERT_LATCH_ENABLED \
  INA2XX_ALERT_LATCH_ENABLED ///< Alias for INA2XX_ALERT_LATCH_ENABLED
#define INA228_ALERT_LATCH_TRANSPARENT \
  INA2XX_ALERT_LATCH_TRANSPARENT ///< Alias for INA2XX_ALERT_LATCH_TRANSPARENT
///@}

///@{
/**
 * @name Legacy type aliases
 *
 * These typedefs provide backward compatibility with earlier versions of the
 * library
 */
typedef INA2XX_MeasurementMode
    INA228_MeasurementMode; ///< Alias for INA2XX_MeasurementMode
typedef INA2XX_ConversionTime
    INA228_ConversionTime; ///< Alias for INA2XX_ConversionTime
typedef INA2XX_AveragingCount
    INA228_AveragingCount; ///< Alias for INA2XX_AveragingCount
typedef INA2XX_AlertPolarity
    INA228_AlertPolarity;                    ///< Alias for INA2XX_AlertPolarity
typedef INA2XX_AlertLatch INA228_AlertLatch; ///< Alias for INA2XX_AlertLatch
///@}

/**
 * @brief Alert trigger options specific to INA228.
 *
 * Allowed values for setAlertType.
 */
typedef enum _alert_type {
  INA228_ALERT_CONVERSION_READY = 0x1, ///< Trigger on conversion ready
  INA228_ALERT_OVERPOWER = 0x2,        ///< Trigger on power over limit
  INA228_ALERT_UNDERVOLTAGE = 0x4,     ///< Trigger on bus voltage under limit
  INA228_ALERT_OVERVOLTAGE = 0x8,      ///< Trigger on bus voltage over limit
  INA228_ALERT_UNDERCURRENT = 0x10,    ///< Trigger on current under limit
  INA228_ALERT_OVERCURRENT = 0x20,     ///< Trigger on current over limit
  INA228_ALERT_NONE = 0x0,             ///< Do not trigger alert pin (Default)
} INA228_AlertType;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            INA228 Current and Power Sensor
 */
class Adafruit_INA228 : public Adafruit_INA2xx {
 public:
  Adafruit_INA228();
  bool begin(uint8_t i2c_addr = INA228_I2CADDR_DEFAULT,
             TwoWire* theWire = &Wire, bool skipReset = false);
  void reset(void) override;

  // INA228 specific functions
  float readEnergy(void);
  float readCharge(void);
  INA228_AlertType getAlertType(void);
  void setAlertType(INA228_AlertType alert);
  void resetAccumulators(void);
  float readDieTemp(void) override;
  float readBusVoltage(void) override;
  void setShunt(float shunt_res = 0.1, float max_current = 3.2) override;

  // INA228 specific register pointer
  Adafruit_I2CRegister* AlertLimit; ///< BusIO Register for AlertLimit

 protected:
  void _updateShuntCalRegister(void) override;
};

#endif