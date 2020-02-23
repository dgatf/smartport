#include "ms5611.h"

MS5611Interface::MS5611Interface(uint8_t address, uint8_t alphaTemp, uint8_t alphaDef) : Bmp(alphaTemp_, alphaDef_, address_)  {}

bool MS5611Interface::begin()
{
    return 0;
}

bool MS5611Interface::readTemperature()
{
    return 0;
}

bool MS5611Interface::readPressure()
{
    return 0;
}
