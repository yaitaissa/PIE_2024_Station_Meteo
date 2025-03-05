#ifndef _SOIL_MOISTURE_SENSOR_
#define _SOIL_MOISTURE_SENSOR_

// Class describing the Soil humidity sensor
class SoilMoistureSensor{

private:
    /**
     * @fn calibrateValues
     * @brief Set up AirValue and WaterValue
     * 
     * @param Air Value measured in the air
     * @param Water Value measured in water
     */
    void calibrateValues(int Air, int Water);
    
    int AirValue;
    int WaterValue;

public:
    /**
     * @fn SoilMoistureSensor
     * @brief Construct a new Soil Moisture Sensor object
     * 
     * @param Air Value measured in the air
     * @param Water Value measured in water
     */
    SoilMoistureSensor(int Air=3590, int Water=0); 

    /**
     * @fn begin
     * @brief Initialize the sensor
     * 
     */
    void begin();

    /**
     * @fn getMoisture
     * @brief Get the measured value
     * 
     * @param m Variable in which to store the value
     * @return true if the measurement was successful
     * @return false otherwise
     */
    bool getMoisture(int *m);

    /**
     * @fn getMoistureRange
     * @brief Get the measured value and the corresponding percentage
     * 
     * @param m Variable in which to store the value
     * @param r Variable in which to store the corresponding percentage
     * @return true if the measurement was successful
     * @return false otherwise
     */
    bool getMoistureRange(int *m, float *r);
};

#endif