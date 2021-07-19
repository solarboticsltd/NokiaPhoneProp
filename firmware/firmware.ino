//******************************************************************************//
//************************** Nokia Phone Firmware ******************************//
//******************************************************************************//
//Arduino Library - standard
#include <Arduino.h>

//Webserver Libraries
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

//Data Storage Library
#include <EEPROM.h>

//LED Control Library
//#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


//Define the Webserver, DNS, and UDP Objects
AsyncWebServer server(80);
DNSServer dns;


//Define the Phone LED count, data pin, LED brightness levels,
//and FastLED object.
#define NUM_LEDS 60
#define DATA_PIN 12
#define BRIGHT 255
#define DIM 64
CRGB leds[NUM_LEDS];


//Important Notes:
//From: https://github.com/jasoncoon/esp8266-fastled-webserver/issues/85
// I add #define FASTLED_INTERRUPT_RETRY_COUNT 0 before #include<FastLED.h> and change line 19
// https://github.com/FastLED/FastLED/blob/master/platforms/esp/8266/clockless_esp8266.h
// from:
// template <int DATA_PIN, int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int XTRA0 = 0, bool FLIP = false, int WAIT_TIME = 50>
// to:
// template <int DATA_PIN, int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int XTRA0 = 0, bool FLIP = false, int WAIT_TIME = 5>
// Then the problem solved...



//If a GET request is made to a page that does not exist on the webserver, return a 404 error.
void notFound(AsyncWebServerRequest *request) {                                
  request->send(404, "text/plain", "Not found");                                
}   



//Arduino Setup Function
void setup() {
  // Start Serial Communication. (for debug logging only)
  Serial.begin(115200);



  //Initialize the FastLED object that will handle the phone LEDs               
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  //Set LED brightness to "Bright".
  FastLED.setBrightness(BRIGHT);
  //Turn off all of the LEDs.
  ledsOff();



  //Initialize the eeprom and read the phone values from memory
  //Begin the eeprom communication with an allocation of 512 bytes.
  EEPROM.begin(512);
  //loadConfigFromEEPROM();



  //start the phone is AP mode as we will not connect it to a local wifi network
  WiFi.softAP("Nokia Prop Phone", "solarbotics");
  Serial.println("WIFI: Fallback AP Mode Started");
  Serial.println("");
  //This assists with the jittering experienced when the web server interrupts the FastLed data string. Why? Great question... This is what happens when I don't save my links to stack overflow...
  WiFi.setSleepMode(WIFI_NONE_SLEEP);



  //When landing on the ip address, send index.html to client.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
	request->send_P(200, "text/html", "INDEX_HTML");                  ///!!!!! CHANGE THIS TO THE PROGMEM STRING LATER instead of "INDEX_HTML"

  });

  //Now that setup is complete, start the web server.
  server.onNotFound(notFound);
  server.begin();
}




//Main loop
void loop() {


}



//LED Control Functions:
//These functions are a quick and easy way to turn off/on all the LEDs.
void ledsOff() {
  FastLED.clear(true);
}
void allOn() {
  //FastLED.setBrightness(DIM);
  for (int i = 0; i < NUM_LEDS; i++) {
    yield();
    leds[i] = CRGB::White;
  }
  FastLED.show();
}
