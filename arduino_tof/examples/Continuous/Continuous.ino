/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include "VL53L0X.h"

VL53L0X sensor;
int stevec = 0;
long int meritveAvg = 0;

void setup()
{
  pinMode(13, OUTPUT); // PWM out
  digitalWrite(13, HIGH);
  
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(500);
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
  uint16_t measurement = sensor.readRangeContinuousMillimeters();
  Serial.print(measurement);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  
  digitalWrite(13, LOW);
  if (measurement > 2000) {
    measurement = 2000;
  }
  delayMicroseconds(measurement);
  digitalWrite(13, HIGH);

  Serial.println();
}
