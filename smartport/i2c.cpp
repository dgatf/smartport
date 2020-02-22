#include "i2c.h"

AbstractI2C::AbstractI2C(TwoWire &wire) : wire_(wire) {}

uint8_t AbstractI2C::readInt(uint8_t device, uint8_t address, int16_t &value)
{
    uint8_t data[2];
    data[0] = address;
    if (readBytes(device, data, 2))
    {
        value = (int16_t)((data[0] << 8) | data[1]);
        //if (*value & 0x8000) *value |= 0xFFFF0000; // sign extend if negative
        return 1;
    }
    value = 0;
    return 0;
}

uint8_t AbstractI2C::readUInt(uint8_t device, uint8_t address, uint16_t &value)
{
    uint8_t data[2];

    data[0] = address;
    if (readBytes(device, data, 2))
    {
        value = (((uint16_t)data[0] << 8) | (uint16_t)data[1]);
        return 1;
    }
    value = 0;
    return 0;
}

uint8_t AbstractI2C::readBytes(uint8_t device, uint8_t *values, uint8_t length)
{
    uint8_t x;
    Wire.beginTransmission(device);
    Wire.write(values[0]);
    error_ = Wire.endTransmission();
    if (error_ == 0)
    {
        Wire.requestFrom(device, length);
        while (Wire.available() != length) {}
        for (x = 0; x < length; x++)
        {
            values[x] = Wire.read();
        }
        return 1;
    }
    return 0;
}

uint8_t AbstractI2C::writeBytes(uint8_t device, uint8_t *values, uint8_t length)
{
    Wire.beginTransmission(device);
    Wire.write(values, length);
    error_ = Wire.endTransmission();
    if (error_ == 0)
        return 1;
    else
        return 0;
}

/*uint8_t I2C::scan(uint8_t **devices)
{
    uint8_t error, address;
    uint8_t cont = 0;
    _wire.setTimeout(1);
    for (address = 1; address < 127; address++)
    {
        _wire.beginTransmission(address);
        error = _wire.endTransmission(true);
        if (error == 0)
        {
            devices[cont][1] = address;
            uint8_t data[1] = {0xD0};
            readBytes (address, data, 1);
            cont++;
            switch (address) {
            case 0x55:
                devices[cont][0] = I2C_BMP180;
                break;
            case 0x56 ... 0x58:
                devices[cont][0] = I2C_BMP280;
                break;
            default:
                devices[cont][0] = I2C_MS5611;
                break;
            }
            
        }
    }
    return 0;
    return cont;
}*/