#ifndef __VERIFICATION_H__
#define __VERIFICATION_H__

#include <ArduinoJson.h>

template<typename T> bool verifyValue(JsonVariant &json, String field, T *output)
{
  if(!json[field].is<T>())
  {
    return false;
  }

  *output = json[field];
  return true;
}

template<typename T> bool verifyValue(JsonVariant &json, String field, T *output, T lowBound, T highBound)
{
  if(!json[field].is<T>())
  {
    return false;
  }
  if(json[field].as<T>() < lowBound || json[field].as<T>() > highBound)
  {
    return false;
  }

  *output = json[field];
  return true;
}

#endif
