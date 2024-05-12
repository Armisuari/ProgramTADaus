#pragma once

#include "config.h"
#include <Arduino.h>
#include <Wire.h>
// #include <SimpleModbusMaster.h>
#include <ModbusMaster.h>

class PLCInterface {

public:
  bool begin();
  // unsigned int regs[CONFIG_PLC_TOTAL_REG];
  bool update(uint16_t writtenAddres, uint16_t writtenValue);
  bool update_coil(uint16_t writtenAddres, bool coilState);

private:
  ModbusMaster node;


  // enum {
  //   PACKET1, // voltage
  //   PACKET2, // power
  //   PACKET3, // saldo
  //   PACKET4, // energy
  //   PACKET5, // getData
  //   TOTAL_NO_OF_PACKETS  // leave this last entry
  // };

  // bool sendVoltage();
  // bool sendPower();
  // bool sendSaldo();
  // bool sendEnergy();
  // bool getData();
  // Packet packets[TOTAL_NO_OF_PACKETS];
};

bool PLCInterface::begin() {
  CONFIG_PLC_SERIAL.begin(CONFIG_PLC_BAUD);
  node.begin(CONFIG_PLC_SLAVEID, CONFIG_PLC_SERIAL);
  return true;
}

bool PLCInterface::update(uint16_t writtenAddres, uint16_t writtenValue) {
  // uint8_t result;
  uint8_t result = node.writeSingleRegister(writtenAddres, writtenValue);

  if (result == node.ku8MBSuccess) {
    // Writing to the register was successful
    // Serial.print("Write successful to register ");
    // Serial.println(writtenAddres);
    return true;
  } else {
    // Writing to the register failed
    // Serial.print("Write failed to register ");
    // Serial.print(writtenAddres);
    // Serial.print(", Error: ");
    // Serial.println(result, HEX);
    return false;
  }

  return true;
}

bool PLCInterface::update_coil(uint16_t coilAddress, bool coilState) {
  // Write a value to the coil
  uint8_t result = node.writeSingleCoil(coilAddress, coilState);

  if (result == node.ku8MBSuccess) {
    // Writing to the coil was successful
    Serial.println("Write successful to coil");
    return true;
  } else {
    // Writing to the coil failed
    // Serial.print("Write failed to coil, Error: ");
    // Serial.println(result, HEX);
    return false;
  }
  return true;
}



// bool PLCInterface::sendVoltage() {
//   modbus_construct(&packets[PACKET1], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 0, 7, 0);

//   return true;
// }

// bool PLCInterface::sendPower() {
//   modbus_construct(&packets[PACKET2], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 1, 7, 0);

//   return true;
// }

// bool PLCInterface::sendSaldo() {
//   modbus_construct(&packets[PACKET3], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 2, 7, 0);

//   return true;
// }

// bool PLCInterface::sendEnergy() {
//   modbus_construct(&packets[PACKET4], CONFIG_PLC_SLAVEID, PRESET_SINGLE_REGISTER, 3, 7, 0);

//   return true;
// }

// bool PLCInterface::begin() {
//   sendVoltage();
//   sendPower();
//   sendSaldo();
//   sendEnergy();
//   // getData();
//   modbus_configure(&Serial, CONFIG_PLC_BAUD, SERIAL_8N1, CONFIG_PLC_TIMEOUT, CONFIG_PLC_POLLING, CONFIG_PLC_RETRY, CONFIG_PLC_TX_ENABLE_PIN, packets, TOTAL_NO_OF_PACKETS, regs);

//   return true;
// }

// bool PLCInterface::update() {
//   modbus_update();

//   return true;
// }

// bool PLCInterface::getData() {
//   modbus_construct(&packets[PACKET5], CONFIG_PLC_SLAVEID, READ_HOLDING_REGISTERS, 0, 7, 0);

//   return true;
// }