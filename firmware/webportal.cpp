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
    configuration.contrast = json["ct"].as<int>();
  }

  if(json.containsKey("rc")){
    configuration.ringCount = json["rc"].as<int>();
  }

  if(json.containsKey("bl")){
    configuration.batLevel = json["bl"].as<int>();
  }

  if(json.containsKey("sl")){
    configuration.sigLevel = json["sl"].as<int>();
  }

  if(json.containsKey("th")){
      configuration.hours = json["th"].as<int>();
  }

  if(json.containsKey("tm")){
    String tempMinute = "0";
    
    if(json["tm"].as<String>().length() < 2)
    {
      tempMinute += json["tm"].as<String>();
    }
    else
    {
      tempMinute = json["tm"].as<String>();
    }
    configuration.minutes = tempMinute.toInt();
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
  Serial.println("Config Values");
  Serial.println("Contrast: " + String(configuration.contrast));
  Serial.println("Ring Count: " + String(configuration.ringCount));
  Serial.println("Bat Level: " + String(configuration.batLevel));
  Serial.println("Sig Level: " + String(configuration.sigLevel));
  Serial.println("Hours " + String(configuration.hours));
  Serial.println("Minutes: " + String(configuration.minutes));
  Serial.println("Phone Number: " + String(configuration.phoneNumber));
  Serial.println("Caller ID: " + String(configuration.callID));
    
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
