// Global librairy with declarations
#include "global.h"

// Sensors libraries
#if GREENGUARD
  #include "Adafruit_VEML7700.h" //Light sensor
  #include "DFRobot_RainfallSensor.h" //Rainfall sensor
#endif
#include "ATH20.h" //Temperature and air humidity sensor
#include "Soil_Moisture_Sensor.h" //Soil humidity sensor

// SD card librairy
#include "SD_Handler.h"

// Create sensors objects
#if GREENGUARD // If the station is a GREENGUARD, create the lux sensor and rainfall sensor objects
  Adafruit_VEML7700 veml = Adafruit_VEML7700();
  DFRobot_RainfallSensor_I2C Sensor(&Wire);
#endif
#if ATH20_VERSION // Create the ATH sensor according to it's version
  ATH20 ATH;
#else
  DFRobot_AHT20 ATH;
#endif
SoilMoistureSensor soilSensor;

// Connect to AWS
void connectAWS()
{
  // Connect to WiFi
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
  doc["Humidité relative du sol (valeur relative entre AirValue et WaterValue)"] = soil_moisture;
  doc["Pourcentage d'humidité du sol"] = humidity_purcentage;
  #if GREENGUARD
    doc["Pluie tombée dans la dernière heure (mm)"] = pluie;
    doc["Pluie dans les dernières 24h (mm)"] = pluie_depuis;
    doc["Nombre de basculement depuis 1h"] = basculement_total;
    doc["Luminosité (lux)"] = lux_value;
  #endif
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

// Initialization function
void setup()
{
  Serial.begin(9600);
  configTime(1*3600,0,"pool.ntp.org");
  
  // Connect to the Internet and initialize the <time.h> librairy
  connectAWS();
  while(!getLocalTime(&timeinfo));
  time(&seconds);

  // Initialize ATH sensor
  ATH.begin();
  #if !ATH20_VERSION
    ATH.reset();
  #endif
  
  // Initialize Soil Huimidity sensor
  soilSensor.begin();

  #if GREENGUARD
    // Initialize Rainfall sensor
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
    Sensor.setRainAccumulatedValue();

    // Initialize Lux sensor
    while(!veml.begin()){
      Serial.println("Light sensor init err!!!");
      delay(1000);
    }

    // Print the gain of the sensor
    Serial.print("Gain:\t");
    switch (veml.getGain()) {
      case VEML7700_GAIN_1: Serial.println("1"); break;
      case VEML7700_GAIN_2: Serial.println("2"); break;
      case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
      case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
    }

    // Print the integration time of the sensor
    // (the time over which the sensor takes the measurement)
    Serial.print("Integration Time (ms):\t");
    switch (veml.getIntegrationTime()) {
      case VEML7700_IT_25MS: Serial.println("25"); break;
      case VEML7700_IT_50MS: Serial.println("50"); break;
      case VEML7700_IT_100MS: Serial.println("100"); break;
      case VEML7700_IT_200MS: Serial.println("200"); break;
      case VEML7700_IT_400MS: Serial.println("400"); break;
      case VEML7700_IT_800MS: Serial.println("800"); break;
    }

    // Set the bit thresholds for the light sensor
    veml.setLowThreshold(10000);
    veml.setHighThreshold(20000);
    veml.interruptEnable(true);
  #endif

  // Mount the SD card
  if (!SD.begin(5)) {
        Serial.println("SD Card Mount Failed");
        return;
    }

  // Check the presence of an SD card
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
      return;
  }

  // Print the type of SD card
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("UNKNOWN");

  // Print the size of the SD card
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Initialize the file
  initializeLogFile("/sensor_data.txt", GREENGUARD);
}

void loop()
{
  // Try to get the time
  if(!getLocalTime(&timeinfo)){
    // If failure, use the <time.h> librairy to get the time and transform it into a date
    seconds = time(NULL);
    struct tm * temp_timeinfo = localtime(&seconds);
    strftime(timeDates, sizeof(timeDates), "%B %d %Y %H:%M:%S", temp_timeinfo);
  }else{
    // If succes, directly get the date 
    strftime(timeDates, sizeof(timeDates), "%B %d %Y %H:%M:%S", &timeinfo); //&A, 
  }
  Serial.println(timeDates);

  // Get the temperature and air humidity measurement
  #if ATH20_VERSION
    ret = ATH.getSensor(&humi, &temp);
    humi = humi*100;
  #else
    ret = ATH.startMeasurementReady();
    humi = ATH.getHumidity_RH();
    temp = ATH.getTemperature_C();
  #endif

  if(ret) // If measurement successfully done, print it
  {
    Serial.print("humidity: ");
    Serial.print(humi);
    Serial.print("%\t temperature: ");
    Serial.println(temp);
  }
  else // Else, notify the operator
  {
    Serial.println("GET DATA FROM ATH20 FAIL");
  }

  //Get the soil humidity measurement and print it
  soilSensor.getMoistureRange(&soil_moisture, &humidity_purcentage);
  Serial.print("Soil humidity:\t");
  Serial.println(soil_moisture);
  Serial.print("Soil humidity purcentage:\t");
  Serial.println(humidity_purcentage);

  #if GREENGUARD
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
    irq = veml.interruptStatus();
    if (irq & VEML7700_INTERRUPT_LOW) {
      Serial.println("** Low threshold");
    }
    if (irq & VEML7700_INTERRUPT_HIGH) {
      Serial.println("** High threshold");
    }
  #endif

  // Create a data line to store
  char logMessage[512];
  #if GREENGUARD
    snprintf(logMessage, sizeof(logMessage),
              "%s, %f, %f, %f, %f, %d, %f, %d, %f",
              timeDates, humi, temp, pluie, pluie_depuis, basculement_total,
              lux_value, soil_moisture, humidity_purcentage);
  #else
    snprintf(logMessage, sizeof(logMessage),
              "%s, %f, %f, %d, %f",
              timeDates, humi, temp,
              soil_moisture, humidity_purcentage);
  #endif

  // Write the data line in the file
  logToSD("/sensor_data.txt", logMessage);

  // Wait before next measurement
  delay(PSEUDO_PERIOD*1000);
}
