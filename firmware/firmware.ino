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

//Include project files (split for readability)
#include "config.h"
#include "rest.h"
#include "webportal.h"

//LED Control Library
//#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Fonts/nokiafc224pt7b.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(D1, D2, D5, D6, D7);

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



//Arduino Setup Function
void setup() {
  // Start Serial Communication. (for debug logging only)
  Serial.begin(115200);

  display.begin();
  display.setContrast(50);
  display.setFont(&nokiafc224pt7b);
  display.clearDisplay();   // clears the screen and buffer

  //Initialize the FastLED object that will handle the phone LEDs               
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  //Set LED brightness to "Bright".
  FastLED.setBrightness(BRIGHT);
  //Turn off all of the LEDs.
  ledsOff();



  //Initialize the eeprom and read the phone values from memory
  //Begin the eeprom communication with an allocation of 512 bytes.
  EEPROM.begin(512);
  loadConfigFromEEPROM();



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

  if(doConfigUpdate)
  {
    saveConfigToEEPROM();
  }
  
  if(doCall)
  {
    drawCall();
    doCall = false;
  }

  delay(200);
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



void drawSignal(int lvl)
{
  
  //Draw the signal icon (could be a bitmap but meh, this is faster dev-wise)
  display.drawFastHLine(1,33,5,BLACK);
  display.drawFastHLine(2,35,3,BLACK);
  display.drawFastVLine(1,33,2,BLACK);
  display.drawFastVLine(3,33,6,BLACK);
  display.drawFastVLine(5,33,2,BLACK);

  //Depending on signal level, draw the signal bars
  if(lvl > 0)
  {
    display.fillRect(1,25,2,7,BLACK);
  }
  if(lvl > 1)
  {
    display.fillRect(1,17,2,7,BLACK);
  }
  if(lvl > 2)
  {
    display.fillRect(1,9,3,7,BLACK);
  }
  if(lvl > 3)
  {
    display.fillRect(1,1,4,7,BLACK);
  }  
}

void drawBattery(int lvl)
{
  
  //Draw the signal icon (could be a bitmap but meh, this is faster dev-wise)
  display.drawFastHLine(80,33,2,BLACK);
  display.drawFastHLine(79,34,4,BLACK);
  display.drawFastHLine(79,38,4,BLACK);
  display.drawFastVLine(79,34,5,BLACK);
  display.drawFastVLine(82,34,5,BLACK);


  //Depending on signal level, draw the signal bars
  if(lvl > 0)
  {
    display.fillRect(81,25,2,7,BLACK);
  }
  if(lvl > 1)
  {
    display.fillRect(81,17,2,7,BLACK);
  }
  if(lvl > 2)
  {
    display.fillRect(80,9,3,7,BLACK);
  }
  if(lvl > 3)
  {
    display.fillRect(79,1,4,7,BLACK);
  }  
}

void drawText(String text, int x, int y)
{
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(x,y);
  display.println(text);
}

void drawTime(int hours, int minutes, int x, int y)
{
  String displayTime = String(hours) + ":" + String(minutes);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(x,y);
  display.println(displayTime);
}


void drawMenu(){
  display.clearDisplay();
  drawBattery(configuration.batLevel);
  drawSignal(configuration.sigLevel);
  drawText("NOKIA",28,23);
  drawText("Menu",30,45); 
  drawTime(configuration.hours, configuration.minutes, 50, 7);
  display.display();
}

void drawCall(){
  
  for(int i = 1; i < (configuration.ringCount * 2 + 1); i++)
  {
    display.clearDisplay();
    drawBattery(configuration.batLevel);
    drawSignal(configuration.sigLevel);
    drawTime(configuration.hours, configuration.minutes, 50, 7);
    drawText(String(configuration.callID), 6, 31);
    drawText("Answer", 25, 45);
    
    if(i % 2 != 0)
    {
      drawText(String(configuration.phoneNumber), 12, 15);
      
    }
    
    display.display();
    delay(400);
  }
  
  drawMenu();
}

void drawConfig()
{
  display.clearDisplay();
  drawText("Contrast: " + String(configuration.contrast), 0, 6);
  drawText("Ring Count: " + String(configuration.ringCount), 0, 14);
  drawText("BatLevel: " + String(configuration.batLevel), 0, 22);
  drawText("SigLevel: " + String(configuration.sigLevel), 0, 30);
  drawText("Time: " + String(configuration.hours)+":"+String(configuration.minutes), 0, 38);
  drawText("PN: " + String(configuration.phoneNumber), 0, 46);
  display.display();
  delay(2000);
  
  display.clearDisplay();
  drawText("ID: " + String(configuration.callID), 0, 6);
  display.display();
  delay(2000);
  
  drawMenu();

}
