#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include "Photo.h"
#include "time.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// int sensorValue = 0;
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
TinyGPSPlus gps;

#define MODE_UART
#ifdef MODE_UART //UART communication  



double lat, lon;
char heure[3];
char minute[3];
char seconde[3];
char jour[3];
char annee[3];
char mois[3];
char heureComplete[10];
char dateComplete[10];

const int photoresistorPin = 36;
int sensorValue = 0;

char data_to_send[30];
SoftwareSerial mySerial(16, 17); // RX, TX

#endif 

void messageHandler(char* topic, byte* payload, unsigned int length);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
char timeDates[32];



void connectAWS()
{
Serial.print("trying to connect");
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
doc["Date"] = dateComplete;
doc["Horaire"] = heureComplete;
doc["longitude"] = lon;
doc["latitude"] = lat;
doc["luminosité"] = sensorValue;
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




int i =0;
int a =0;

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    lat = gps.location.lat();
    Serial.print(gps.location.lat(), 6);
    lon = (gps.location.lng());
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {

    sprintf(mois, "%02d", gps.date.month());
    sprintf(jour, "%02d", gps.date.day());
    sprintf(annee, "%02d", gps.date.year());
    sprintf(dateComplete, "%s.%s.%s", jour, mois, annee);

    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {

    sprintf(heure, "%02d", gps.time.hour()+1);
    sprintf(minute, "%02d", gps.time.minute());
    sprintf(seconde, "%02d", gps.time.second());
    sprintf(heureComplete, "%s.%s.%s", heure, minute, seconde);

    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void setup() {
  Serial.begin(115200);      // For debugging purposes
  configTime(1 * 3600, 0, "pool.ntp.org");
  mySerial.begin(9600); 
  connectAWS();  



  Serial.println();
  Serial.println(F("Done."));
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
  // This sketch displays information every time a new sentence is correctly encoded.
  while (mySerial.available() > 0)
    if (gps.encode(mySerial.read()))
      displayInfo();

  sensorValue =  analogRead(photoresistorPin);

  publishMessage();
  client.loop();
  memset(data_to_send,0,30); 
  
  delay(1000);
}


