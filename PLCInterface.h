#pragma once

#include "config.h"
#include <Arduino.h>
#include <Wire.h>
#include <SimpleModbusMaster.h>

class PLCInterface {

public:
  bool begin();
  unsigned int regs[CONFIG_PLC_TOTAL_REG];
  bool update();

private:
  enum {
    PACKET1, // voltage
    PACKET2, // power
    PACKET3, // saldo
    PACKET4, // energy
    PACKET5, // getData
    TOTAL_NO_OF_PACKETS  // leave this last entry
  };

  bool sendVoltage();
  bool sendPower();
  bool sendSaldo();
  bool sendEnergy();
  bool getData();
  Packet packets[TOTAL_NO_OF_PACKETS];
};

bool PLCInterface::sendVoltage() {
  modbus_construct(&packets[PACKET1], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 0, 7, 0);

  return true;
}

bool PLCInterface::sendPower() {
  modbus_construct(&packets[PACKET2], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 1, 7, 0);

  return true;
}

bool PLCInterface::sendSaldo() {
  modbus_construct(&packets[PACKET3], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 2, 7, 0);

  return true;
}

bool PLCInterface::sendEnergy() {
  modbus_construct(&packets[PACKET4], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 3, 7, 0);

  return true;
}

bool PLCInterface::begin() {
  sendVoltage();
  sendPower();
  sendSaldo();
  sendEnergy();
  // getData();
  modbus_configure(&Serial, CONFIG_PLC_BAUD, SERIAL_8N1, CONFIG_PLC_TIMEOUT, CONFIG_PLC_POLLING, CONFIG_PLC_RETRY, CONFIG_PLC_TX_ENABLE_PIN, packets, TOTAL_NO_OF_PACKETS, regs);

  return true;
}

bool PLCInterface::update() {
  modbus_update();

  return true;
}

bool PLCInterface::getData() {
  modbus_construct(&packets[PACKET5], CONFIG_PLC_SLAVEID, READ_HOLDING_REGISTERS, 0, 7, 0);

  return true;
}