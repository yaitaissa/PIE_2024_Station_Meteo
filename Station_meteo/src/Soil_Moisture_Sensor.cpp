// Soil humidity sensor librairy
#include "Soil_Moisture_Sensor.h"
#include <ArduinoJson.h>

SoilMoistureSensor::SoilMoistureSensor(int Air=3590, int Water=0)
{
    calibrateValues(Air, Water);
}

void SoilMoistureSensor::calibrateValues(int Air, int Water)
{
    // Set the values 
    SoilMoistureSensor::AirValue = Air;
    SoilMoistureSensor::WaterValue  = Water;
}

void SoilMoistureSensor::begin()
{
    // Ensure that Airvalue > WaterValue
    if(AirValue <= WaterValue){
        AirValue = WaterValue + 1;
    }
}

bool SoilMoistureSensor::getMoisture(int *m)
{
    // Read the data
    *m = analogRead(39);
    return 1;
}


bool SoilMoistureSensor::getMoistureRange(int *m, float *r)
{
    // Read the data
    getMoisture(m);

    // Compute the percentage
    *r = 100.0f * (static_cast<float>(AirValue) - static_cast<float>(*m)) / (static_cast<float>(AirValue) - static_cast<float>(WaterValue));

    if (*r < 0.0f) *r = 0.0f;
    if (*r > 100.0f) *r = 100.0f;

    return 1;
}
