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


bool SoilMoistureSensor::getMoistureRange(int *m, int *r)
{
    getMoisture(m);
    int intervals = (AirValue - WaterValue)/3;

    if(*m > WaterValue && *m < (WaterValue + intervals))
    {
        *r = -1; //Very wet
    }
    else if(*m > (WaterValue + intervals) && *m < (AirValue - intervals))
    {
        *r = 0; //Wet
    }
    else if(*m < AirValue && *m > (AirValue - intervals))
    {
        *r = 1; //Dry
    }

    return 1;
}
