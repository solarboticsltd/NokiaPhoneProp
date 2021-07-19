#include <ArduinoJson.h>

#include "rest.h"

//If a GET request is made to a page that does not exist on the webserver,return a 404 error.
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

//Send an http error code and message contained in a JSON payload to the request 
void httpError(int code, AsyncWebServerRequest *request, String message) {

  //Create a json object and specify it's capacity
  const int capacity = 4 * JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  JsonObject error = doc.to<JsonObject>();

  error["error"] = message;

  //Serialize the message into a JSON object
  String output;
  serializeJson(error, output);

  request->send(code, "application/json", output);
}
