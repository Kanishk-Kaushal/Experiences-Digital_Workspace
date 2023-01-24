/* Get all possible data from MPU6050
 * Accelerometer values are given as multiple of the gravity [1g = 9.81 m/s²]
 * Gyro values are given in deg/s
 * Angles are given in degrees
 * Note that X and Y are tilt angles and not pitch/roll.
 * Kanishk Kaushal
 */

#include <Arduino.h>
#include "Wire.h"
#include <MPU6050_light.h>
#include "Secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

String bbStatus;

void printData(void);

MPU6050 mpu(Wire);

long timer = 0;

float init_r, init_p, init_y;
float r, p, y;

int iter = 0;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
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
  doc["Billboard Status"] = bbStatus;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  connectAWS();

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0)
  {
  } // stop everything if could not connect to MPU6050

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true, true); // gyro and accelero
  Serial.println("Done!\n");

  mpu.update();
}

void loop()
{
  mpu.update();

  if (millis() - timer > 1000)
  {
    iter++; // Read initial values at 5th iteration | Inital values are noise-y in the setup function, therefore using this approach

    if(iter < 5)
    {
      Serial.println("Callibrating...");
    }

    if (iter == 5)
    {
      Serial.println("Done Callibrating.");
      init_r = mpu.getAngleX();
      init_p = mpu.getAngleY();
      init_y = mpu.getAngleZ();
    }

    r = mpu.getAngleX();
    p = mpu.getAngleY();
    y = mpu.getAngleZ();

    if (iter > 5)
      iter = 6;

    if (iter == 6)
    {
      if ((abs(r - init_r) >= 2) || (abs(p - init_p) >= 2) || (abs(y - init_y) >= 2))
      {
        Serial.println("BILLBOARD MUST BE CHECKED");
        bbStatus = "BILLBOARD MUST BE CHECKED";
      }
      else
      {
        Serial.println("BILLBOARD SEEMS FINE");
        bbStatus = "BILLBOARD SEEMS FINE";
      }
    }

    // printData();
    timer = millis();
    publishMessage();
    client.loop();
  }
}

void printData(void)
{
  // print data every second
  Serial.print("Init Values :");
  Serial.println(init_r);
  Serial.println(init_p);
  Serial.println(init_y);
  Serial.println(F("TEMPERATURE: "));
  Serial.println(mpu.getTemp());
  Serial.print(F("ACCELERO  X: "));
  Serial.print(mpu.getAccX());
  Serial.print("\tY: ");
  Serial.print(mpu.getAccY());
  Serial.print("\tZ: ");
  Serial.println(mpu.getAccZ());

  Serial.print(F("GYRO      X: "));
  Serial.print(mpu.getGyroX());
  Serial.print("\tY: ");
  Serial.print(mpu.getGyroY());
  Serial.print("\tZ: ");
  Serial.println(mpu.getGyroZ());

  Serial.print(F("ACC ANGLE X: "));
  Serial.print(mpu.getAccAngleX());
  Serial.print("\tY: ");
  Serial.println(mpu.getAccAngleY());

  Serial.print(F("ANGLE     X: "));
  Serial.print(mpu.getAngleX());
  Serial.print("\tY: ");
  Serial.print(mpu.getAngleY());
  Serial.print("\tZ: ");
  Serial.println(mpu.getAngleZ());
  Serial.println(F("=====================================================\n"));
}