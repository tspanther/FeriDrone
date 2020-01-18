/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include "VL53L0X.h"

VL53L0X sensor;
uint8_t led;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(115200);
  Wire.begin();

  sensor.setTimeout(50);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  } else {
    sensor.setSignalRateLimit(0.1);
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange,18);
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange,14);
  }

  sensor.startContinuous(1);
}

void loop()
{
  if (led == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    led = 1;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    led = 0;
  }
  
  uint16_t measur = sensor.readRangeContinuousMillimeters() / 10;
  if (measur > 254) {
    measur = 254;
  }
  uint8_t buf;
  memcpy(&buf, &measur, sizeof(uint8_t));
  if (sensor.timeoutOccurred()) { buf = 255; }
  Serial.write(&buf, sizeof(uint8_t));
}
