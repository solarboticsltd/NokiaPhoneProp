#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>

extern const String DEFAULT_PHONE_NUMBER;
extern const String DEFAULT_CALLER_ID;

//A struct that holds the variables we will save to
//eeprom when the user updates them through the website
struct config_t
{
  unsigned long checksum;
  int vibration = 1;
  int contrast = 50;
  int ringCount = 8;
  int batLevel = 4;
  int sigLevel = 4;
  int hours = 17;
  int minutes = 56;
  char phoneNumber[16];
  char callID[16];
}; 

extern struct config_t configuration;

//Define a flag for when config data should be updated in EEPROM
extern bool doConfigUpdate;

extern bool doCall;

void saveConfigToEEPROM();
void loadConfigFromEEPROM();
void factoryResetEEPROM();

#endif
