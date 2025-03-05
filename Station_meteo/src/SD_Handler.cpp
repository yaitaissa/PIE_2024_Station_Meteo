#include "SD_Handler.h"

// Write in the log file
void logToSD(const char *path, const char *message) {
    // Open the file in append mode
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    // Write in the file
    if (file.println(message)) {
        Serial.println("Message logged to SD");
    } else {
        Serial.println("Failed to log message");
    }
    file.close();
}

// Initialize the log file
void initializeLogFile(const char *path, int station_type) {
    // Checking if the file already exists
    if (!SD.exists(path)) {
        // If not, create it and write the header
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
