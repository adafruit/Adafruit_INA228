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
 * 	http://www.adafruit.com/products/4226
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
 *    @param  reset
 *            Whether or not to reset the registers on initialization.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_INA228::begin(uint8_t i2c_address, TwoWire *theWire, bool reset) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, theWire);

  if (!i2c_dev->begin()) {
    return false;
  }

  Adafruit_I2CRegister *device_register =
      new Adafruit_I2CRegister(i2c_dev, INA228_REG_DVC_UID, 2, MSBFIRST);
  Adafruit_I2CRegister *mfg_register =
      new Adafruit_I2CRegister(i2c_dev, INA228_REG_MFG_UID, 2, MSBFIRST);
  Adafruit_I2CRegisterBits *device_id =
      new Adafruit_I2CRegisterBits(device_register, 12, 4);

  // make sure we're talking to the right chip
  if ((mfg_register->read() != 0x5449) || (device_id->read() != 0x228)) {
    return false;
  }

  Config = new Adafruit_I2CRegister(i2c_dev, INA228_REG_CONFIG, 2, MSBFIRST);
  ADC_Config =
      new Adafruit_I2CRegister(i2c_dev, INA228_REG_ADCCFG, 2, MSBFIRST);
  Diag_Alert =
      new Adafruit_I2CRegister(i2c_dev, INA228_REG_DIAGALRT, 2, MSBFIRST);

  if (reset)
    reset();
  delay(2); // delay 2ms to give time for first measurement to finish
  return true;
}
/**************************************************************************/
/*!
    @brief Resets the harware. All registers are set to default values,
    the same as a power-on reset.
*/
/**************************************************************************/
void Adafruit_INA228::reset(void) {
  Adafruit_I2CRegisterBits reset = Adafruit_I2CRegisterBits(Config, 1, 15);
  reset.write(1);
  Adafruit_I2CRegisterBits alert_conv =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 14);
  alert_conv.write(1);
  setMode(INA228_MODE_CONTINUOUS);
}

/**************************************************************************/
/*!
    @brief Resets the accumulation registers for energy and charge.
*/
/**************************************************************************/
void Adafruit_INA228::resetAccumulated(void) {
  Adafruit_I2CRegisterBits resetAcc = Adafruit_I2CRegisterBits(Config, 1, 14);
  resetAcc.write(1);
}

/**************************************************************************/
/*!
    @brief Sets the shunt calibration by resistor
    @param shunt_res Resistance of the shunt in ohms (floating point)
*/
/**************************************************************************/
void Adafruit_INA228::setShunt(float shunt_res, float max_current) {
  _current_lsb = max_current / (float)(1UL << 19);
  // Serial.print("current lsb is (uA) ");
  // Serial.println(_current_lsb * 1000000, 8);

  bool adcrange = (Config->read() >> 4) & 1;
  float scale = 1;
  if (adcrange) {
    scale = 4;
  }

  float shunt_cal = 13107.2 * 1000000.0 * shunt_res * _current_lsb * scale;
  // Serial.print("shunt cal is ");
  // Serial.println(shunt_cal);

  Adafruit_I2CRegister shunt =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_SHUNTCAL, 2, MSBFIRST);
  shunt.write(shunt_cal);
}

/**************************************************************************/
/*!
    @brief Reads the die temperature
    @return The current die temp in deg C
*/
/**************************************************************************/
float Adafruit_INA228::readDieTemp(void) {
  Adafruit_I2CRegister temp =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_DIETEMP, 2, MSBFIRST);
  return (float)temp.read() * 7.8125 / 1000.0;
}
/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Current register.
    @return The current current measurement in mA
*/
/**************************************************************************/
float Adafruit_INA228::getCurrent_mA(void) {
  Adafruit_I2CRegister current =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_CURRENT, 3, MSBFIRST);
  int32_t i = current.read();
  if (i & 0x800000)
    i |= 0xFF000000;
  return (float)i / 16.0 * _current_lsb * 1000.0;
}
/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Bus Voltage register.
    @return The current bus voltage measurement in mV
*/
/**************************************************************************/
float Adafruit_INA228::getBusVoltage_V(void) {

  Adafruit_I2CRegister bus_voltage =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_VBUS, 3, MSBFIRST);
  return (float)((uint32_t)bus_voltage.read() >> 4) * 195.3125 / 1000.0;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Shunt Voltage register.
    @return The current shunt voltage measurement in mV
*/
/**************************************************************************/
float Adafruit_INA228::getShuntVoltage_mV(void) {
  bool adcrange = (Config->read() >> 4) & 1;
  float scale = 312.5;
  if (adcrange) {
    scale = 78.125;
  }

  Adafruit_I2CRegister shunt_voltage =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_VSHUNT, 3, MSBFIRST);
  int32_t v = shunt_voltage.read();
  if (v & 0x800000)
    v |= 0xFF000000;
  return v / 16.0 * scale / 1000000.0;
}

/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Power register.
    @return The current Power calculation in mW
*/
/**************************************************************************/
float Adafruit_INA228::readPower(void) {
  Adafruit_I2CRegister power =
      Adafruit_I2CRegister(i2c_dev, INA228_REG_POWER, 3, MSBFIRST);
  return (float)power.read() * 3.2 * _current_lsb * 1000;
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
    @brief Returns the current measurement mode
    @return The current mode
*/
/**************************************************************************/
INA228_MeasurementMode Adafruit_INA228::getMode(void) {
  Adafruit_I2CRegisterBits mode = Adafruit_I2CRegisterBits(ADC_Config, 4, 12);
  return (INA228_MeasurementMode)mode.read();
}
/**************************************************************************/
/*!
    @brief Sets a new measurement mode
    @param new_mode
           The new mode to be set
*/
/**************************************************************************/
void Adafruit_INA228::setMode(INA228_MeasurementMode new_mode) {
  Adafruit_I2CRegisterBits mode = Adafruit_I2CRegisterBits(ADC_Config, 4, 12);
  mode.write(new_mode);
}
/**************************************************************************/
/*!
    @brief Reads the current number of averaging samples
    @return The current number of averaging samples
*/
/**************************************************************************/
INA228_AveragingCount Adafruit_INA228::getAveragingCount(void) {
  Adafruit_I2CRegisterBits averaging_count =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 0);
  return (INA228_AveragingCount)averaging_count.read();
}
/**************************************************************************/
/*!
    @brief Sets the number of averaging samples
    @param count
           The number of samples to be averaged
*/
/**************************************************************************/
void Adafruit_INA228::setAveragingCount(INA228_AveragingCount count) {
  Adafruit_I2CRegisterBits averaging_count =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 0);
  averaging_count.write(count);
}
/**************************************************************************/
/*!
    @brief Reads the current current conversion time
    @return The current current conversion time
*/
/**************************************************************************/
INA228_ConversionTime Adafruit_INA228::getCurrentConversionTime(void) {
  Adafruit_I2CRegisterBits current_conversion_time =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 6);
  return (INA228_ConversionTime)current_conversion_time.read();
}
/**************************************************************************/
/*!
    @brief Sets the current conversion time
    @param time
           The new current conversion time
*/
/**************************************************************************/
void Adafruit_INA228::setCurrentConversionTime(INA228_ConversionTime time) {
  Adafruit_I2CRegisterBits current_conversion_time =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 6);
  current_conversion_time.write(time);
}
/**************************************************************************/
/*!
    @brief Reads the current bus voltage conversion time
    @return The current bus voltage conversion time
*/
/**************************************************************************/
INA228_ConversionTime Adafruit_INA228::getVoltageConversionTime(void) {
  Adafruit_I2CRegisterBits voltage_conversion_time =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 9);
  return (INA228_ConversionTime)voltage_conversion_time.read();
}
/**************************************************************************/
/*!
    @brief Sets the bus voltage conversion time
    @param time
           The new bus voltage conversion time
*/
/**************************************************************************/
void Adafruit_INA228::setVoltageConversionTime(INA228_ConversionTime time) {
  Adafruit_I2CRegisterBits voltage_conversion_time =
      Adafruit_I2CRegisterBits(ADC_Config, 3, 9);
  voltage_conversion_time.write(time);
}

/**************************************************************************/
/*!
    @brief Checks if the most recent one shot measurement has completed
    @return true if the conversion has completed
*/
/**************************************************************************/
bool Adafruit_INA228::conversionReady(void) {
  Adafruit_I2CRegisterBits conversion_ready =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 1);
  return conversion_ready.read();
}

/**************************************************************************/
/*!
    @brief Reads the current alert polarity setting
    @return The current bus alert polarity setting
*/
/**************************************************************************/
INA228_AlertPolarity Adafruit_INA228::getAlertPolarity(void) {
  Adafruit_I2CRegisterBits alert_polarity =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 12);
  return (INA228_AlertPolarity)alert_polarity.read();
}
/**************************************************************************/
/*!
    @brief Sets Alert Polarity Bit
    @param polarity
           The polarity of the alert pin
*/
/**************************************************************************/
void Adafruit_INA228::setAlertPolarity(INA228_AlertPolarity polarity) {
  Adafruit_I2CRegisterBits alert_polarity =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 12);
  alert_polarity.write(polarity);
}
/**************************************************************************/
/*!
    @brief Reads the current alert latch setting
    @return The current bus alert latch setting
*/
/**************************************************************************/
INA228_AlertLatch Adafruit_INA228::getAlertLatch(void) {
  Adafruit_I2CRegisterBits alert_latch =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 15);
  return (INA228_AlertLatch)alert_latch.read();
}
/**************************************************************************/
/*!
    @brief Sets Alert Latch Bit
    @param state
           The parameter which asserts the ALERT pin
*/
/**************************************************************************/
void Adafruit_INA228::setAlertLatch(INA228_AlertLatch state) {
  Adafruit_I2CRegisterBits alert_latch =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 15);
  alert_latch.write(state);
}
/**************************************************************************/
/*!
    @brief Reads the 12 possible alert reason bits from DIAG_ALRT
    @return 10 bits that indiccate MEMSTAT (bit 0), CONVRF, POL, BUSUL, BUSOL,
   SHNTUL, SHNTOL, TMPOL, reserved, MATHOF, CHARGEOF, ENERGYOF (bit 11)
*/
/**************************************************************************/
uint16_t Adafruit_INA228::alertFunctionFlags(void) {
  Adafruit_I2CRegisterBits alert_flags =
      Adafruit_I2CRegisterBits(Diag_Alert, 12, 0);
  return alert_flags.read();
}

/**************************************************************************/
/**
 * @brief Gets the current alert type setting.
 *
 * @return INA228_AlertType The current alert type.
 */
/**************************************************************************/
INA228_AlertType Adafruit_INA228::getAlertType(void) {
  uint16_t reg_value = readRegister(INA228_REG_DIAGALRT);
  return static_cast<INA228_AlertType>(reg_value);
}

/**************************************************************************/
/**
 * @brief Sets the alert type.
 *
 * @param alert The alert type to set.
 */
/**************************************************************************/
void Adafruit_INA228::setAlertType(INA228_AlertType alert) {
  uint16_t reg_value = alert;
  writeRegister(INA228_REG_DIAGALRT, reg_value);
}

/**
 * @brief Set the alert limit for a specific fault type.
 * @param limit The threshold value for the alert.
 * @param faultType The type of fault for which to set the limit.
 */
void Adafruit_INA228::setAlertLimit(float limit, INA228_FaultType faultType) {
  uint16_t reg_value;
  switch (faultType) {
  case INA228_FAULT_OVERCURRENT:
    reg_value = static_cast<uint16_t>(limit / (312.5e-9 * 16));
    writeRegister(INA228_REG_SOVL, reg_value);
    break;
  case INA228_FAULT_OVERVOLTAGE:
    reg_value = static_cast<uint16_t>(limit / (195.3125e-6 * 16));
    writeRegister(INA228_REG_BOVL, reg_value);
    break;
    // Additional fault types can be added here
  }
}

// /**
//  * @brief Get the current alert limit for a specific fault type.
//  * @param faultType The type of fault for which to get the limit.
//  * @return The current alert limit value.
//  */
// float Adafruit_INA228::getAlertLimit(INA228_FaultType faultType) {
//   uint16_t reg_value;
//   switch (faultType) {
//     case INA228_FAULT_OVERCURRENT:
//       reg_value = readRegister(INA228_REG_SOVL);
//       return reg_value * (312.5e-9 * 16);
//     case INA228_FAULT_OVERVOLTAGE:
//       reg_value = readRegister(INA228_REG_BOVL);
//       return reg_value * (195.3125e-6 * 16);
//     // Additional fault types can be added here
//   }
//   return 0.0; // Default return value
// }

/**
 * @brief Get the current alert limit for a specific fault type.
 * @param faultType The type of fault for which to get the limit.
 * @return The current alert limit value.
 */
float Adafruit_INA228::getAlertLimit(INA228_FaultType faultType) {
  uint16_t reg_value;
  uint8_t adcRange =
      readRegister(INA228_REG_ADC_CONFIG) & 0x01; // Extracting the ADCRANGE bit

  switch (faultType) {
  case INA228_FAULT_OVERCURRENT:
    reg_value = readRegister(INA228_REG_SOVL);
    if (adcRange == 0) {
      return reg_value * (5e-6); // 5 µV/LSB for ADCRANGE = 0
    } else {
      return reg_value * (1.25e-6); // 1.25 µV/LSB for ADCRANGE = 1
    }

  case INA228_FAULT_OVERVOLTAGE:
    reg_value = readRegister(INA228_REG_BOVL);
    return reg_value * (3.125e-3); // 3.125 mV/LSB

  case INA228_FAULT_UNDERVOLTAGE:
    reg_value = readRegister(INA228_REG_BUVL);
    return reg_value * (3.125e-3); // 3.125 mV/LSB for BUVL

  case INA228_FAULT_TEMPERATURE:
    reg_value = readRegister(TEMP_LIMIT);
    return reg_value * (7.8125e-3); // 7.8125 m°C/LSB
  case INA228_FAULT
  default:
    return 0.0; // Default return value
  }
}

/**
 * @brief Sets the alert limit based on the fault type.
 *
 * @param limit The limit value to set for the specified fault type.
 * @param faultType The type of fault for which to set the limit.
 */
void Adafruit_INA228::setAlertLimit(float limit, INA228_FaultType faultType) {
  uint16_t reg_value;
  switch (faultType) {
  case INA228_FAULT_OVERCURRENT:
    reg_value = static_cast<uint16_t>(
        limit / (312.5e-9 * 16)); // Conversion for overcurrent
    writeRegister(INA228_REG_SOVL, reg_value);
    break;
  case INA228_FAULT_OVERVOLTAGE:
    reg_value = static_cast<uint16_t>(
        limit / (195.3125e-6 * 16)); // Conversion for overvoltage
    writeRegister(INA228_REG_BOVL, reg_value);
    break;
  }
}

/**
 * @brief Sets the Slow Alert function. When enabled, ALERT function is asserted
 * on the completed averaged value. This gives the flexibility to delay the
 * ALERT until after the averaged value. See SLOWALERT bit in DIAG_ALRT register
 *
 * @param enable True to enable Slow Alert, false to disable.
 */
void Adafruit_INA228::setSlowAlert(bool enable) {
  Adafruit_I2CRegisterBits slow_alert =
      Adafruit_I2CRegisterBits(Diag_Alert, 1, 13);
  slow_alert.write(enable);
}