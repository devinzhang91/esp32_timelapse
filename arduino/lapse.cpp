#include "Arduino.h"
#include "camera.h"
#include <stdio.h>
#include "file.h"
#include "config.h"

unsigned long fileIndex = 0;
unsigned long lapseIndex = 0;
unsigned long frameInterval = 1000;
bool mjpeg = true;
bool lapseRunning = false;
bool enableFlashLight = false;
unsigned long lastFrameDelta = 0;


void setEnableFlashLight(bool enable){
  enableFlashLight = enable;
}

void setInterval(unsigned long delta)
{
    frameInterval = delta;
}

bool startLapse()
{
    if(lapseRunning) return true;
    fileIndex = 0;
    char path[32];
    for(; lapseIndex < 10000; lapseIndex++)
    {
        sprintf(path, "/lapse%03d", lapseIndex);
        if (!fileExists(path))
        {
            createDir(path);
            lastFrameDelta = 0;
            lapseRunning = true;
            return true;
        }
    }
	return false;
}

bool stopLapse()
{
    lapseRunning = false;
}

bool processLapse(unsigned long dt)
{
    if(!lapseRunning) return false;

    lastFrameDelta += dt;
    if(lastFrameDelta >= frameInterval)
    {
#if USE_SD
      if(enableFlashLight){
        digitalWrite(FLASH_LIGHT_GPIO_NUM, HIGH);
        delay(200);
      }
#endif
        lastFrameDelta -= frameInterval;
        camera_fb_t *fb = NULL;
        esp_err_t res = ESP_OK;
        fb = esp_camera_fb_get();
        if (!fb)
        {
	        Serial.println("Camera capture failed");
	        return false;
        }

        char path[32];
        sprintf(path, "/lapse%03d/pic%05d.jpg", lapseIndex, fileIndex);
        Serial.printf("fb:%d x %d len: %d ", fb->height, fb->width, fb->len);
        Serial.println(path);
        if(!writeFile(path, (const unsigned char *)fb->buf, fb->len))
        {
            lapseRunning = false;
            return false;
        }
        fileIndex++;
        esp_camera_fb_return(fb);
        
#if USE_SD
        digitalWrite(FLASH_LIGHT_GPIO_NUM, LOW);
#endif
    }
    return true;
}
