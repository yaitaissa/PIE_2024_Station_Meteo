#ifndef _GLOBAL
#define _GLOBAL

// Include useful librairies
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include "time.h"

// Define the station 
#define ATH20_VERSION 1 // Version of the code to use for the Temperature and air humidity sensor (1 : ATH20, 0 : DFRobot_ATH20)
#define GREENGUARD 0    // Version of the station (0 : AQUACHECK, 1 : GREENGUARD)

// Define the dealy between measurement
#define PSEUDO_PERIOD 60 // in (s)

// Declare global variables
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
void messageHandler(char* topic, byte* payload, unsigned int length);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

char timeDates[32];

struct tm timeinfo;
time_t seconds;

int ret;
uint16_t irq;

float humi ;
float temp;
float pluie;
float pluie_depuis;
int basculement_total;
float lux_value;
int soil_moisture;
float humidity_purcentage; //0%: same humidity as air, 100%: same humidity as water

#endif