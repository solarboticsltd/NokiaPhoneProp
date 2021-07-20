#ifndef __WEBPORTAL_H__
#define __WEBPORTAL_H__

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

void postWebValues(AsyncWebServerRequest*, JsonVariant&);
void getWebValues(AsyncWebServerRequest*);
void putCall(AsyncWebServerRequest*);

#endif
