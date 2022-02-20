#include <WiFi.h>
#include <ESPmDNS.h>
#include "config.h"
#include "file.h"
#include "camera.h"
#include "lapse.h"

#if STA_MODE
const char* ssid = "Mushroom home";
const char* password = "19092430";
const char* host = "esp32cam";
#else
const char* ssid = "esp32 timelapse";
const char* password = "qwerty";
#endif


void startCameraServer();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(FLASH_LIGHT_GPIO_NUM, OUTPUT); //Light
  digitalWrite(FLASH_LIGHT_GPIO_NUM, LOW);
#if STA_MODE
  Serial.printf("Station Mode! SSID: %s \n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
    digitalWrite(FLASH_LIGHT_GPIO_NUM, HIGH);
    delay(10);
    digitalWrite(FLASH_LIGHT_GPIO_NUM, LOW);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  MDNS.begin(host);
  Serial.print("Open http://");
  Serial.print(host);
  Serial.println(".local");
#else
  Serial.printf("Soft AP Mode! SSID: %s \n", ssid);
  WiFi.softAP(ssid, password);
  digitalWrite(FLASH_LIGHT_GPIO_NUM, HIGH);
  delay(200);
  digitalWrite(FLASH_LIGHT_GPIO_NUM, LOW);
#endif

  startCameraServer();
#if STA_MODE
  Serial.print(WiFi.localIP());
  String WiFiAddr = WiFi.localIP().toString();
#else
  Serial.println(WiFi.softAPIP());
  String WiFiAddr = WiFi.softAPIP().toString();
#endif
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFiAddr);
  Serial.println("' to connect");

  initFileSystem();
  initCamera();
}

void loop()
{
  unsigned long t = millis();
  static unsigned long ot = 0;
  unsigned long dt = t - ot;
  ot = t;
  processLapse(dt);
//   esp_sleep_enable_timer_wakeup(1*1000);
//   esp_light_sleep_start();
}
