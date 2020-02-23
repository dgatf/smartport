#ifndef I2C_H
#define I2C_H

#include <Arduino.h>
#include <Wire.h>

class I2C
{
private:
protected:
    uint8_t error_;
    uint8_t address_;
public:
    I2C();
    //uint8_t scan(uint8_t *devices);
    uint8_t readInt(uint8_t device, uint8_t address, int16_t &value);
    uint8_t readUInt(uint8_t device, uint8_t address, uint16_t &value);
    uint8_t readBytes(uint8_t device, uint8_t *values, uint8_t length);
    uint8_t writeBytes(uint8_t device, uint8_t *values, uint8_t length);
    uint8_t getError();
};

#endif