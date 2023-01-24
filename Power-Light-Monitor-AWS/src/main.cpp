#include <Arduino.h>
#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <BH1750.h>
#include <Wire.h>

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

BH1750 lightMeter;

// main power supply pin
const int acPin = 13;
// ldr pin
// NOTE: CANNOT USE ADC2 AS WIFI MODE IS ENABLED
const int sensorPin = 36;
// power status
int powStatus, prev_powStatus;
// light status
int lightStatus, prev_lightStatus;
// initial external light intensity value
int lightInit;
// current light intensity value
int lightVal;
// connectivity
int valRSSI;
String intStatus;

void publishMessage();

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char *topic, byte *payload, unsigned int length)
{
  Serial.println("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  // received message
  const int message = doc["message"];

  if(message == 1)
    publishMessage();
  Serial.println(message);
  //publishMessage();
}

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

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

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
  StaticJsonDocument<200> doc;
  doc["Power Status"] = powStatus;
  doc["Light Status"] = lightStatus;
  doc["Internet Status"] = intStatus;
  doc["Testing"] = "Hi from esp32";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  lightMeter.begin();

  pinMode(acPin, INPUT);

  // reads initial intensity of external light
  lightInit = lightMeter.readLightLevel();

  connectAWS();
}

void loop()
{
  int pinState = digitalRead(acPin);
  lightVal = lightMeter.readLightLevel();

  valRSSI = WiFi.RSSI();

  // RSSI conversion table
  
  if(valRSSI <= -90)
    intStatus = "Unusable";
  else if(valRSSI >= -80 && valRSSI < -70 )
    intStatus = "Not Good";
  else if(valRSSI >= -70 && valRSSI < -67)
    intStatus = "Okay";
  else if(valRSSI >= -67 && valRSSI < -30)
    intStatus = "Very Good";
  else
    intStatus = "Amazing";
  

  if (pinState == HIGH)
  {
    powStatus = 1;
    // digitalWrite(logPin, LOW);
    Serial.println("Power ON");
  }
  else
  {
    //publishMessage();
    powStatus = 0;
    // digitalWrite(logPin, HIGH);
    Serial.println("Power OFF");
  }

  // 300 is an experimental value 
  if ((lightVal - lightInit) < 300)
  {
    Serial.print("Billboard ON");
    lightStatus = 0;
  }
  else
  {
    //publishMessage();
    Serial.print("Billboard OFF");
    lightStatus = 1;
  }

  
  if((prev_lightStatus^lightStatus)||(prev_powStatus^powStatus))
    publishMessage();
  

  //delay(200);
  
  prev_lightStatus = lightStatus;
  prev_powStatus = powStatus;
  //publishMessage();
  client.loop();
  delay(1000);
}