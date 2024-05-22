#pragma once

#include <LiquidCrystal_I2C.h>

class DisplayInterface {
public:
  DisplayInterface(uint8_t lcd_Addr);
  bool begin();
  bool setPowerValue(float f);
  bool setVoltageValue(float v);
  bool setSaldoValue(float k);
  bool setCurrentValue(float i);
  bool setPfValue(float pf);
  bool setKondisiSaldo(int kondisiSaldo);
  bool setStatusUsage(int status);

private:
  void refreshDisplay0();
  void refreshDisplay1();

  int _kondisiSaldo;
  int _statusUsage;

  float _powerValue;
  float _voltageValue;
  float _saldoKwh;
  float _currentValue;
  float _pfValue;
  uint8_t _lcd_Addr;

  LiquidCrystal_I2C _lcd;
};

// Constructor implementation
DisplayInterface::DisplayInterface(uint8_t lcd_Addr)
  : _powerValue(0.0), _lcd_Addr(lcd_Addr), _lcd(lcd_Addr, 16, 2) {}

bool DisplayInterface::begin() {
  _lcd.init();
  _lcd.backlight();
  _lcd.setCursor(0, 0);
  _lcd.print("Sistem Manajemen");
  _lcd.setCursor(0, 1);
  _lcd.print("Pembatas Energi");
  delay(2000);
  return true;
}

void DisplayInterface::refreshDisplay0() {
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.printf("Daya: %.2f W", _powerValue);

  _lcd.setCursor(14, 0);
  _lcd.print(String(_kondisiSaldo) + String(_statusUsage));

  _lcd.setCursor(0, 1);
  if (_saldoKwh / 1000 < 0.1) {
    _lcd.printf("Saldo: %.2fWh", _saldoKwh);
  } else {
    _lcd.printf("Saldo: %.1fKWh", _saldoKwh / 1000);
  }
}

void DisplayInterface::refreshDisplay1() {
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.printf("V:%d     i:%.2f", int(_voltageValue), _currentValue);
  _lcd.setCursor(0, 1);
  _lcd.printf("    pf:%.2f", _pfValue);
}

bool DisplayInterface::setPowerValue(float f) {
  _powerValue = f;
  refreshDisplay0();
  return true;
}

bool DisplayInterface::setVoltageValue(float v) {
  _voltageValue = v;
  refreshDisplay1();
  return true;
}

bool DisplayInterface::setSaldoValue(float k) {
  _saldoKwh = k;
  // Serial.printf("[DisplayInterface] _saldoKwh = %.2f\n", _saldoKwh);
  refreshDisplay0();
  return true;
}

bool DisplayInterface::setCurrentValue(float i) {
  _currentValue = i;
  refreshDisplay1();
  return true;
}

bool DisplayInterface::setPfValue(float pf) {
  _pfValue = pf;
  refreshDisplay1();
  return true;
}

bool DisplayInterface::setKondisiSaldo(int kondisiSaldo) {
  _kondisiSaldo = kondisiSaldo;
  refreshDisplay0();
  return true;
}

bool DisplayInterface::setStatusUsage(int status)
{
  _statusUsage = status;
  refreshDisplay0();
  return true;
}