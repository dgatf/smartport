#include "smartport.h"

AbstractDevice::AbstractDevice() {}

float AbstractDevice::calcAverage(float alpha, float value, float newValue)
{
    return value + alpha * (newValue - value);
}

Sensor::Sensor(uint16_t dataId, uint8_t indexM, uint8_t indexL, uint8_t refresh, AbstractDevice *device) : dataId_(dataId), indexL_(indexL), indexM_(indexM), refresh_(refresh), device_(device) {}
Sensor::Sensor(uint16_t dataId, uint8_t indexL, uint8_t refresh, AbstractDevice *device) : dataId_(dataId), indexL_(indexL), refresh_(refresh), device_(device) {}
Sensor::Sensor(uint16_t dataId, uint8_t refresh, AbstractDevice *device) : dataId_(dataId), refresh_(refresh), device_(device) {}

float Sensor::read(uint8_t index)
{
    return device_->read(index);
}

uint8_t Sensor::indexL()
{
    return indexL_;
}

uint8_t Sensor::indexM()
{
    return indexM_;
}

uint16_t Sensor::timestamp()
{
    return timestamp_;
}

void Sensor::setTimestamp(uint16_t timestamp)
{
    timestamp_ = timestamp;
}

uint16_t Sensor::dataId()
{
    return dataId_;
}

uint16_t Sensor::frameId()
{
    return frameId_;
}

uint8_t Sensor::refresh()
{
    return refresh_;
}

float Sensor::valueM()
{
    return valueM_;
}

void Sensor::setValueM(float value)
{
    valueM_ = value;
}

float Sensor::valueL()
{
    return valueM_;
}

void Sensor::setValueL(float value)
{
    valueL_ = value;
}

Smartport::Smartport(Stream &serial) : serial(serial)
{
    pinMode(LED_SMARTPORT, OUTPUT);
}

uint8_t Smartport::sensorId()
{
    return sensorId_;
}

void Smartport::setSensorId(uint8_t sensorId)
{
    sensorId_ = sensorId;
}

void Smartport::setSensorIdTx(uint8_t sensorIdTx)
{
    sensorIdTx_ = sensorIdTx;
}

uint8_t Smartport::maintenanceMode()
{
    return maintenanceMode_;
}

uint8_t Smartport::available()
{
    return serial.available();
}

uint8_t Smartport::getSensorIdCrc(uint8_t sensorId)
{
    uint8_t sensorIdMatrix[28] = {0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0xC6, 0x67, 0x48, 0xE9, 0x6A, 0xCB, 0xAC, 0xD, 0x8E, 0x2F, 0xD0, 0x71, 0xF2, 0x53, 0x34, 0x95, 0x16, 0xB7, 0x98, 0x39, 0xBA, 0x1B};
    if (sensorId > 28)
    {
        return 0;
    }
    return sensorIdMatrix[sensorId - 1];
}

void Smartport::sendByte(uint8_t c, uint16_t *crcp)
{
    uint16_t crc = *crcp;

    if (crcp != NULL)
    {
        crc += c;
        crc += crc >> 8;
        crc &= 0x00FF;
        *crcp = crc;
    }

    if (c == 0x7D || c == 0x7E)
    {
        serial.write(0x7D);
        c ^= 0x20;
    }

    serial.write(c);
}

void Smartport::sendData(uint16_t dataId, uint32_t val)
{
    sendData(0x10, dataId, val);
}

void Smartport::sendData(uint8_t typeId, uint16_t dataId, uint32_t val)
{
    digitalWrite(LED_SMARTPORT, HIGH);
    uint16_t crc = 0;
    uint8_t *u8p;
    // typeId
    sendByte(typeId, &crc);
    // dataId
    u8p = (uint8_t *)&dataId;
    sendByte(u8p[0], &crc);
    sendByte(u8p[1], &crc);
    // val
    u8p = (uint8_t *)&val;
    sendByte(u8p[0], &crc);
    sendByte(u8p[1], &crc);
    sendByte(u8p[2], &crc);
    sendByte(u8p[3], &crc);
    // crc
    sendByte(0xFF - (uint8_t)crc, NULL);
    digitalWrite(LED_SMARTPORT, LOW);
}

void Smartport::sendVoid()
{
    uint8_t buf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    serial.write(buf, 8);
}

uint32_t Smartport::formatData(uint16_t dataId, float valueM, float valueL)
{

    if ((dataId >= GPS_SPEED_FIRST_ID && dataId <= GPS_SPEED_LAST_ID) ||
        (dataId >= RBOX_BATT1_FIRST_ID && dataId <= RBOX_BATT2_FIRST_ID))
        return round(valueL * 1000);

    if ((dataId >= ALT_FIRST_ID && dataId <= VARIO_LAST_ID) ||
        (dataId >= VFAS_FIRST_ID && dataId <= VFAS_LAST_ID) ||
        (dataId >= ACCX_FIRST_ID && dataId <= GPS_ALT_LAST_ID) ||
        (dataId >= GPS_COURS_FIRST_ID && dataId <= GPS_COURS_LAST_ID) ||
        (dataId >= A3_FIRST_ID && dataId <= A4_LAST_ID))
        return round(valueL * 100);

    if ((dataId >= CURR_FIRST_ID && dataId <= CURR_LAST_ID) ||
        (dataId >= AIR_SPEED_FIRST_ID && dataId <= AIR_SPEED_LAST_ID) ||
        dataId == A1_ID || dataId == A2_ID || dataId == RXBT_ID)
        return round(valueL * 10);

    if (dataId >= ESC_POWER_FIRST_ID && dataId <= ESC_POWER_LAST_ID)
        return (uint32_t)round(valueM * 100) << 16 | (uint16_t)round(valueL * 100);

    if (dataId >= ESC_RPM_CONS_FIRST_ID && dataId <= ESC_RPM_CONS_LAST_ID)
        return (uint32_t)round(valueM) << 16 | (uint16_t)round((valueL) / 100);

    if (dataId >= SBEC_POWER_FIRST_ID && dataId <= SBEC_POWER_LAST_ID)
        return (uint32_t)round(valueM * 1000) << 16 | (uint16_t)round((valueL)*1000);

    if (dataId >= CELLS_FIRST_ID && dataId <= CELLS_LAST_ID)
        return (uint16_t)round(valueM * 500) << 8 | (uint16_t)valueL;

    return round(valueL);
}

void Smartport::addSensor(Sensor *newSensorP)
{
    static Sensor *prevSensorP;
    if (sensorP == NULL)
    {
        prevSensorP = newSensorP;
        newSensorP->nextP = newSensorP;
    }
    sensorP = newSensorP;
    sensorP->nextP = prevSensorP->nextP;
    prevSensorP->nextP = newSensorP;
    prevSensorP = newSensorP;
}

bool Smartport::addPacket(uint16_t dataId, uint32_t value)
{
    return addPacket(0x10, dataId, value);
}

bool Smartport::addPacket(uint8_t frameId, uint16_t dataId, uint32_t value)
{
    if (packetP == NULL)
    {
        packetP = (Packet *)malloc(sizeof(Packet));
        packetP->frameId = frameId;
        packetP->dataId = dataId;
        packetP->value = value;
        return true;
    }
    return false;
}

void Smartport::deleteSensors()
{
    if (sensorP != NULL)
    {
        Sensor *firstSensorP = sensorP;
        Sensor *nextSensorP;
        do
        {
            nextSensorP = sensorP->nextP;
            free(sensorP);
            sensorP = nextSensorP;
        } while (sensorP != firstSensorP);
        sensorP = NULL;
    }
}

uint8_t Smartport::read(uint8_t *data)
{
    uint8_t cont = 0;
    if (serial.available())
    {
        uint16_t tsRead = millis();
        uint16_t crc = 0;
        while ((uint16_t)millis() - tsRead < SMARTPORT_TIMEOUT)
        {
            if (serial.available())
            {
                data[cont] = serial.read();
                if (data[cont] == 0x7D)
                {
                    data[cont] = serial.read() ^ 0x20;
                }
                cont++;
            }
        }
        if (data[0] != 0x7E)
            return RECEIVED_NONE;
        if (cont == 10)
        {
            for (uint8_t i = 2; i < 9; i++)
            {
                crc += data[i];
                crc += crc >> 8;
                crc &= 0x00FF;
            }
            crc = 0xFF - (uint8_t)crc;
            if (crc != data[9])
                return RECEIVED_NONE;
            else
                return RECEIVED_PACKET;
        }
        if (cont == 2)
            return RECEIVED_POLL;
    }
    return RECEIVED_NONE;
}

uint8_t Smartport::update(uint8_t &frameId, uint16_t &dataId, uint32_t &value)
{
    if (available())
    {
        uint8_t data[64];
        uint8_t packetType;
        packetType = read(data);
        if (packetType == RECEIVED_POLL && data[1] == sensorId_)
        {
            if (packetP != NULL && maintenanceMode_) // if maintenance send packet
            {
                sendData(packetP->frameId, packetP->dataId, packetP->value);
                dataId = packetP->dataId;
                value = packetP->value;
                free(packetP);
                packetP = NULL;
                return SENT_PACKET;
            }
            if (sensorP != NULL && !maintenanceMode_) // else send telemetry
            {
                Sensor *initSensorP = sensorP; // loop updating sensors until correct timestamp or 1 sensors cycle
                while (((uint16_t)millis() - sensorP->timestamp() <= (uint16_t)sensorP->refresh() * 100) || sensorP->nextP == initSensorP)
                {
                    sensorP->setValueL(sensorP->read(sensorP->indexL()));
                    sensorP->setValueM(sensorP->read(sensorP->indexM()));
                    sensorP = sensorP->nextP;
                }
                if ((uint16_t)millis() - sensorP->timestamp() >= (uint16_t)sensorP->refresh() * 100)
                {
                    sensorP->setValueL(sensorP->read(sensorP->indexL()));
                    sensorP->setValueM(sensorP->read(sensorP->indexM()));
                    sendData(sensorP->frameId(), sensorP->dataId(), formatData(sensorP->dataId(), sensorP->valueM(), sensorP->valueL()));
                    sensorP->setTimestamp(millis());
                    sensorP = sensorP->nextP;
                    dataId = sensorP->dataId();
                    return SENT_TELEMETRY;
                }
                else
                {
                    dataId = 0;
                    value = 0;
                    sendVoid();
                    sensorP = sensorP->nextP;
                    return SENT_VOID;
                }
            }
        }
        else if (packetType == RECEIVED_PACKET && data[1] == sensorIdTx_)
        {
            frameId = data[2];
            dataId = (uint16_t)data[4] << 8 | data[3];
            value = (uint32_t)data[8] << 24 | (uint32_t)data[7] << 16 |
                    (uint16_t)data[6] << 8 | data[5];
            if (frameId == 0x21 && dataId == 0xFFFF && value == 0x80)
            {
                maintenanceMode_ = true;
                return MAINTENANCE_ON;
            }
            if (frameId == 0x20 && dataId == 0xFFFF && value == 0x80)
            {
                maintenanceMode_ = false;
                return MAINTENANCE_OFF;
            }
            return RECEIVED_PACKET;
        }
    }
    return SENT_NONE;
}

uint8_t Smartport::update()
{
    uint8_t frameId;
    uint16_t dataId;
    uint32_t value;
    return update(frameId, dataId, value);
}

bool Smartport::sendPacketReady()
{
    if (packetP != NULL)
        return false;
    return true;
}

/*void Smartport::begin(uint32_t baudRate, uint8_t sensorId, uint8_t sensorIdTx)
{
    sensorId = getSensorIdMatrix(sensorId);
    sensorIdTx = getSensorIdMatrix(sensorIdTx);
    //serial.begin(baudRate);
}*/
