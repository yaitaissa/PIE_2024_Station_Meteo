#ifndef _SOIL_MOISTURE_SENSOR_
#define _SOIL_MOISTURE_SENSOR_

class SoilMoistureSensor{

private:
    void calibrateValues();

public:

    int AirValue;
    int WaterValue;

    void begin();
    bool getMoisture(int *m);
    bool getMoistureRange(int *m, int *r);
};

#endif