#include "config.h"
#include "rest.h"
#include "verification.h"
#include "webportal.h"



//Send nokia settings to the webpage
void getWebValues(AsyncWebServerRequest *request)
{
  Serial.println("WEB: Config Data Requested");

  //Create a JSON Object for the response payload
  const int capacity = JSON_OBJECT_SIZE(8 * 2);
  StaticJsonDocument<capacity>doc;

  //Fill JSON object with config settings
  doc["ct"] = configuration.contrast;
  doc["rc"] = configuration.ringCount;
  doc["bl"] = configuration.batLevel;
  doc["sl"] = configuration.sigLevel;
  doc["th"] = configuration.hours;
  doc["tm"] = configuration.minutes;
  doc["pn"] = configuration.phoneNumber;
  doc["ci"] = configuration.callID;

  //Send JSON payload to the webpage
  String response;
  serializeJson(doc, response);
  request->send(200, "application/json", response);
}



//Recieve nokia settings from the webpage and store them in memory
void postWebValues(AsyncWebServerRequest *request, JsonVariant &json) {
  Serial.println("WEB: Web Data Sent");

  //If the correct keys are in the JSON object, validate their values and store them in the config struct
  if(json.containsKey("ct")){
    verifyValue<int>(json,"ct", &configuration.contrast, 0, 100);
  }

  if(json.containsKey("rc")){
    verifyValue<int>(json,"rc", &configuration.ringCount, 1, 20);
  }

  if(json.containsKey("bl")){
    verifyValue<int>(json,"bl", &configuration.batLevel, 0, 4);
  }

  if(json.containsKey("sl")){
    verifyValue<int>(json,"sl", &configuration.sigLevel, 0, 4);
  }

  if(json.containsKey("th")){
    verifyValue<int>(json,"th", &configuration.hours, 0, 23);
  }

  if(json.containsKey("tm")){
    verifyValue<int>(json,"tm", &configuration.minutes, 0, 59);
  }

  if(json.containsKey("pn")){
    //convery json object to String and then save as a char array
    String phNumber = json["pn"].as<String>();
    if(phNumber.length() <= 15)
    {
      phNumber.toCharArray(configuration.phoneNumber, phNumber.length()+1);
    }
  } 

  if(json.containsKey("ci")){
    //convery json object to String and then save as a char array
    String cID = json["ci"].as<String>();
    if(cID.length() <= 15)
    {
      cID.toCharArray(configuration.callID, cID.length()+1);
    }
  }   

  //Raise the update flag so the eeprom is updated with the config values
  doConfigUpdate = true;
  //Send the config values back to the webpage
  getWebValues(request);
}



//Recieve a demo request from website and verify JSON payload
void putCall(AsyncWebServerRequest *request) {
  Serial.println("WEB: Call Requested");

  doCall = true;
  
  request->send(200);
}
