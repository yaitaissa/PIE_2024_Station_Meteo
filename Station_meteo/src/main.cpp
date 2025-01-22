/*
Ref for SD handling: https://www.upesy.fr/blogs/tutorials/upesy-microsd-module-quickstart-guide#
*/

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include "time.h"

//Sensors libraries
#include "Adafruit_VEML7700.h" //Light sensor
#include "DFRobot_RainfallSensor.h" //Rainfall sensor
#include "ATH20.h" //Temperature and air humidity sensor
#include "Soil_Moisture_Sensor.h" //Soil humidity sensor

//SD card librairies
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define ATH20_VERSION 1 // the version of the code to use for the Temperature and air humidity sensor (1 : ATH20, 0 : DFRobot_ATH20)

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
void messageHandler(char* topic, byte* payload, unsigned int length);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

char timeDates[32];


float humi ;
float temp;
float pluie;
float pluie_depuis;
int basculement_total;
float lux_value;
int soil_moisture;
float humidity_purcentage; //0%: same humidity as air, 100%: same humidity as water

// Create sensors objects
Adafruit_VEML7700 veml = Adafruit_VEML7700();
SoilMoistureSensor soilSensor;
#if ATH20_VERSION
  ATH20 ATH;
#else
  DFRobot_AHT20 ATH;
#endif
DFRobot_RainfallSensor_I2C Sensor(&Wire);

// Connect to AWS
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);
  Serial.println("Connecting to AWS IOT");
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}

// Send a message to the server
void publishMessage()
{
  StaticJsonDocument<400> doc;
  doc["timestamp"]=timeDates;
  doc["humidity (%)"] =humi;
  doc["temperature(°C)"] = temp;
  doc["Pluie tombée dans la dernière heure (mm)"] = pluie;
  doc["Pluie dans les dernières 24h (mm)"] = pluie_depuis;
  doc["Nombre de basculement depuis 1h"] = basculement_total;
  doc["Luminosité (lux)"] = lux_value;
  doc["Humidité relative du sol (valeur relative entre AirValue et WaterValue)"] = soil_moisture;
  doc["Pourcentage d'humidité du sol"] = humidity_purcentage;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

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

void initializeLogFile(const char *path) {
    if (!SD.exists(path)) {
        File file = SD.open(path, FILE_WRITE);
        if (file) {
            file.println("Date, Humidity (%), Temperature (°C), Rain (mm), Cumulative Rain (mm), Total Tipping Count, Light (lux), Soil Moisture (%), Humidity Purcentage (%)");
            Serial.println("Header added to log file");
            file.close();
        } else {
            Serial.println("Failed to create log file");
        }
    }
}



void setup()
{
  Serial.begin(9600);
  configTime(1*3600,0,"pool.ntp.org");
  connectAWS();

  // Initializing ATH sensor
  ATH.begin();
  #if !ATH20_VERSION
    ATH.reset();
  #endif
  
  // Initializing Soil Huimidity sensor
  soilSensor.begin();

  // Initializing Rainfall sensor
  while(!Sensor.begin()){
    Serial.println("Sensor init err!!!");
    delay(1000);
  }
  Serial.print("vid:\t");
  Serial.println(Sensor.vid,HEX);
  Serial.print("pid:\t");
  Serial.println(Sensor.pid,HEX);
  Serial.print("Version:\t");
  Serial.println(Sensor.getFirmwareVersion());

  //Set the accumulated rainfall value, unit: mm
  //Sensor.setRainAccumulatedValue();

  // Initializing Lux sensor
  while(!veml.begin()){
    Serial.println("Light sensor init err!!!");
    delay(1000);
  }

  Serial.print("Gain:\t");
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print("Integration Time (ms):\t");
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }
  //Set the bit thresholds for the light sensor
  //veml.setLowThreshold(10000);
  //veml.setHighThreshold(20000);
  //veml.interruptEnable(true);

  if (!SD.begin(5)) { // Remplacez 5 par le pin CS si nécessaire
        Serial.println("SD Card Mount Failed");
        return;
    }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("UNKNOWN");

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Initialiser le fichier avec l'en-tête si nécessaire
  initializeLogFile("/sensor_data.txt");

}

void loop()
{

  connectAWS();

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }else{
    //Serial.println(&timeinfo, "&A, %B %d %Y %H:%M:%S");
    strftime(timeDates, sizeof(timeDates), "%B %d %Y %H:%M:%S", &timeinfo); //&A, 
    Serial.println(timeDates);
  }
  int ret;

  #if ATH20_VERSION
    ret = ATH.getSensor(&humi, &temp);
  #else
    ret = ATH.startMeasurementReady();
    humi = ATH.getHumidity_RH();
    temp = ATH.getTemperature_C();
  #endif

  if(ret) // GET DATA OK
  {
    Serial.print("humidity: ");
    Serial.print(humi * 100);
    Serial.print("%\t temperature: ");
    Serial.println(temp);
  }
  else // GET DATA FAIL
  {
    Serial.println("GET DATA FROM ATH20 FAIL");
  }

  //Get the accumulated rainfall during the sensor operating time
  pluie = Sensor.getRainfall();
  Serial.print("Quantité d'eau tombée pendant cette session:\t");
  Serial.print(pluie);
  Serial.println(" mm");
  //Get the accumulated rainfall in the past 1 hour (function parameter can be 1-24)
  pluie_depuis = Sensor.getRainfall(24);
  Serial.print("Quantité d'eau tombée dans les dernières 24h:\t");
  Serial.print(pluie_depuis);
  Serial.println(" mm");
  //Get the raw data, number of tipping bucket counts
  basculement_total = Sensor.getRawData();
  Serial.print("rainfall raw:\t");
  Serial.println(basculement_total);

  //Get the lux value
  lux_value = veml.readLux();
  Serial.print("Lux:\t");
  Serial.println(lux_value);
  uint16_t irq = veml.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold");
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold");
  }

  //Get the soil humidity 
  soilSensor.getMoistureRange(&soil_moisture, &humidity_purcentage);
  Serial.print("Soil humidity:\t");
  Serial.println(soil_moisture);
  Serial.print("Soil humidity purcentage:\t");
  Serial.println(humidity_purcentage);

  //publishMessage();
  //client.loop();


  // Création d'une ligne de données
  char logMessage[512];
  snprintf(logMessage, sizeof(logMessage),
            "%s, %.2f, %.2f, %.2f, %.2f, %d, %.2f, %d, %.2f",
            timeDates, humi, temp, pluie, pluie_depuis, basculement_total,
            lux_value, soil_moisture, humidity_purcentage);

  // Écriture dans le fichier
  logToSD("/sensor_data.txt", logMessage);

  delay( 1000);
}
