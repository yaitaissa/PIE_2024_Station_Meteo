// ATH20 ARDUINO LIBRARY
#include "Soil_Moisture_Sensor.h"
#include <ArduinoJson.h>

void SoilMoistureSensor::calibrateValues()
{
    SoilMoistureSensor::AirValue = 570; //Value to record in the air
    SoilMoistureSensor::WaterValue  = 0; //Value to record in water
}

void SoilMoistureSensor::begin()
{
    calibrateValues();
}

bool SoilMoistureSensor::getMoisture(int *m)
{
    *m = analogRead(39);
    return 1;
}


bool SoilMoistureSensor::getMoistureRange(int *m, float *r)
{
    getMoisture(m);
    *r = (1.0 - *m / AirValue) * 100;

    return 1;
}
