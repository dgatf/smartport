#include "bmp180.h"

Bmp180Interface::Bmp180Interface(TwoWire &wire, uint8_t address, uint8_t alphaTemp, uint8_t alphaDef) : AbstractBmp(wire_, address_, alphaTemp_, alphaDef_) {}

bool Bmp180Interface::begin()
{
    //wire_.begin();
    Serial.println(millis());
    float c3, c4, b1;
    if (readInt(address_, 0xAA, AC1_) &&
        readInt(address_, 0xAC, AC2_) &&
        readInt(address_, 0xAE, AC3_) &&
        readUInt(address_, 0xB0, AC4_) &&
        readUInt(address_, 0xB2, AC5_) &&
        readUInt(address_, 0xB4, AC6_) &&
        readInt(address_, 0xB6, VB1_) &&
        readInt(address_, 0xB8, VB2_) &&
        readInt(address_, 0xBA, MB_) &&
        readInt(address_, 0xBC, MC_) &&
        readInt(address_, 0xBE, MD_))
    {
        c3 = 160.0 * pow(2, -15) * AC3_;
        c4 = pow(10, -3) * pow(2, -15) * AC4_;
        b1 = pow(160, 2) * pow(2, -30) * VB1_;
        c5_ = (pow(2, -15) / 160) * AC5_;
        c6_ = AC6_;
        mc_ = (pow(2, 11) / pow(160, 2)) * MC_;
        md_ = MD_ / 160.0;
        x0_ = AC1_;
        x1_ = 160.0 * pow(2, -13) * AC2_;
        x2_ = pow(160, 2) * pow(2, -25) * VB2_;
        y0_ = c4 * pow(2, 15);
        y1_ = c4 * c3;
        y2_ = c4 * b1;
        p0_ = (3791.0 - 8.0) / 1600.0;
        p1_ = 1.0 - 7357.0 * pow(2, -20);
        p2_ = 3038.0 * 100.0 * pow(2, -36);
        return 1;
    }
    else
    {
        return 0;
    }
    return 0;
}

uint8_t Bmp180Interface::startTemperature()
{
    uint8_t data[2], result;

    data[0] = BMP_REG_CONTROL;
    data[1] = BMP180_COMMAND_TEMPERATURE;
    result = writeBytes(address_, data, 2);
    if (result)
        return 5;
    else
        return 0;
}

uint8_t Bmp180Interface::startPressure()
{
    uint8_t data[2], result, delay;

    data[0] = BMP_REG_CONTROL;

    switch (sampling_)
    {
    case 0:
        data[1] = BMP180_COMMAND_PRESSURE0;
        delay = 5;
        break;
    case 1:
        data[1] = BMP180_COMMAND_PRESSURE1;
        delay = 8;
        break;
    case 2:
        data[1] = BMP180_COMMAND_PRESSURE2;
        delay = 14;
        break;
    case 3:
        data[1] = BMP180_COMMAND_PRESSURE3;
        delay = 26;
        break;
    default:
        data[1] = BMP180_COMMAND_PRESSURE0;
        delay = 5;
        break;
    }
    result = writeBytes(address_, data, 2);
    if (result)
        return delay; // return the delay in ms (rounded up) to wait before retrieving data
    else
        return 0; // or return 0 if there was a problem communicating with the BMP
}

bool Bmp180Interface::readTemperature()
{
    if (state_ == BMP180_STATE_READY)
    {
        state_ = BMP180_STATE_WAIT_TEMP;
        delay_ = startTemperature();
        timestamp_ = millis();
        return false;
    }
    if (state_ == BMP180_STATE_WAIT_PRESS)
    {
        return false;
    }
    if (state_ == BMP180_STATE_WAIT_TEMP && millis() - timestamp_ > delay_)
    {
        uint8_t data[2];
        char result;
        float tu, a, t;

        data[0] = BMP180_REG_RESULT;

        result = readBytes(address_, data, 2);
        if (result) // good read, calculate temperature
        {
            tu = (data[0] * 256.0) + data[1];
            a = c5_ * (tu - c6_);
            t = a + (mc_ / (a + md_));
            temperature_ = calcAverage(alphaTemp_, temperature_, t);
        }
        return result;
    }
    return 5;
}

bool Bmp180Interface::readPressure()
{
    if (state_ == BMP180_STATE_READY)
    {
        state_ = BMP180_STATE_WAIT_PRESS;
        delay_ = startPressure();
        timestamp_ = millis();
        return false;
    }
    if (state_ == BMP180_STATE_WAIT_TEMP)
    {
        return false;
    }
    if (state_ == BMP180_STATE_WAIT_PRESS && millis() - timestamp_ > delay_)
    {

        uint8_t data[3];
        uint8_t result;
        float pu, s, x, y, z, p;

        data[0] = BMP180_REG_RESULT;
        result = readBytes(address_, data, 3);
        if (result)
        {
            pu = (data[0] * 256.0) + data[1] + (data[2] / 256.0);
            s = temperature_ - 25.0;
            x = (x2_ * pow(s, 2)) + (x1_ * s) + x0_;
            y = (y2_ * pow(s, 2)) + (y1_ * s) + y0_;
            z = (pu - x) / y;
            p = (p2_ * pow(z, 2)) + (p1_ * z) + p0_;
            pressure_ = calcAverage(alphaDef_, pressure_, p);
        }
        return result;
    }
    return false;
}

float Bmp180Interface::read(uint8_t index)
{
    if (index == BMP_TEMPERATURE)
    {
#ifdef SIM_POLL
        return 25;
#endif
        readTemperature();
        return temperature_;
    }
#ifdef SIM_POLL
    return 25;
#endif
    if (readPressure())
    {
        calcAltitude();
    }
    return altitude_;
}