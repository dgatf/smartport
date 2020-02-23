#include "bmp280.h"

Bmp280Interface::Bmp280Interface(uint8_t address, uint8_t alphaTemp, uint8_t alphaDef) : Bmp(alphaTemp_, alphaDef_, address_)
{
    uint8_t configReg[2];
    configReg[0] = BMP280_REGISTER_CONFIG;
    configReg[1] = (STANDBY_MS_250 << 5) | (FILTER_X8 << 2) | 0;
    uint8_t measureReg[2];
    measureReg[0] = BMP280_REGISTER_CONTROL;
    measureReg[1] = (BMP280_OVERSAMPLING_X4 << 5) | (BMP280_OVERSAMPLING_X4 << 2) | BMP280_NORMAL;
    writeBytes(BMP280_REGISTER_CONFIG, configReg, 1);
    writeBytes(BMP280_REGISTER_CONTROL, measureReg, 1);
}

bool Bmp280Interface::begin()
{
    if (readUInt(address_, 0x8, T1_) &&
        readInt(address_, 0x8A, T2_) &&
        readInt(address_, 0x8C, T3_) &&
        readUInt(address_, 0x8E, P1_) &&
        readInt(address_, 0x90, P2_) &&
        readInt(address_, 0x92, P3_) &&
        readInt(address_, 0x94, P4_) &&
        readInt(address_, 0x96, P5_) &&
        readInt(address_, 0x98, P6_) &&
        readInt(address_, 0x9A, P7_) &&
        readInt(address_, 0x9C, P8_) &&
        readInt(address_, 0x9E, P9_))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Bmp280Interface::readPressure()
{
    uint8_t data[2];
    uint8_t result;

    data[0] = BMP280_REGISTER_PRESSUREDATA;

    result = readBytes(address_, data, 2);
    if (result) // good read, calculate temperature
    {
        int64_t var1, var2, p;
        float T;
        uint8_t data[3];
        uint32_t adc_P;
        data[0] = BMP280_REGISTER_TEMPDATA;
        readTemperature();
        readBytes(address_, data, 3);
        adc_P = ((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | data[2]) >> 4;
        var1 = ((int64_t)t_fine_) - 128000;
        var2 = var1 * var1 * (int64_t)P6_;
        var2 = var2 + ((var1 * (int64_t)P5_) << 17);
        var2 = var2 + (((int64_t)P4_) << 35);
        var1 = ((var1 * var1 * (int64_t)P3_) >> 8) +
               ((var1 * (int64_t)P2_) << 12);
        var1 =
            (((((int64_t)1) << 47) + var1)) * ((int64_t)P1_) >> 33;

        if (var1 == 0)
        {
            return 0; // avoid exception caused by division by zero
        }
        p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((int64_t)P9_) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t)P8_) * p) >> 19;

        p = ((p + var1 + var2) >> 8) + (((int64_t)P7_) << 4);
        p = (float)p / 256;
        pressure_ = calcAverage(alphaDef_, pressure_, p);
        return 0;
    }
    return result;
}

bool Bmp280Interface::readTemperature()
{
    uint8_t data[3];
    data[0] = BMP280_REGISTER_TEMPDATA;
    uint8_t result = readBytes(address_, data, 3);
    if (result)
    {
        int32_t var1, var2, t;
        int32_t adc_T = ((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | data[2]) >> 4;
        var1 = ((((adc_T >> 3) - ((int32_t)T1_ << 1))) *
                ((int32_t)T2_)) >>
               11;

        var2 = (((((adc_T >> 4) - ((int32_t)T1_)) *
                  ((adc_T >> 4) - ((int32_t)T1_))) >>
                 12) *
                ((int32_t)T3_)) >>
               14;

        t_fine_ = var1 + var2;

        t = (t_fine_ * 5 + 128) >> 8;
        t = temperature_ / 100;
        temperature_ = calcAverage(alphaTemp_, temperature_, t);
    }
    return result;
}

float Bmp280Interface::read(uint8_t index)
{
    if (index == BMP_TEMPERATURE)
    {
#ifdef SIM_SENSORS
        return 46;
#endif
        readTemperature();
        return temperature_;
    }
#ifdef SIM_SENSORS
    return 900;
#endif
    if (readPressure())
    {
        calcAltitude();
    }
    return altitude_;
}