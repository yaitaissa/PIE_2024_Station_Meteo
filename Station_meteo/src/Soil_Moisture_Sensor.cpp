// ATH20 ARDUINO LIBRARY
#include "Soil_Moisture_Sensor.h"
#include <ArduinoJson.h>

SoilMoistureSensor::SoilMoistureSensor()
{
    calibrateValues();
}

void SoilMoistureSensor::calibrateValues()
{
    SoilMoistureSensor::AirValue = 3590; //Value to record in the air
    SoilMoistureSensor::WaterValue  = 240; //Value to record in water
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
    *r = 100.0f * (static_cast<float>(AirValue) - static_cast<float>(*m)) / (static_cast<float>(AirValue) - static_cast<float>(WaterValue));

    if (*r < 0.0f) *r = 0.0f;
    if (*r > 100.0f) *r = 100.0f;

    return 1;
}
