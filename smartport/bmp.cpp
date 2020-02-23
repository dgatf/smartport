#include "bmp.h"

Bmp::Bmp(uint8_t alphaTemp, uint8_t alphaDef, uint8_t address) : alphaTemp_(alphaTemp), alphaDef_(alphaDef), address_(address) {}

float Bmp::calcAltitude()
{
	return 44330.0 * (1 - pow(pressure_ / P0_, 1 / 5.255));
}