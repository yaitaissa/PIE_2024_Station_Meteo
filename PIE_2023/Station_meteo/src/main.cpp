#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include "ATH20.h"
#include <SoftwareSerial.h>
#include "DFRobot_RainfallSensor.h"
#include "time.h"


#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
void messageHandler(char* topic, byte* payload, unsigned int length);
ATH20 ATH;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

char timeDates[32];


float humi ;
float temp;

float pluie;
float pluie_depuis;
int basculement_total;

#define MODE_UART
#ifdef MODE_UART //UART communication
SoftwareSerial mySerial(/*rx =*/16, /*tx =*/17);
DFRobot_RainfallSensor_UART Sensor(/*Stream *=*/&mySerial);
#else //I2C communication
  DFRobot_RainfallSensor_I2C Sensor(&Wire);
#endif


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
StaticJsonDocument<400> doc;
doc["timestamp"]=timeDates;
doc["humidity (%)"] =humi;
doc["temperature(°C)"] = temp;
doc["Pluie tombée dans la dernière heure (mm)"] = pluie;
doc["Pluie dans les dernières 24h (mm)"] = pluie_depuis;
doc["Nombre de basculement depuis 1h"] = basculement_total;
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

void setup()
{
Serial.begin(115200);
configTime(1*3600,0,"pool.ntp.org");
connectAWS();
ATH.begin();

#ifdef MODE_UART
  mySerial.begin(9600);
#endif 
  Serial.begin(9600);

  delay(1000);
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

int ret = ATH.getSensor(&humi, &temp);
if(ret) // GET DATA OK
{
Serial.print("humidity: ");
Serial.print(humi * 100);
Serial.print("%\t temerature: ");
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
// Get the raw data, number of tipping bucket counts
basculement_total = Sensor.getRawData();
Serial.print("rainfall raw:\t");
Serial.println(basculement_total);

publishMessage();
client.loop();
delay((10 * 60 - 2.5) * 1000);
}
