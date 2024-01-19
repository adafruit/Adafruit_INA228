/*!
 *  @file Adafruit_INA228.h
 *
 * 	I2C Driver for INA228 Current and Power sensor
 *
 * 	This is a library for the Adafruit INA228 breakout:
 * 	http://www.adafruit.com/products/xxxx
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

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Wire.h>

#define INA228_I2CADDR_DEFAULT 0x40 ///< INA228 default i2c address
//! @brief Configuration Register (CONFIG), 16 bits. Includes bits for reset,
//! shunt calibration reset, conversion delay, temperature compensation, and ADC
//! range.
#define INA228_REG_CONFIG 0x00 ///< Configuration Register
//! @brief ADC Configuration Register (ADC_CONFIG), 16 bits. Includes bits for
//! ADC mode, averaging sample count, and temperature / shunt/bus voltage
//! conversion time.
#define INA228_REG_ADCCFG 0x01 ///< ADC Configuration Register
//! @brief Shunt Calibration Register (SHUNT_CAL), 16 bits. Provides the device
//! with a conversion constant value that represents shunt resistance for
//! calculating current.
#define INA228_REG_SHUNTCAL 0x02
//! @brief Shunt Temperature Coefficient Register (SHUNT_TEMPCO), 16 bits.
//! Stores the temperature coefficient of the shunt for temperature compensation
//! correction.
#define INA228_REG_SHUNTTEMPCO 0x03
//! @brief Shunt Voltage Measurement Register (VSHUNT), 24 bits. Measures
//! differential voltage across the shunt output.
#define INA228_REG_VSHUNT 0x04
//! @brief Bus Voltage Measurement Register (VBUS), 24 bits. Measures bus
//! voltage output.
#define INA228_REG_VBUS 0x05
//! @brief Temperature Measurement Register (DIETEMP), 16 bits. Measures
//! internal die temperature.
#define INA228_REG_DIETEMP 0x06
//! @brief Current Result Register (CURRENT), 24 bits. Outputs calculated
//! current in Amperes.
#define INA228_REG_CURRENT 0x07
//! @brief Power Result Register (POWER), 24 bits. Outputs calculated power in
//! watts.
#define INA228_REG_POWER 0x08
//! @brief Energy Result Register (ENERGY), 40 bits. Outputs calculated energy
//! in Joules.
#define INA228_REG_ENERGY 0x09
//! @brief Charge Result Register (CHARGE), 40 bits. Outputs calculated charge
//! in Coulombs.
#define INA228_REG_CHARGE 0x0A
//! @brief Diagnostic Flags and Alert Register (DIAG_ALRT), 16 bits. Includes
//! various diagnostic flags and alert settings.
#define INA228_REG_DIAGALRT 0x0B
//! @brief Shunt Overvoltage Threshold Register (SOVL), 16 bits. Sets threshold
//! for shunt overvoltage detection.
#define INA228_REG_SOVL 0x0C
//! @brief Shunt Undervoltage Threshold Register (SUVL), 16 bits. Sets threshold
//! for shunt undervoltage detection.
#define INA228_REG_SUVL 0x0D
//! @brief Bus Overvoltage Threshold Register (BOVL), 16 bits. Sets threshold
//! for bus overvoltage detection.
#define INA228_REG_BOVL 0x0E
//! @brief Bus Undervoltage Threshold Register (BUVL), 16 bits. Sets threshold
//! for bus undervoltage detection.
#define INA228_REG_BUVL 0x0F
//! @brief Temperature Over-Limit Threshold Register (TEMP_LIMIT), 16 bits. Sets
//! threshold for over temperature detection.
#define INA228_REG_TEMPLIMIT 0x10
//! @brief Power Over-Limit Threshold Register (PWR_LIMIT), 16 bits. Sets
//! threshold for power over-limit detection.
#define INA228_REG_PWRLIMIT 0x10
#define INA228_REG_MFG_UID 0x3E ///< Manufacturer ID Register
#define INA228_REG_DVC_UID 0x3F ///< Device ID and Revision Register

/**
 * @brief Mode options.
 *
 * Allowed values for setMode.
 */
typedef enum _mode {
  INA228_MODE_SHUTDOWN = 0x00, /**< SHUTDOWN: Minimize quiescient current and
                                turn off current into the device inputs. Set
                                another mode to exit shutown mode **/
  INA228_MODE_TRIGGERED =
      0x07,                      /**< TRIGGERED: Trigger a one-shot measurement
                                   of temp, current and bus voltage. Set the TRIGGERED
                                   mode again to take a new measurement **/
  INA228_MODE_CONTINUOUS = 0x0F, /**< CONTINUOUS: (Default) Continuously update
                                    the temp, current, bus voltage and power
                                    registers with new measurements **/
} INA228_MeasurementMode;

/**
 * @brief Conversion Time options.
 *
 * Allowed values for setCurrentConversionTime and setVoltageConversionTime.
 */
typedef enum _conversion_time {
  INA228_TIME_50_us,   ///< Measurement time: 50us
  INA228_TIME_84_us,   ///< Measurement time: 84us
  INA228_TIME_150_us,  ///< Measurement time: 150us
  INA228_TIME_280_us,  ///< Measurement time: 280us
  INA228_TIME_540_us,  ///< Measurement time: 540us
  INA228_TIME_1052_us, ///< Measurement time: 1052us
  INA228_TIME_2074_us, ///< Measurement time: 2074us
  INA228_TIME_4120_us, ///< Measurement time: 4120us
} INA228_ConversionTime;

/**
 * @brief Averaging Count options.
 *
 * Allowed values forsetAveragingCount.
 */
typedef enum _count {
  INA228_COUNT_1,    ///< Window size: 1 sample (Default)
  INA228_COUNT_4,    ///< Window size: 4 samples
  INA228_COUNT_16,   ///< Window size: 16 samples
  INA228_COUNT_64,   ///< Window size: 64 samples
  INA228_COUNT_128,  ///< Window size: 128 samples
  INA228_COUNT_256,  ///< Window size: 256 samples
  INA228_COUNT_512,  ///< Window size: 512 samples
  INA228_COUNT_1024, ///< Window size: 1024 samples
} INA228_AveragingCount;

/**
 * @brief Alert trigger options.
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

/**
 * @brief Alert pin polarity options.
 *
 * Allowed values for setAlertPolarity.
 */
typedef enum _alert_polarity {
  INA228_ALERT_POLARITY_NORMAL = 0x0, ///< Active high open-collector (Default)
  INA228_ALERT_POLARITY_INVERTED = 0x1, ///< Active low open-collector
} INA228_AlertPolarity;

/**
 * @brief Alert pin latch options.
 *
 * Allowed values for setAlertLatch.
 */
typedef enum _alert_latch {
  INA228_ALERT_LATCH_ENABLED = 0x1,     /**< Alert will latch until Mask/Enable
                                           register is read **/
  INA228_ALERT_LATCH_TRANSPARENT = 0x0, /**< Alert will reset when fault is
                                           cleared **/
} INA228_AlertLatch;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            INA228 Current and Power Sensor
 */
class Adafruit_INA228 {
public:
  Adafruit_INA228();
  bool begin(uint8_t i2c_addr = INA228_I2CADDR_DEFAULT,
             TwoWire *theWire = &Wire, reset = true);
  void reset(void);
  void resetAccumulated(void);

  void setShunt(float shunt_res = 0.015, float max_current = 10);
  float readDieTemp(void);

  float readCurrent(void);
  float readBusVoltage(void);
  float readShuntVoltage(void);
  float readPower(void);
  float readEnergy(void);

  void setMode(INA228_MeasurementMode mode);
  INA228_MeasurementMode getMode(void);

  bool conversionReady(void);
  uint16_t alertFunctionFlags(void);

  float getAlertLimit(void);
  void setAlertLimit(float limit);
  INA228_AlertLatch getAlertLatch(void);
  void setAlertLatch(INA228_AlertLatch state);
  INA228_AlertPolarity getAlertPolarity(void);
  void setAlertPolarity(INA228_AlertPolarity polarity);
  INA228_AlertType getAlertType(void);
  void setAlertType(INA228_AlertType alert);

  INA228_ConversionTime getCurrentConversionTime(void);
  void setCurrentConversionTime(INA228_ConversionTime time);
  INA228_ConversionTime getVoltageConversionTime(void);
  void setVoltageConversionTime(INA228_ConversionTime time);
  INA228_AveragingCount getAveragingCount(void);
  void setAveragingCount(INA228_AveragingCount count);

  Adafruit_I2CRegister *Config, ///< BusIO Register for Config
      *ADC_Config,              ///< BusIO Register for Config
      *Diag_Alert,              ///< BusIO Register for MaskEnable
      *AlertLimit;              ///< BusIO Register for AlertLimit

private:
  float _current_lsb;
  Adafruit_I2CDevice *i2c_dev;
};

#endif
