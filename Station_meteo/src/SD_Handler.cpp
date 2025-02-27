#include "SD_Handler.h"

void logToSD(const char *path, const char *message) {
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.println(message)) {
        Serial.println("Message logged to SD");
    } else {
        Serial.println("Failed to log message");
    }
    file.close();
}

void initializeLogFile(const char *path, int station_type) {
    // Checking if the file exists
    if (!SD.exists(path)) {
        File file = SD.open(path, FILE_WRITE);
        if (file) {
            if(station_type){
                file.println("Date, Humidity (%), Temperature (°C), Rain (mm), Cumulative Rain (mm), Total Tipping Count, Light (lux), Soil Moisture (%), Humidity Purcentage (%)");
            }
            else{
                file.println("Date, Humidity (%), Temperature (°C), Soil Moisture (%), Humidity Purcentage (%)");
            }
            Serial.println("Header added to log file");
            file.close();
        } else {
            Serial.println("Failed to create log file");
        }
    }
}
