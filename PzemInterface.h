#pragma once

#include "config.h"
#include <Arduino.h>
#include <PZEM004Tv30.h>

// typedef struct
// {
//   float voltage;
//   float current;
//   float power;
//   float energy;
//   float PF;
// } PzemData_t;

typedef struct
{
  float value[5]; // Array to store voltage, power, current, energy, and PF
} PzemData_t;


class PzemInterface {
public:
  bool begin();
  bool readVoltage(size_t index, PzemData_t &v);
  bool readCurrent(size_t index, PzemData_t &i);
  bool readPower(size_t index, PzemData_t &p);
  bool readEnergy(size_t index, PzemData_t &e);
  bool readPF(size_t index, PzemData_t &pf);

private:
  PZEM004Tv30 pzem[CONFIG_PZEM_NUM];
};


bool PzemInterface::begin() {
  for (size_t i = 0; i < CONFIG_PZEM_NUM; ++i) {
    pzem[i] = PZEM004Tv30(CONFIG_PZEM_SERIAL, CONFIG_PZEM_RX_PIN, CONFIG_PZEM_TX_PIN, 1 + i);
  }

  return true;
}

bool PzemInterface::readVoltage(size_t index, PzemData_t &v) {
  v.value[0] = pzem[index].voltage();
  return true;
}

bool PzemInterface::readPower(size_t index, PzemData_t &p) {
  p.value[1] = pzem[index].power();
  return true;
}

bool PzemInterface::readCurrent(size_t index, PzemData_t &i)
{
  i.value[2] = pzem[index].current();
  return true;
}

bool PzemInterface::readEnergy(size_t index, PzemData_t &e) {
  e.value[3] = pzem[index].energy();
  return true;
}


bool PzemInterface::readPF(size_t index, PzemData_t &pf)
{
  pf.value[4] = pzem[index].pf();
  return true;
}