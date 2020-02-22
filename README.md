# Smartport library

An arduino smartport library for the frsky protocol

This library can be used at low level or high level:

 - Low level. Use sendData, sendVoid and formatdata to send smartport packets
 - High level. Encapsulate the communication and control the smartport with sensors added to a circular buffer and manage receiveing and sending packets easily 

Initalize the smartport object passing the serial connection stream. Usually softwareserial with a single wire inverted. Use sendData, sendVoid and formatdata to send packets. Use read to receive packets

Use classes sensor and device to abstract from low level protocol:

Initialize sensorId, needed to send telemetry and sensorIdTx to process incoming packets (do not confuse with polls)

Sensor is the sensor in smartport and device is the physical sensor in arduino. Define sensors and link to a measurement device. More devices may be implemented from the abstract class device

Use addSensor to the sensors in a circular buffer

Each time update is called a sensor is updated and telemetry packet is sent (if there is a poll request and the refresh rate of any sensor is over). For each sensor define a refresh rate (0 for instant rate) and the EMA alpha (alpha = 1 for no averaging)
packets in maintenance mode use sendPacket  (note: for now only one packet at a time. Fifo to be implemented)
Maintenance mode is handled. When activated, the telemetry stops. To send

