
#include <SoftwareSerial.h>
#include "smartport.h"

#define PIN_SMARTPORT_RX 7
#define PIN_SMARTPORT_TX 7
#define SENSOR_ID 5

uint8_t data[10], sensorId, cont = 0;

SoftwareSerial smartportSerial(PIN_SMARTPORT_RX, PIN_SMARTPORT_TX, true);
Smartport smartport(smartportSerial);

void setup() {
  smartportSerial.begin(57600);
  sensorId = smartport.getSensorIdCrc(SENSOR_ID);

}

void loop() {
  if (RECEIVED_POLL == smartport.read(data)) {
    if (data[1] == sensorId) {
      if (cont == 0) {
        float escRpm = 12349;
        smartport.sendData(ESC_RPM_CONS_FIRST_ID, smartport.formatData(ESC_RPM_CONS_FIRST_ID, 0, escRpm));
      }
      if (cont == 1) {
        float volt = 6.78;
        smartport.sendData(VFAS_FIRST_ID, smartport.formatData(VFAS_FIRST_ID, volt));
      }
      if (cont == 2) {
        float volt = 24.45;
        smartport.sendData(VFAS_FIRST_ID + 1, smartport.formatData(ALT_FIRST_ID + 1, volt));
      }
      cont++;
      if (cont == 3) cont = 0;
    }
  }
}
