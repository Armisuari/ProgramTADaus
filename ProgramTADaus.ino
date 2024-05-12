#include "PzemInterface.h"
#include "PLCInterface.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x26, 16, 2);

PzemInterface pzem;
PLCInterface plc;

unsigned int saldoToken = 6000;

PzemData_t data;
float currentSaldo;

typedef enum
{
  AMAN,
  HAMPIR_HABIS,
  HAMPIR_HABIS_SEKALI,
  HABIS
} KondisiSaldo;
KondisiSaldo kondisiSaldo;

void setup()
{
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

  currentSaldo = float(saldoToken) / 1444.0 * 1000; // wh unit
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (currentSaldo < 0.0)
    currentSaldo = 0;

  static unsigned long prevMillPzem;
  if (millis() - prevMillPzem >= 3000UL)
  {
    prevMillPzem = millis();

    static bool _switch;
    if (!_switch)
    {
      readDisplay();
      _switch = true;
    }
    else
    {
      readDisplayAlt();
      _switch = !_switch;
    }
  }

  static unsigned long prevMillSaldo;
  if (millis() - prevMillSaldo >= 60000U)
  {
    prevMillSaldo = millis();
    currentSaldo = getSaldo();
  }

  // Check and update saldo condition
  kondisiSaldo = updateSaldoCondition(currentSaldo);
  lcd.setCursor(15, 0);
  lcd.print(kondisiSaldo);

  static int i;
  static unsigned long prevMillPlc;
  if (millis() - prevMillPlc >= 1000U)
  {
    prevMillPlc = millis();
    plc.update(0, 220);
    plc.update(1, 20);
    plc.update(2, kondisiSaldo);

    //   // static bool state;
    //   // if (!state)
    //   // {
    //   //   state = true;
    //   // }
    //   // else
    //   // {
    //   //   state = false;
    //   // }

    //   // plc.update_coil(0, state);
    //   // Serial.printf("state: %d\n", state);
  }

  float getSaldo()
  {
    return currentSaldo - (data.value[1] / 60);
  }

  void readDisplay()
  {
    static float previousPower;
    static KondisiSaldo previousSaldo;

    // Read voltage and power
    pzem.readVoltage(0, data);
    pzem.readPower(0, data);
    // data.value[1] = 1000;

    if (currentSaldo <= 0.0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Token Listrik ");
      lcd.setCursor(0, 1);
      lcd.print("     Habis     ");

      // Serial.println("Token Listrik Habis !!!");
    }
    else
    {

      // Check if the values have changed before updating the LCD
      if (data.value[1] != previousPower)
      {
        // Update previous values
        previousPower = data.value[1];

        // Clear the LCD and update the display
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.printf("Daya: %.1f W", previousPower);

        lcd.setCursor(0, 1);
        if (currentSaldo / 1000 < 0.001)
        {
          lcd.printf("Saldo: %.3f Wh", currentSaldo);
        }
        else
        {
          lcd.printf("Saldo: %.3f kWh", currentSaldo / 1000);
        }

        // Serial.printf("saldo: %.9f kWh | daya: %.3f W\n", currentSaldo / 1000, previousPower);
      }
    }
  }

  void readDisplayAlt()
  {
    // static float previousVoltage;
    // static float previousCurrent;
    // static float previousPF;

    // if (data.value[0] != previousVoltage || data.value[2] != previousCurrent || data.value[4] != previousPF) {
    //   previousVoltage = data.value[0];
    //   previousCurrent = data.value[2];
    //   previousPF = data.value[4];

    pzem.readVoltage(0, data);
    pzem.readCurrent(0, data);
    pzem.readPF(0, data);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("v:%.f  pf:%.2f", data.value[0], data.value[4]);
    // lcd.printf("v: %.1f  i: %.1f", previousVoltage, previousCurrent);

    lcd.setCursor(0, 1);
    lcd.printf("i:%.2f", data.value[2]);
    // lcd.printf("pf: %.1f", previousPF);
    // }
  }

  // Function to update saldo condition based on currentSaldo value
  KondisiSaldo updateSaldoCondition(float saldo)
  {
    if (saldo >= 5000)
    {
      return AMAN;
    }
    else if (saldo > 2000)
    {
      return HAMPIR_HABIS;
    }
    else if (saldo > 0)
    {
      return HAMPIR_HABIS_SEKALI;
    }
    else
    {
      return HABIS;
    }
  }
