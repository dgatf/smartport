#ifndef I2C_H
#define I2C_H

#include <Arduino.h>
#include <Wire.h>

#define I2C_BMP180 1
#define I2C_BMP280 2
#define I2C_MS5611 3

class AbstractI2C
{
private:
protected:
    uint8_t error_;
    uint8_t address_;
public:
    TwoWire &wire_;
    AbstractI2C(TwoWire &wire);
    //uint8_t scan(uint8_t *devices);
    uint8_t readInt(uint8_t device, uint8_t address, int16_t &value);
    uint8_t readUInt(uint8_t device, uint8_t address, uint16_t &value);
    uint8_t readBytes(uint8_t device, uint8_t *values, uint8_t length);
    uint8_t writeBytes(uint8_t device, uint8_t *values, uint8_t length);
    uint8_t getError();
    virtual bool begin() = 0;
};

#endif