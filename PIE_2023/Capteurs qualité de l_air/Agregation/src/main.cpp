#include <DFRobot_ENS160.h>
#include "DFRobot_AirQualitySensor.h"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "time.h"
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define I2C_COMMUNICATION //I2C communication. Comment out this line of code if you want to use SPI communication.
void messageHandler(char* topic, byte* payload, unsigned int length);
#ifdef I2C_COMMUNICATION
/**
* For Fermion version, the default I2C address is 0x53, connect SDO pin to GND and I2C address will be 0x52
*/
DFRobot_ENS160_I2C ENS160(&Wire, /*I2CAddr*/ 0x52);
DFRobot_AirQualitySensor particle(&Wire, 0x19);
#else
/**
* Set up digital pin according to the on-board pin connected with SPI chip-select pin
* csPin Available Pins. For example: ESP32&ESP8266(D3), m0(6)
*/
uint8_t csPin = D3;
DFRobot_ENS160_SPI ENS160(&SPI, csPin);
#endif
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
char timeDates[32];
uint16_t TVOC = ENS160.getTVOC();
uint16_t ECO2 = ENS160.getTVOC();

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



void publishMessage()
{
StaticJsonDocument<800> doc;

doc["timestamp"] = timeDates;
doc["TCOV"] = TVOC;
doc["CO2"] = ECO2;
doc["PM2.5"] = particle.gainParticleConcentration_ugm3(PARTICLE_PM2_5_ATMOSPHERE);
doc["PM1.0"] = particle.gainParticleConcentration_ugm3(PARTICLE_PM1_0_ATMOSPHERE);
doc["PM10"] = particle.gainParticleConcentration_ugm3(PARTICLE_PM10_ATMOSPHERE);
char jsonBuffer[512];
serializeJson(doc, jsonBuffer); // print to client
client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}



void setup(void)
{
Serial.begin(115200);
configTime(1 * 3600, 0, "pool.ntp.org");

connectAWS();

// Init the sensor
while( NO_ERR != ENS160.begin() ){
Serial.println("Communication with CO2 Sensor failed, please check connection");
delay(3000);
}
while (!particle.begin()) {
 Serial.println("Communication with Air Quality Sensor failed, please check connection");
 delay(3000);
}
Serial.println("Begin ok!");
/**
* Set power mode
* mode Configurable power mode:
* ENS160_SLEEP_MODE: DEEP SLEEP mode (low power standby)
* ENS160_IDLE_MODE: IDLE mode (low-power)
* ENS160_STANDARD_MODE: STANDARD Gas Sensing Modes
*/
ENS160.setPWRMode(ENS160_STANDARD_MODE);
/**
* Users write ambient temperature and relative humidity into ENS160 for calibration and compensation of the
measured gas data.
* ambientTemp Compensate the current ambient temperature, float type, unit: C
* relativeHumidity Compensate the current ambient humidity, float type, unit: %rH
*/
ENS160.setTempAndHum(/*temperature=*/25.0, /*humidity=*/50.0);


uint8_t version = particle.gainVersion();
Serial.print("version is : ");
Serial.println(version);
delay(1000);

}



void loop()
{

struct tm timeinfo;
if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
}else{
//Serial.println(&timeinfo, "&A, %B %d %Y %H:%M:%S");
strftime(timeDates, sizeof(timeDates), "%B %d %Y %H:%M:%S", &timeinfo); //&A, 
Serial.println(timeDates);
}




/**
* Get the sensor operating status
* Return value: 0-Normal operation,
* 1-Warm-Up phase, first 3 minutes after power-on.
* 2-Initial Start-Up phase, first full hour of operation after initial power-on. Only once in the sensor’s lifetime.
* note: Note that the status will only be stored in the non-volatile memory after an initial 24h of continuous
* operation. If unpowered before conclusion of said period, the ENS160 will resume "Initial Start-up" mode
* after re-powering.
*/
uint8_t Status = ENS160.getENS160Status();
Serial.print("Sensor operating status : ");
Serial.println(Status);
/**
* Get the air quality index
* Return value: 1-Excellent, 2-Good, 3-Moderate, 4-Poor, 5-Unhealthy
*/
uint8_t AQI = ENS160.getAQI();
Serial.print("Air quality index : ");
Serial.println(AQI);
Serial.println();

Serial.println("CO² sensor:");
/**
* Get TVOC concentration
* Return value range: 0–65000, unit: ppb
*/
TVOC= ENS160.getTVOC();
Serial.print("Concentration of total volatile organic compounds : ");
Serial.print(TVOC);
Serial.println(" ppb");
/**
* Get CO2 equivalent concentration calculated according to the detected data of VOCs and hydrogen (eCO2 –
Equivalent CO2)
* Return value range: 400–65000, unit: ppm
* Five levels: Excellent(400 - 600), Good(600 - 800), Moderate(800 - 1000),
* Poor(1000 - 1500), Unhealthy(> 1500)
*/
ECO2= ENS160.getECO2();
Serial.print("Carbon dioxide equivalent concentration : ");
Serial.print(ECO2);
Serial.println(" ppm");
Serial.println();

Serial.println("PM sensor");
uint16_t PM2_5 = particle.gainParticleConcentration_ugm3(PARTICLE_PM2_5_ATMOSPHERE);
uint16_t PM1_0 = particle.gainParticleConcentration_ugm3(PARTICLE_PM1_0_ATMOSPHERE);
uint16_t PM10 = particle.gainParticleConcentration_ugm3(PARTICLE_PM10_ATMOSPHERE);

Serial.print("PM2.5 concentration:");
Serial.print(PM2_5);
Serial.println(" ug/m3");
Serial.print("PM1.0 concentration:");
Serial.print(PM1_0);
Serial.println(" ug/m3");
Serial.print("PM10 concentration:");
Serial.print(PM10);
Serial.println(" ug/m3");
Serial.println();
Serial.println();


publishMessage();
client.loop();

delay(1000);
}





void messageHandler(char* topic, byte* payload, unsigned int length)
{
Serial.print("incoming: ");
Serial.println(topic);
StaticJsonDocument<800> doc;
deserializeJson(doc, payload);
const char* message = doc["message"];
Serial.println(message);
}
