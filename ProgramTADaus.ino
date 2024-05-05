#include "PzemInterface.h"
#include "PLCInterface.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x26, 16, 2);

PzemInterface pzem;
PLCInterface plc;

unsigned int saldoToken = 200000;

PzemData_t data;
float currentSaldo;

typedef enum {
  AMAN,
  HAMPIR_HABIS,
  HAMPIR_HABIS_SEKALI,
  HABIS
} KondisiSaldo;

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Manajemen");
  lcd.setCursor(0, 1);
  lcd.print("Pembatas Energi");
  delay(1000);

  pzem.begin();
  plc.begin();

  currentSaldo = saldoToken / 1444;
}

void loop() {
  // put your main code here, to run repeatedly:

  if (currentSaldo < 0) currentSaldo = 0;

  static unsigned long prevMillPzem;
  if (millis() - prevMillPzem >= 500U) {
    prevMillPzem = millis();
    readDisplay();
  }

  static unsigned long prevMillSaldo;
  if (millis() - prevMillSaldo >= 6000U) {
    prevMillSaldo = millis();
    currentSaldo = getSaldo();
  }

  KondisiSaldo kondiSaldo;
  if (currentSaldo > 5.0)
  {
    kondisiSaldo
  }

  plc.update();

  static int i;
  static unsigned long prevMillPlc;
  if (millis() - prevMillPlc >= 1000U) {
    prevMillPlc = millis();
    if (i <= 2) {
      plc.regs[0] = data.value[0 + i];
      i++;
    } else {
      i = 0;
    }
  }
}

float getSaldo() {
  return currentSaldo - (data.value[1] / 60);
}

void readDisplay() {
  pzem.readVoltage(0, data);
  // pzem.readPower(0, data);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("Daya: %.f W", data.value[1]);

  lcd.setCursor(0, 1);
  lcd.printf("Saldo: %.1f kWh", currentSaldo);
}
