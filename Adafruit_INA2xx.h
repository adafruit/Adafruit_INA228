/*!
 *  @file Adafruit_INA2xx.h
 *
 * 	I2C Driver base class for INA2xx Current and Power sensors
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

#ifndef _ADAFRUIT_INA2XX_H
#define _ADAFRUIT_INA2XX_H

#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Wire.h>

#include "Arduino.h"

// Common registers for INA2xx family
#define INA2XX_REG_CONFIG 0x00    ///< Configuration register
#define INA2XX_REG_ADCCFG 0x01    ///< ADC configuration register
#define INA2XX_REG_SHUNTCAL 0x02  ///< Shunt calibration register
#define INA2XX_REG_VSHUNT 0x04    ///< Shunt voltage measurement register
#define INA2XX_REG_VBUS 0x05      ///< Bus voltage measurement register
#define INA2XX_REG_DIETEMP 0x06   ///< Temperature measurement register
#define INA2XX_REG_CURRENT 0x07   ///< Current result register
#define INA2XX_REG_POWER 0x08     ///< Power result register
#define INA2XX_REG_DIAGALRT 0x0B  ///< Diagnostic flags and alert register
#define INA2XX_REG_SOVL 0x0C      ///< Shunt overvoltage threshold register
#define INA2XX_REG_SUVL 0x0D      ///< Shunt undervoltage threshold register
#define INA2XX_REG_BOVL 0x0E      ///< Bus overvoltage threshold register
#define INA2XX_REG_BUVL 0x0F      ///< Bus undervoltage threshold register
#define INA2XX_REG_TEMPLIMIT 0x10 ///< Temperature over-limit threshold register
#define INA2XX_REG_PWRLIMIT 0x11  ///< Power over-limit threshold register
#define INA2XX_REG_MFG_UID 0x3E   ///< Manufacturer ID register
#define INA2XX_REG_DVC_UID 0x3F   ///< Device ID and revision register

#define INA2XX_I2CADDR_DEFAULT 0x40 ///< INA2xx default i2c address

/**
 * @brief Mode options.
 *
 * Allowed values for setMode.
 */
typedef enum _mode {
  /**< SHUTDOWN: Minimize quiescient current and turn off current into the
   device inputs. Set another mode to exit shutown mode **/
  INA2XX_MODE_SHUTDOWN = 0x00,

  /**< Triggered bus voltage, single shot **/
  INA2XX_MODE_TRIG_BUS = 0x01,
  /**< Triggered shunt voltage, single shot **/
  INA2XX_MODE_TRIG_SHUNT = 0x02,
  /**< Triggered shunt voltage and bus voltage, single shot **/
  INA2XX_MODE_TRIG_BUS_SHUNT = 0x03,
  /**< Triggered temperature, single shot **/
  INA2XX_MODE_TRIG_TEMP = 0x04,
  /**< Triggered temperature and bus voltage, single shot **/
  INA2XX_MODE_TRIG_TEMP_BUS = 0x05,
  /**< Triggered temperature and shunt voltage, single shot **/
  INA2XX_MODE_TRIG_TEMP_SHUNT = 0x06,
  /**< Triggered bus voltage, shunt voltage and temperature, single shot **/
  INA2XX_MODE_TRIG_TEMP_BUS_SHUNT = 0x07,

  /**< Shutdown **/
  INA2XX_MODE_SHUTDOWN2 = 0x08,
  /**< Continuous bus voltage only **/
  INA2XX_MODE_CONT_BUS = 0x09,
  /**< Continuous shunt voltage only **/
  INA2XX_MODE_CONT_SHUNT = 0x0A,
  /**< Continuous shunt and bus voltage **/
  INA2XX_MODE_CONT_BUS_SHUNT = 0x0B,
  /**< Continuous temperature only **/
  INA2XX_MODE_CONT_TEMP = 0x0C,
  /**< Continuous bus voltage and temperature **/
  INA2XX_MODE_CONT_TEMP_BUS = 0x0D,
  /**< Continuous temperature and shunt voltage **/
  INA2XX_MODE_CONT_TEMP_SHUNT = 0x0E,
  /**< Continuous bus voltage, shunt voltage and temperature **/
  INA2XX_MODE_CONT_TEMP_BUS_SHUNT = 0x0F,

  /**< TRIGGERED: Trigger a one-shot measurement of temp, current and bus
   voltage. Set the TRIGGERED mode again to take a new measurement **/
  INA2XX_MODE_TRIGGERED = INA2XX_MODE_TRIG_TEMP_BUS_SHUNT,
  /**< CONTINUOUS: (Default) Continuously update the temp, current, bus
   voltage and power registers with new measurements **/
  INA2XX_MODE_CONTINUOUS = INA2XX_MODE_CONT_TEMP_BUS_SHUNT
} INA2XX_MeasurementMode;

/**
 * @brief Conversion Time options.
 *
 * Allowed values for setCurrentConversionTime and setVoltageConversionTime.
 */
typedef enum _conversion_time {
  INA2XX_TIME_50_us,   ///< Measurement time: 50us
  INA2XX_TIME_84_us,   ///< Measurement time: 84us
  INA2XX_TIME_150_us,  ///< Measurement time: 150us
  INA2XX_TIME_280_us,  ///< Measurement time: 280us
  INA2XX_TIME_540_us,  ///< Measurement time: 540us
  INA2XX_TIME_1052_us, ///< Measurement time: 1052us
  INA2XX_TIME_2074_us, ///< Measurement time: 2074us
  INA2XX_TIME_4120_us, ///< Measurement time: 4120us
} INA2XX_ConversionTime;

/**
 * @brief Averaging Count options.
 *
 * Allowed values forsetAveragingCount.
 */
typedef enum _count {
  INA2XX_COUNT_1,    ///< Window size: 1 sample (Default)
  INA2XX_COUNT_4,    ///< Window size: 4 samples
  INA2XX_COUNT_16,   ///< Window size: 16 samples
  INA2XX_COUNT_64,   ///< Window size: 64 samples
  INA2XX_COUNT_128,  ///< Window size: 128 samples
  INA2XX_COUNT_256,  ///< Window size: 256 samples
  INA2XX_COUNT_512,  ///< Window size: 512 samples
  INA2XX_COUNT_1024, ///< Window size: 1024 samples
} INA2XX_AveragingCount;

/**
 * @brief Alert pin polarity options.
 *
 * Allowed values for setAlertPolarity.
 */
typedef enum _alert_polarity {
  INA2XX_ALERT_POLARITY_NORMAL = 0x0, ///< Active high open-collector (Default)
  INA2XX_ALERT_POLARITY_INVERTED = 0x1, ///< Active low open-collector
} INA2XX_AlertPolarity;

/**
 * @brief Alert pin latch options.
 *
 * Allowed values for setAlertLatch.
 */
typedef enum _alert_latch {
  INA2XX_ALERT_LATCH_ENABLED = 0x1,     /**< Alert will latch until Mask/Enable
                                          register is read **/
  INA2XX_ALERT_LATCH_TRANSPARENT = 0x0, /**< Alert will reset when fault is
                                          cleared **/
} INA2XX_AlertLatch;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            INA2xx Current and Power Sensor
 */
class Adafruit_INA2xx {
 public:
  Adafruit_INA2xx();
  virtual bool begin(uint8_t i2c_addr = INA2XX_I2CADDR_DEFAULT,
                     TwoWire* theWire = &Wire, bool skipReset = false);
  virtual void reset(void);

  virtual void setShunt(float shunt_res = 0.1, float max_current = 3.2);
  void setADCRange(uint8_t);
  uint8_t getADCRange(void);
  virtual float readDieTemp(void);
  virtual float readBusVoltage(void);

  // Common INA interface:
  float getBusVoltage_V(void);
  float getShuntVoltage_mV(void);
  float getCurrent_mA(void);
  float getPower_mW(void);
  //

  virtual float readCurrent(void);
  virtual float readShuntVoltage(void);
  virtual float readPower(void);

  void setMode(INA2XX_MeasurementMode mode);
  INA2XX_MeasurementMode getMode(void);

  bool conversionReady(void);
  uint16_t alertFunctionFlags(void);

  INA2XX_AlertLatch getAlertLatch(void);
  void setAlertLatch(INA2XX_AlertLatch state);
  INA2XX_AlertPolarity getAlertPolarity(void);
  void setAlertPolarity(INA2XX_AlertPolarity polarity);

  INA2XX_ConversionTime getCurrentConversionTime(void);
  void setCurrentConversionTime(INA2XX_ConversionTime time);
  INA2XX_ConversionTime getVoltageConversionTime(void);
  void setVoltageConversionTime(INA2XX_ConversionTime time);
  INA2XX_ConversionTime getTemperatureConversionTime(void);
  void setTemperatureConversionTime(INA2XX_ConversionTime time);
  INA2XX_AveragingCount getAveragingCount(void);
  void setAveragingCount(INA2XX_AveragingCount count);

  Adafruit_I2CRegister *Config, ///< BusIO Register for Config
      *ADC_Config,              ///< BusIO Register for ADC Config
      *Diag_Alert;              ///< BusIO Register for Diagnostic Alerts

 protected:
  virtual void _updateShuntCalRegister(
      void);          ///< Updates the shunt calibration register based on
                      ///< device-specific calculations
  float _shunt_res;   ///< Shunt resistance value in ohms
  float _current_lsb; ///< Current LSB value used for calculations
  Adafruit_I2CDevice* i2c_dev; ///< I2C device interface
  uint16_t _device_id;         ///< Device ID for chip verification
};

#endif