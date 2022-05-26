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
#define DATA_PIN D0
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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" type "text/css" href=style.css>
  <title>Shadow Clock Configuration</title>
</head>
<body style="margin:0" style="padding:0">
  <table cellpadding="3" cellspacing="0" align="center">
    <colgroup>
      <col style="width:50%">
    </colgroup>
<!--Form for data input of clock configuration values -->
    <form name="forms" id="forms">
      <tr>
        <th colspan="2">
          <h2>Nokia 3310 Configuration</h2>
        </th>
      </tr>
      <tr>
        <th colspan="2">
          <h3>Settings:</h3>
        <th>
      </tr>
      <tr>
        <th colspan="2">
          <h4>Adjust these settings and then press the submit button to send them to the phone.</h4>
        </th>
      </tr>
      <tr>
        <td>Vibration:</td>
        <td style="text-align:center">
          <select id="vb" name="vb">
            <option value="0">0</option>
            <option value="1" selected>1</option>
          </select>
        </td>
      </tr>
        <td>Ring Count:</td>
        <td style="text-align:center">
          <select id="rc" name="rc">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3" selected>3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
            <option value="9">9</option>
            <option value="10">10</option>
            <option value="11">11</option>
            <option value="12">12</option>
            <option value="13">13</option>
            <option value="14">14</option>
            <option value="15">15</option>
            <option value="16">16</option>
            <option value="17">17</option>
            <option value="18">18</option>
            <option value="19">19</option>
            <option value="20">20</option>
          </select>
        </td>
      </tr>
      <tr>
        <td>Battery Level:</td>
        <td style="text-align:center">
          <select id="bl" name="bl">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2" selected>2</option>
            <option value="3">3</option>
            <option value="4">4</option>
          </select>
        </td>
      </tr>
      <tr>
        <td>Signal Level:</td>
        <td style="text-align:center">
          <select id="sl" name="sl">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3" selected>3</option>
            <option value="4">4</option>
          </select>
        </td>
      </tr>
      <tr>
        <td>Hours:</td>
        <td style="text-align:center">
          <select id="th" name="th">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3">3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
            <option value="9">9</option>
            <option value="10" selected>10</option>
            <option value="11">11</option>
            <option value="12">12</option>
            <option value="13">13</option>
            <option value="14">14</option>
            <option value="15">15</option>
            <option value="16">16</option>
            <option value="17">17</option>
            <option value="18">18</option>
            <option value="19">19</option>
            <option value="20">20</option>
            <option value="21">21</option>
            <option value="22">22</option>
            <option value="23">23</option>
          </select>
        </td>
      </tr>
      <tr>
        <td>Minutes:</td>
        <td style="text-align:center">
          <select id="tm" name="tm">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3">3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
            <option value="9" selected>9</option>
            <option value="10">10</option>
            <option value="11">11</option>
            <option value="12">12</option>
            <option value="13">13</option>
            <option value="14">14</option>
            <option value="15">15</option>
            <option value="16">16</option>
            <option value="17">17</option>
            <option value="18">18</option>
            <option value="19">19</option>
            <option value="20">20</option>
            <option value="21">21</option>
            <option value="22">22</option>
            <option value="23">23</option>
            <option value="24">24</option>
            <option value="25">25</option>
            <option value="26">26</option>
            <option value="27">27</option>
            <option value="28">28</option>
            <option value="29">29</option>
            <option value="30">30</option>
            <option value="31">31</option>
            <option value="32">32</option>
            <option value="33">33</option>
            <option value="34">34</option>
            <option value="35">35</option>
            <option value="36">36</option>
            <option value="37">37</option>
            <option value="38">38</option>
            <option value="39">39</option>
            <option value="40">40</option>
            <option value="41">41</option>
            <option value="42">42</option>
            <option value="43">43</option>
            <option value="44">44</option>
            <option value="45">45</option>
            <option value="46">46</option>
            <option value="47">47</option>
            <option value="48">48</option>
            <option value="49">49</option>
            <option value="50">50</option>
            <option value="51">51</option>
            <option value="52">52</option>
            <option value="53">53</option>
            <option value="54">54</option>
            <option value="55">55</option>
            <option value="56">56</option>
            <option value="57">57</option>
            <option value="58">58</option>
            <option value="59">59</option>
          </select>
        </td>
      </tr>
      <tr>
        <td>Phone Number:</td>
        <td style="text-align:center"><input type="text" id="pn" name="pn" style="width:126px"></td>
      </tr>
      <tr>
        <td>Caller ID:</td>
        <td style="text-align:center"><input type="text" id="ci" name="ci" style="width:126px"></td>
      </tr>
      <tr>
        <td>Lights:</td>
        <td style="text-align:center">
          <select id="lt" name="lt">
            <option value="0">0</option>
            <option value="1">1</option>
          </select>
        </td>
      </tr>
      <tr>
        <td style="text-align:center" colspan="2"><button class="mybutton" type="submit">Submit</button></td>
      </tr>
    </form>
    <tr>
      <th colspan="2">
        <h3>Make Call:</h3>
      <th>
    </tr>
    <tr>
      <th colspan="2">
        <h4>Press this button to have the Nokia 3310 make a "call"</h4>
      </th>
    </tr>
    <tr>
      <td colspan="2" style="text-align:center"><input type="button" value="Make Call" id="callButton" style="width:150px" onclick="makeCall()"></td>
    </tr>
  </table>

  <script>
    // get the form element from dom
    const formElement = document.querySelector('form#forms');

    // convert the form to JSON
    const getFormJSON = (form) => {
      const data = new FormData(form);
      return Array.from(data.keys()).reduce((result, key) => {
        result[key] = data.get(key);
        return result;
      }, {});
    };

    // handle the form submission event, prevent default form behaviour, check validity, convert form to JSON
    const handler = (event) => {
      event.preventDefault();
      const valid = formElement.reportValidity();
      if (valid) {
        const result = getFormJSON(formElement);
        var request = new XMLHttpRequest();
        request.open("POST", "/config", true);
        request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        request.send(JSON.stringify(result));
      }
    }
    function makeCall(){
      request = new XMLHttpRequest();
      if (request) {
        request.open("PUT", "/call", true);
        request.send();
      }
    }


    formElement.addEventListener("submit", handler);
  </script>
</body>
</html>

)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
th, td{
  text-align:left;
  color:white;
}

h2{
  text-align:center;
  margin-top:15px;
  margin-bottom:2px;
  color:#FECE35;
}

h3{
  margin-top:30px;
  margin-bottom:2px;
  color:#FECE35;
}

h4{
  margin-top:2px;
  margin-bottom:10px;
  color:#8B8B8B;
}

input{
  margin-top:1px;
  margin-bottom:2px;
  margin-left:0px;
  align:left;
  width:126px;
}

select{
  margin-top:1px;
  margin-bottom:2px;
  margin-left:0px;
  align:left;
  width:130px;
}

table{
  max-width: 400px;
  border-radius: 25px;
  background-color:#3a3a3a;
  border: 20px solid#3a3a3a;
}

button{
  margin-top:10px;
  margin-bottom:2px;
  width:150px;
  align:center;
  text-align:center;
}
)rawliteral";

//Arduino Setup Function
void setup() {
  // Start Serial Communication. (for debug logging only)
  Serial.begin(115200);

  display.begin();
  display.setContrast(50);
  display.setFont(&nokiafc224pt7b);
  display.clearDisplay();   // clears the screen and buffer

  //Output pins for the Vibration and Backlight controls
  pinMode(D3, OUTPUT);//Lights
  pinMode(D4, OUTPUT);//Vibration

  digitalWrite(D3, HIGH); //Lights
  digitalWrite(D4, LOW); //Vibration

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


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", style_css);
  });

  //Config Web Handlers. Refer to webportal.cpp for function details.
  server.on("/config", HTTP_GET, getWebValues);
  server.on("/call", HTTP_PUT, putCall);

  AsyncCallbackJsonWebHandler* configPOSTHandler = new AsyncCallbackJsonWebHandler("/config", postWebValues, 128 * 2);
  configPOSTHandler->setMethod(HTTP_POST);
  configPOSTHandler->setMaxContentLength(128 * 2);
  server.addHandler(configPOSTHandler);
  
  //Now that setup is complete, start the web server.
  server.onNotFound(notFound);
  server.begin();

  drawMenu();
}



//Main loop
void loop() {

  if(doConfigUpdate)
  {
    saveConfigToEEPROM();
    drawConfig();
    delay(2000);
    drawMenu();
    doConfigUpdate = false;
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
  String tempHours = String(hours);
  String tempMinutes = String(minutes);

  if(hours < 10)
  {
    tempHours = "0" + String(hours);
  }
  if(minutes < 10)
  {
    tempMinutes = "0" + String(minutes);
  }
  String displayTime = tempHours + ":" + tempMinutes;

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
  int vibeTime[] = {600, 200, 600, 1000};
  int ringTimer = millis();
  int vibeTimer = millis();
  int vibeCounter = 0;
  bool vibeFlag = true;
  bool ringFlag = true;
  int callTime = millis();
 
  while((millis() - callTime) < ((vibeTime[0]+vibeTime[1]+vibeTime[2]+vibeTime[3]) * configuration.ringCount))
  {
    Serial.println("Ringing");
    
    if(millis() - ringTimer > 400)
    {
      Serial.println("Flipping Ring Flag");
      ringTimer = millis();
      ringFlag = !ringFlag;
    }

    if(millis() - vibeTimer > vibeTime[vibeCounter])
    {
      Serial.println("Flipping Vibe Flag");
      vibeTimer = millis();
      vibeCounter++;
      if(vibeCounter > 3)
      {
        vibeCounter = 0;
      }
      vibeFlag = !vibeFlag;
    }
    
    if(ringFlag == true)
    {
      display.clearDisplay();
      drawBattery(configuration.batLevel);
      drawSignal(configuration.sigLevel);
      drawTime(configuration.hours, configuration.minutes, 50, 7);
      drawText(String(configuration.callID), 6, 31);
      drawText("Answer", 25, 45);
      drawText(String(configuration.phoneNumber), 8, 15);

      if(configuration.lights == 1)
      {
        digitalWrite(D3, LOW);
      }
    }
    else
    {
      display.clearDisplay();
      drawBattery(configuration.batLevel);
      drawSignal(configuration.sigLevel);
      drawTime(configuration.hours, configuration.minutes, 50, 7);
      drawText(String(configuration.callID), 6, 31);
      drawText("Answer", 25, 45);

      digitalWrite(D3, HIGH);
    }

    if(vibeFlag == true && configuration.vibration == 1)
    {
      digitalWrite(D4, HIGH);
    }
    else
    {
      digitalWrite(D4, LOW);
    }

    
    display.display();
    delay(100);
  }

  digitalWrite(D4, LOW);
  digitalWrite(D3, HIGH);
  drawMenu();
}

void drawConfig()
{
  display.clearDisplay();
  drawText("Vibration: " + String(configuration.vibration), 0, 6);
  drawText("Ring Count: " + String(configuration.ringCount), 0, 14);
  drawText("BatLevel: " + String(configuration.batLevel), 0, 22);
  drawText("SigLevel: " + String(configuration.sigLevel), 0, 30);
  drawText("Time: " + String(configuration.hours)+":"+String(configuration.minutes), 0, 38);
  drawText("PN: " + String(configuration.phoneNumber), 0, 46);
  display.display();
  delay(2000);
  
  display.clearDisplay();
  drawText("ID: " + String(configuration.callID), 0, 6);
  drawText("Lights: " + String(configuration.lights), 0, 14);
  display.display();
  delay(2000);
  
  drawMenu();
}