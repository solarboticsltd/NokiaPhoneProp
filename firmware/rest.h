#ifndef __REST_H__
#define __REST_H__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void notFound(AsyncWebServerRequest*);
void httpError(int, AsyncWebServerRequest*, String);

#endif
