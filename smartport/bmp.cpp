#include "bmp.h"

AbstractBmp::AbstractBmp(TwoWire &wire, uint8_t alphaTemp, uint8_t alphaDef, uint8_t address) : AbstractI2C(wire_), alphaTemp_(alphaTemp), alphaDef_(alphaDef), address_(address) {}

float AbstractBmp::calcAltitude()
{
	return 44330.0 * (1 - pow(pressure_ / P0_, 1 / 5.255));
}