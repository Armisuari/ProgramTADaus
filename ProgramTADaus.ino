// #define TEST // matikan untuk jalankan program utama

#ifdef TEST
#include "main_200524.h";
#else

#include "Application.h"

Application app;

void setup() {
  // Serial.begin(115200);
  app.init();
}

void loop() {

  // update pzem data and displayed value
  static unsigned long prevMillMeter;
  if (millis() - prevMillMeter >= 500UL)
  {
    prevMillMeter = millis();
    app.updateMeterData();
  }

  static unsigned long prevMillPlc;
  if (millis() - prevMillPlc >= 1000UL)
  {
    prevMillPlc = millis();
    app.sendToPLC();
  }

  // update saldo token
  app.updateSaldo();
}

#endif