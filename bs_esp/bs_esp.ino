#include <Arduino.h>
#include "prototypes.h"
#include "all.h"

std::vector<void (*)()> _loops;
void loopRegister(void (*callback)()) {
  _loops.push_back(callback);
}

void setup() {
  // put your setup code here, to run once:
  delay(INITIAL_DELAY);
  #if not USE_UART_PINS_AS_GPIO
  Serial.begin(SERIAL_BAUDRATE);
  #endif
  eepromSetup();
  settingsSetup();
  // wifiSetup();
  wifi2Setup();
  httpSetup(); // for getting the wifi credentials and FOTA update
  httpClientSetup();  // for getting device id, config, settings etc.
  mqtt2Setup();
  relaySetup();
  buttonSetup();

  #if ENABLE_PING_MSG_TO_SERVER
  pingSetup();
  #endif
  
  #if POWER_PROVIDER != POWER_PROVIDER_NONE
  powerSetup();
  #endif

  #if LIGHT_PROVIDER != LIGHT_PROVIDER_NONE
  lightSetup();
  #endif

  #if ELEGANT_OTA_SUPPORT
  elegantotaSetup();
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long start = millis();
  for (unsigned char i = 0; i < _loops.size(); i++) {
      (_loops[i])();
  }
}