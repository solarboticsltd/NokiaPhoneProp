#include <EEPROM.h>

#include "config.h"

#define EEPROM_CONFIG_ADDRESS 0

const String DEFAULT_PHONE_NUMBER = "14032326268";
const String DEFAULT_CALLER_ID = "SolarboticsLtd";

struct config_t configuration;

unsigned long crc32(uint8_t *buf, size_t len) {
	const unsigned long crc_table[16] = {
		0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
		0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
		0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
	};

	unsigned long crc = ~0L;
	for (size_t index = 0 ; index < len  ; ++index) {
		crc = crc_table[(crc ^ (buf[index] >> 0)) & 0x0f] ^ (crc >> 4);
		crc = crc_table[(crc ^ (buf[index] >> 4)) & 0x0f] ^ (crc >> 4);
		crc = ~crc;
	}

	return crc;
}

//Save clock data to eeprom
void saveConfigToEEPROM() {
  // Update the configuration checksum
  configuration.checksum = 0UL;
  configuration.checksum = crc32((uint8_t*)&configuration, sizeof(config_t));

  EEPROM.put(EEPROM_CONFIG_ADDRESS, configuration);

  //Commit the date to eeprom (.put only sends it to the ram).
  EEPROM.commit();
}

void loadConfigFromEEPROM() {
  EEPROM.get(EEPROM_CONFIG_ADDRESS, configuration);

  unsigned long expected = configuration.checksum;

  configuration.checksum = 0UL;
  configuration.checksum = crc32((uint8_t*)&configuration, sizeof(config_t));

  // Reset the configuration if it looks corupt on uninitialized
  if(configuration.checksum != expected) {
    factoryResetEEPROM();
  }
}

// Reset the shadow clock config values to defaults and then update the eeprom
void factoryResetEEPROM(){

  Serial.println("FACTORY RESET: Resetting config and commiting to eeprom");
  
  configuration.page = 1;
  configuration.contrast = 50;
  configuration.ringCount = 8;
  configuration.batLevel = 4;
  configuration.sigLevel = 4;
  configuration.hours = 17;
  configuration.minutes = 56;
  DEFAULT_PHONE_NUMBER.toCharArray(configuration.phoneNumber, DEFAULT_PHONE_NUMBER.length()+1);
  DEFAULT_CALLER_ID.toCharArray(configuration.callID, DEFAULT_CALLER_ID.length()+1);

  saveConfigToEEPROM();
}
