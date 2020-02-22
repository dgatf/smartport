
#include <SoftwareSerial.h>
#include "smartport.h"
#include "voltage.h"
#include "ntc.h"

#define PIN_SMARTPORT_RX 7
#define PIN_SMARTPORT_TX 7
#define PIN_VOLTAGE1 A1
#define PIN_NTC1 A2
#define SENSOR_ID 5

uint8_t data[10], sensorId, cont = 0;
uint8_t alphaVolt = 30; // alpha*100
uint8_t refreshVolt = 5; // ms/100
uint8_t alphaTemp = 10;
uint8_t refreshTemp = 10;

SoftwareSerial smartportSerial(PIN_SMARTPORT_RX, PIN_SMARTPORT_TX, true);
Smartport smartport(smartportSerial);

void setup() {
  Serial.begin(115200);
  smartportSerial.begin(57600);
  sensorId = smartport.getSensorIdCrc(SENSOR_ID);
  
  Sensor *sensorP;
  VoltageInterface *voltage;
  voltage = new VoltageInterface(PIN_VOLTAGE1, alphaVolt);
  sensorP = new Sensor(A3_FIRST_ID, 10, voltage);
  smartport.addSensor(sensorP);
  
  NtcInterface *ntc;
  ntc = new NtcInterface(PIN_NTC1, alphaTemp);
  sensorP = new Sensor(T1_FIRST_ID, 0, ntc);
  smartport.addSensor(sensorP);

}

void loop() {
  smartport.update();
}
