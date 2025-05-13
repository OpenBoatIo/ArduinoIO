#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "TimeManager.h"

// Pin config
#define ONE_WIRE_BUS 4

// Sensor objects (must be defined *somewhere* for linker to find them)
inline OneWire oneWire(ONE_WIRE_BUS);
inline DallasTemperature sensors(&oneWire);

// Init temperature sensor
inline void setupTemperatureSensor() {
  sensors.begin();
}

// Read temperature
inline float readTemperature() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  return temp;
}

#endif

void ReadTempSensors() {
  static unsigned long lastReadTime = 0;
  static float lastTemp = NAN;
  static unsigned long lastSamePrintTime = 0;

  unsigned long currentTime = millis();

  // Only run once per second
  if (currentTime - lastReadTime < 1000) return;
  lastReadTime = currentTime;

  float temp = readTemperature();

  if (temp == DEVICE_DISCONNECTED_C || temp == -127.0) {
    String tsError="{\"Type\": \"Temp\",\"Meta\": {\"Date\": \""+getTimestamp() +"\",\"Error\":\"Temperature sensor not found!\"},\"Data\": {\"value\": \""+String(temp, 1)+"\"}}";
    addLog(tsError);
    return;
  }

  bool tempChanged = isnan(lastTemp) || abs(temp - lastTemp) >= 0.1;
String ts="{\"Type\": \"Temp\",\"Meta\": {\"Date\": \""+getTimestamp() +"\"},\"Data\": {\"value\": \""+String(temp, 1)+"\"}}";

  if (tempChanged) {    
    addLog(ts);
    lastTemp = temp;
    lastSamePrintTime = currentTime;
  } else if (currentTime - lastSamePrintTime >= 10000) {
    // Temperature unchanged, but 10 seconds passed – log again
    addLog(ts);
    lastSamePrintTime = currentTime;
  }
}