#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SPIFFS.h"
#include "SPI.h"
#include "TFT_eSPI.h"

#define FORMAT_SPIFFS_IF_FAILED true

void drawBmp(const char *filename, int16_t x, int16_t y);
void readFile(fs::FS &fs, const char *path);
void deleteFile(fs::FS &fs, const char *path);
uint16_t read16(fs::File &f);
uint32_t read32(fs::File &f);
void getImg(const char *link, const char *fname);
void homescreen(void);

const char *ssid = "OnePlus95g";   // your network SSID (name of wifi network)
const char *password = "password"; // your network password

const char *link1 = "https://sindhbad.experiences.digital//public/images/Test_rotated.bmp";
const char *link2 = "https://sindhbad.experiences.digital//public/images/Test_03.bmp";
const char *loc1 = "/img1.bmp";
const char *loc2 = "/img2.bmp";

TFT_eSPI tft = TFT_eSPI();

int LED_inbuilt = 2;

bool downloadFlag = 0;

void setup()
{
  Serial.end();
  delay(100);

  Serial.begin(115200);

  pinMode(LED_inbuilt, OUTPUT);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);

  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());

  digitalWrite(LED_inbuilt, HIGH);
  delay(500);
  digitalWrite(LED_inbuilt, LOW);
  delay(500);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");

    homescreen();
  }

  Serial.println("Connected to the WiFi network");

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // deleteFile(SPIFFS, "/img.bmp");

  getImg(link1, loc1);
  delay(1000);
  getImg(link2, loc2);

  downloadFlag = 1;
  
}

void loop()
{
  drawBmp(loc1, 0, 0);
  delay(6000);

  drawBmp(loc2, 0, 0);
  delay(6000);
}

void getImg(const char *link, const char *fname)
{
  File file = SPIFFS.open(fname, "w");

  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }

  if ((WiFi.status() == WL_CONNECTED))
  { // Check the current connection status
    HTTPClient http;
    http.begin(link);          // Specify the URL and certificate
    int httpCode = http.GET(); // Make the request

    if (httpCode == HTTP_CODE_OK)
    { // Check for the returning code
      http.writeToStream(&file);
    }

    file.close();
    Serial.println("you have finished downloading");
    http.end(); // Free the resources
  }

  delay(1000);
}

void drawBmp(const char *filename, int16_t x, int16_t y)
{

  if ((x >= tft.width()) || (y >= tft.height()))
  {
    Serial.println("Image Size is more than the screen resolution");
    return;
  }

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++)
      {

        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t *bptr = lineBuffer;
        uint16_t *tptr = (uint16_t *)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in ");
      Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else
      Serial.println("BMP format not recognized.");
  }
  else
  {
    Serial.println("Invalid BMP header");
  }
  bmpFS.close();
}

uint16_t read16(fs::File &f)
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f)
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");

  while (file.available())
  {
    Serial.write(file.read());
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("- file deleted");
  }
  else
  {
    Serial.println("- delete failed");
  }
}

void homescreen(void)
{
   // Now initialise the TFT
  tft.begin();
  tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLUE);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.println("Experiences");
  tft.println("Digital");
  tft.setTextSize(2);
  tft.println("Downloading");
  tft.println("Content");

}