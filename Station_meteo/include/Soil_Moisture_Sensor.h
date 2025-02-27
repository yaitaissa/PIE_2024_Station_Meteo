#ifndef _SOIL_MOISTURE_SENSOR_
#define _SOIL_MOISTURE_SENSOR_

class SoilMoistureSensor{

private:
    void calibrateValues();

public:

    int AirValue;
    int WaterValue;

    SoilMoistureSensor(); // Constructeur
    void begin();
    bool getMoisture(int *m);
    bool getMoistureRange(int *m, float *r);
};

#endif