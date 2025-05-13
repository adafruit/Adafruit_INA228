#include <Adafruit_INA238.h>

Adafruit_INA238 ina238 = Adafruit_INA238();

void setup() {
  Serial.begin(115200);
  // Wait until serial port is opened
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit INA238 Test");

  if (!ina238.begin()) {
    Serial.println("Couldn't find INA238 chip");
    while (1)
      ;
  }
  Serial.println("Found INA238 chip");
  // set shunt resistance and max current
  ina238.setShunt(0.015, 10.0);

  ina238.setAveragingCount(INA2XX_COUNT_16);
  uint16_t counts[] = {1, 4, 16, 64, 128, 256, 512, 1024};
  Serial.print("Averaging counts: ");
  Serial.println(counts[ina238.getAveragingCount()]);

  // set the time over which to measure the current and bus voltage
  ina238.setVoltageConversionTime(INA2XX_TIME_150_us);
  Serial.print("Voltage conversion time: ");
  switch (ina238.getVoltageConversionTime()) {
  case INA2XX_TIME_50_us:
    Serial.print("50");
    break;
  case INA2XX_TIME_84_us:
    Serial.print("84");
    break;
  case INA2XX_TIME_150_us:
    Serial.print("150");
    break;
  case INA2XX_TIME_280_us:
    Serial.print("280");
    break;
  case INA2XX_TIME_540_us:
    Serial.print("540");
    break;
  case INA2XX_TIME_1052_us:
    Serial.print("1052");
    break;
  case INA2XX_TIME_2074_us:
    Serial.print("2074");
    break;
  case INA2XX_TIME_4120_us:
    Serial.print("4120");
    break;
  }
  Serial.println(" uS");

  ina238.setCurrentConversionTime(INA2XX_TIME_280_us);
  Serial.print("Current conversion time: ");
  switch (ina238.getCurrentConversionTime()) {
  case INA2XX_TIME_50_us:
    Serial.print("50");
    break;
  case INA2XX_TIME_84_us:
    Serial.print("84");
    break;
  case INA2XX_TIME_150_us:
    Serial.print("150");
    break;
  case INA2XX_TIME_280_us:
    Serial.print("280");
    break;
  case INA2XX_TIME_540_us:
    Serial.print("540");
    break;
  case INA2XX_TIME_1052_us:
    Serial.print("1052");
    break;
  case INA2XX_TIME_2074_us:
    Serial.print("2074");
    break;
  case INA2XX_TIME_4120_us:
    Serial.print("4120");
    break;
  }
  Serial.println(" uS");

  // default polarity for the alert is low on ready, but
  // it can be inverted!
  // ina238.setAlertPolarity(1);
}

void loop() {

  // by default the sensor does continuous reading, but
  // we can set to triggered mode. to do that, we have to set
  // the mode to trigger a new reading, then wait for a conversion
  // either by checking the ALERT pin or reading the ready register
  // ina238.setMode(INA2XX_MODE_TRIGGERED);
  // while (!ina238.conversionReady())
  //  delay(1);

  Serial.print("Current: ");
  Serial.print(ina238.getCurrent_mA());
  Serial.println(" mA");

  Serial.print("Bus Voltage: ");
  Serial.print(ina238.getBusVoltage_V());
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(ina238.getShuntVoltage_mV() * 1000.0); // Convert from mV to μV
  Serial.println(" uV");

  Serial.print("Power: ");
  Serial.print(ina238.getPower_mW());
  Serial.println(" mW");

  Serial.print("Temperature: ");
  Serial.print(ina238.readDieTemp());
  Serial.println(" *C");

  Serial.println();
  delay(1000);
}