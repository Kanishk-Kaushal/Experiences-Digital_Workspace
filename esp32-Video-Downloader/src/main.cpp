#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "UDHttp.h"
#include "mySD.h"


const char* ssid     = "OnePlus95g";
const char* password = "password";

ext::File root;
//these callbacks will be invoked to read and write data to sdcard
//and process response
//and showing progress 
int responsef(uint8_t *buffer, int len){
  Serial.printf("%s\n", buffer);
  return 0;
}
//read data callback
int rdataf(uint8_t *buffer, int len){
  //read file to upload
  if (root.available()) {
    return root.read(buffer, len);
  }
  return 0;
}
//write data callback
int wdataf(uint8_t *buffer, int len){
  //write downloaded data to file
  return root.write(buffer, len);
}

void progressf(int percent){
  Serial.printf("%d\n", percent);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  Serial.print("Initializing SD card...");
  if (!SD.begin(32, 14, 12, 27)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  SD.remove("/test.mp4");
  {
    
    //UDHttp udh;
    HTTPClient http;
    //open file on sdcard to write
    root = SD.open("/test1.mp4", FILE_WRITE);
    if (!root) {
       Serial.println("can not open file!");
       return;
    }
    //download the file from url
    //udh.download("https://sindhbad.experiences.digital//public/images/bacardi.mp4", wdataf, progressf);
    http.begin("https://sindhbad.experiences.digital//public/images/videocomp.mp4");
    int httpCode = http.GET(); // Make the request

    if (httpCode == HTTP_CODE_OK)
    { // Check for the returning code
      http.writeToStream(&root);
    }
    root.close();
    Serial.printf("done downloading\n");
    http.end();

  }

  /*
  {
    UDHttp udh;
    //open file on sdcard to read
    root = SD.open("test.pdf");
    if (!root) {
       Serial.println("can not open file!");
       return;
    }
    //upload downloaded file to local server
    udh.upload("http://192.168.1.107:80/upload/upload.php", "test.pdf", root.size(), rdataf, progressf, responsef);
    root.close();
    Serial.printf("done uploading\n");
  }
  */
}

void loop() {
  // put your main code here, to run repeatedly:

}