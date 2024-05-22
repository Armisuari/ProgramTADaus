#include "PzemInterface.h"
#include "PLCInterface.h"
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x26, 16, 2);

PzemInterface pzem;
PLCInterface plc;

unsigned int saldoToken = 100000;
unsigned int targetToken = 500;

PzemData_t data[2];
float currentSaldo[2];
float energyTarget[2];
unsigned long lastResetTime;

unsigned int voltage[2];
float power[2];
float energy[2];

typedef enum {
  AMAN,
  HAMPIR_HABIS,
  HAMPIR_HABIS_SEKALI,
  HABIS
} KondisiSaldo;
KondisiSaldo kondisiSaldo[2];

typedef enum {
  NORMAL,
  BOROS
} Penggunaan;
Penggunaan penggunaan[2];

float getSaldo();
void readDisplay();
void readDisplayAlt();
KondisiSaldo updateSaldoCondition(float saldo);
void resetEnergy();
void switchDisplay();

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Manajemen");
  lcd.setCursor(0, 1);
  lcd.print("Pembatas Energi");
  delay(2000);

  pzem.begin();
  plc.begin();

  currentSaldo[0] = float(saldoToken) / 1444.0 * 1000;  // wh unit
  energyTarget[0] = float(targetToken) / 1444.0 / 30;   // batasan energei perhari

  // Read the last reset time from EEPROM
  EEPROM.get(0, lastResetTime);

  // Check if the stored time is valid
  if (lastResetTime == 0xFFFFFFFF) {
    lastResetTime = millis();  // If not valid, initialize with the current time
    EEPROM.put(0, lastResetTime);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  // reset energy every 24 hour to sync the energy calculation
  resetEnergy();

  for (int i = 0; i < 2; ++i) {
    voltage[i] = int(data[0].value[0]);
    power[i] = data[0].value[1];
    energy[i] = data[0].value[3];
  }

  if (currentSaldo[0] < 0.0)
    currentSaldo[0] = 0;

  static unsigned long prevMillPzem;
  if (millis() - prevMillPzem >= 3000UL) {
    prevMillPzem = millis();
    switchDisplay();
  }

  static unsigned long prevMillSaldo;
  if (millis() - prevMillSaldo >= 60000U) {
    prevMillSaldo = millis();
    currentSaldo[0] = getSaldo();
  }

  // Check and update saldo condition
  kondisiSaldo[0] = updateSaldoCondition(currentSaldo[0]);
  lcd.setCursor(14, 0);
  lcd.print(kondisiSaldo[0]);

  if (energy[0] >= energyTarget[0]) {
    penggunaan[0] = BOROS;
  } else {
    penggunaan[0] = NORMAL;
  }

  static int i;
  static unsigned long prevMillPlc;
  if (millis() - prevMillPlc >= 1000U) {
    prevMillPlc = millis();
    plc.update(0, voltage[0]);
    plc.update(1, power[0]);
    plc.update(2, kondisiSaldo[0]);
    plc.update(3, penggunaan[0]);
  }
}

float getSaldo() {
  return currentSaldo[0] - (data[0].value[1] / 60);
}

void readDisplay() {
  static float previousPower;
  static KondisiSaldo previousSaldo;

  // Read voltage and power
  pzem.readVoltage(0, data[0]);
  pzem.readPower(0, data[0]);
  pzem.readEnergy(0, data[0]);
  // data.value[1] = 1000;

  if (data[0].value[1] == NAN) data[0].value[1] = 0.0;

  if (currentSaldo[0] <= 0.0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Token Listrik ");
    lcd.setCursor(0, 1);
    lcd.print("     Habis     ");

    // Serial.println("Token Listrik Habis !!!");
  } else {

    // Check if the values have changed before updating the LCD
    if (data[0].value[1] != previousPower) {
      // Update previous values
      previousPower = data[0].value[1];

      // Clear the LCD and update the display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.printf("Daya: %.1f W", previousPower);

      lcd.setCursor(0, 1);
      if (currentSaldo[0] / 1000 < 0.001) {
        lcd.printf("Saldo: %.1f Wh", currentSaldo[0]);
      } else {
        lcd.printf("Saldo: %.2f kWh", currentSaldo[0] / 1000);
      }

      // Serial.printf("saldo: %.9f kWh | daya: %.3f W\n", currentSaldo / 1000, previousPower);
    }
  }
}

void readDisplayAlt() {
  // static float previousVoltage;
  // static float previousCurrent;
  // static float previousPF;

  // if (data.value[0] != previousVoltage || data.value[2] != previousCurrent || data.value[4] != previousPF) {
  //   previousVoltage = data.value[0];
  //   previousCurrent = data.value[2];
  //   previousPF = data.value[4];

  pzem.readVoltage(0, data[0]);
  pzem.readCurrent(0, data[0]);
  pzem.readPF(0, data[0]);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("v:%.f  pf:%.2f", data[0].value[0], data[0].value[4]);
  // lcd.printf("v: %.1f  i: %.1f", previousVoltage, previousCurrent);

  lcd.setCursor(0, 1);
  lcd.printf("i:%.2f e:%.2f  %d", data[0].value[2], data[0].value[3], penggunaan[0]);
  // lcd.printf("pf: %.1f", previousPF);
  // }
}

// Function to update saldo condition based on currentSaldo value
KondisiSaldo updateSaldoCondition(float saldo) {
  if (saldo >= 5000) {
    return AMAN;
  } else if (saldo > 2000) {
    return HAMPIR_HABIS;
  } else if (saldo > 0) {
    return HAMPIR_HABIS_SEKALI;
  } else {
    return HABIS;
  }
}

void switchDisplay() {
  static bool _switch;
  if (!_switch) {
    readDisplay();
    _switch = true;
  } else {
    readDisplayAlt();
    _switch = !_switch;
  }
}

void resetEnergy() {
  // Calculate elapsed time considering overflow
  unsigned long elapsedTime = millis() - lastResetTime;
  if (elapsedTime < 0) {
    elapsedTime = (0xFFFFFFFF - lastResetTime) + millis();
  }

  if (elapsedTime >= 86400000) {  // 24 jam
                                  // if (elapsedTime >= 60000) { // 1 menit
    lastResetTime = millis();
    EEPROM.put(0, lastResetTime);  // Save the reset time to EEPROM
    // Serial.println("Resetting energy meter...");
    pzem.reset(0);
  }
}
