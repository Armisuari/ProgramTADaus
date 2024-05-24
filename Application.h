#pragma once

#include "PzemInterface.h"
#include "PLCInterface.h"
#include "DisplayInterface.h"
#include <EEPROM.h>

typedef enum {
  AMAN,
  HAMPIR_HABIS,
  HAMPIR_HABIS_SEKALI,
  HABIS
} KondisiSaldo;

typedef enum {
  NORMAL,
  BOROS
} EnergyUsage;

class Application {
public:
  bool init();
  bool updateMeterData();
  bool sendToPLC();
  bool updateSaldo();

private:
  PzemInterface _pzem;
  PLCInterface _plc;
  DisplayInterface _disp[2] = { DisplayInterface(0x26), DisplayInterface(0x27) };
  PzemData_t data[2];

  KondisiSaldo kondisiSaldo[2];
  KondisiSaldo updateSaldoCondition(float saldo);

  EnergyUsage energyUsage[2];
  EnergyUsage updateEnergyUsage(float energy, float limitKwh);

  long saldoToken[2];
  float saldoKwh[2];
  float limitToken[2];
  float limitKwh[2];

  unsigned long _lastResetTime;
};

bool Application::init() {

  saldoToken[0] = 20;
  saldoToken[1] = 20;

  limitToken[0] = 10;
  limitToken[1] = 10;

  for (int i = 0; i < 2; ++i) {
    saldoKwh[i] = float(saldoToken[i]) / 1444.0 * 1000;
    limitKwh[i] = float(limitToken[i]) / 1444.0 * 30;
    // Serial.printf("[Application] saldoKwh = %.2f\n", saldoKwh[i]);
    _disp[i].setSaldoValue(saldoKwh[i]);
  }

  if (!_pzem.begin() || !_plc.begin() || !_disp[0].begin() || !_disp[1].begin()) {
    // if (!_pzem.begin() || !_disp[0].begin() || !_disp[1].begin()) {
    return false;
  }

  // Read the last reset time from EEPROM
  EEPROM.get(0, _lastResetTime);

  // Check if the stored time is valid
  if (_lastResetTime == 0xFFFFFFFF) {
    _lastResetTime = millis();  // If not valid, initialize with the current time
    EEPROM.put(0, _lastResetTime);
  }

  return true;
}

bool Application::updateMeterData() {
  for (int i; i < 2; ++i) {
    _pzem.readPower(i, data[i]);
    _pzem.readVoltage(i, data[i]);
    _pzem.readCurrent(i, data[i]);
    _pzem.readEnergy(i, data[i]);
    _pzem.readPF(i, data[i]);
    kondisiSaldo[i] = updateSaldoCondition(saldoKwh[i]);
    updateEnergyUsage(data[i].value[3], limitKwh[i]);
  }

  static bool _switch;
  static unsigned long prevMillSwitch;

  if (millis() - prevMillSwitch >= 3000) {
    prevMillSwitch = millis();

    for (int i = 0; i < 2; ++i) {
      if (_switch) {
        _disp[i].setPowerValue(data[i].value[1]);
        _disp[i].setKondisiSaldo(int(kondisiSaldo[i]));
        _disp[i].setStatusUsage(int(energyUsage[i]));
        // Serial.println("display power");
      } else {
        _disp[i].setVoltageValue(data[i].value[0]);
        _disp[i].setCurrentValue(data[i].value[2]);
        _disp[i].setPfValue(data[i].value[4]);
        // Serial.println("display voltage");
      }
    }
    _switch = !_switch;
  }

  return true;
}

bool Application::updateSaldo() {
  static unsigned long prevMillSaldo;
  if (millis() - prevMillSaldo >= 60000UL) {
    prevMillSaldo = millis();
    for (int i = 0; i < 2; ++i) {
      saldoKwh[i] = saldoKwh[i] - (data[i].value[1] / 60);
      _disp[i].setSaldoValue(saldoKwh[i]);
    }
  }
  return true;
}

bool Application::sendToPLC() {
  // kamar 1
  _plc.update(0, data[0].value[0]);
  _plc.update(1, data[0].value[1]);
  _plc.update(2, kondisiSaldo[0]);
  _plc.update(3, energyUsage[0]);

  // kamar 2
  _plc.update(4, data[1].value[0]);
  _plc.update(5, data[1].value[1]);
  _plc.update(6, kondisiSaldo[1]);
  _plc.update(7, energyUsage[1]);
  return true;
}

KondisiSaldo Application::updateSaldoCondition(float saldo) {
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

EnergyUsage Application::updateEnergyUsage(float energy, float limitKwh) {
  if (energy >= limitKwh) {
    return BOROS;
  } else {
    return NORMAL;
  }
}