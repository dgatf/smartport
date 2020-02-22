# Smartport library

An arduino smartport library for the frsky protocol

This library can be used at low level or high level:

 - Low level. Use *sendData, sendVoid and formatdata* to send smartport packets
 - High level. Encapsulate the communication and control the smartport with sensors added to a circular buffer and manage receiveing and sending packets easily 

Initalize the smartport object passing the serial connection stream. Usually a softwareserial, single wire and inverted. Use *sendData, sendVoid and formatdata* to send packets. Use *read* to receive packets

Use classes *sensor* and *device* to abstract away from low level protocol:

Initialize *sensorId*, needed to send telemetry and *sensorIdTx* to process incoming packets (do not confuse with polls)

Sensor is the sensor in smartport and device is the physical sensor in arduino. Define sensors and link to a measurement device. More devices may be implemented from the abstract class device

Use *addSensor* to add sensors in a circular buffer

Calling *update* sensors are updated until refresh rate is achieved in any sensor or all sensors have been updated. Then a telemetry packet is sent. For each sensor you can define a refresh rate (1 = 100ms) and the EMA alpha (33 = 0.33, alpha = 1 for no averaging)

Maintenance mode is handled. When activated, the telemetry stops. To send packets while in maintenance mode use sendPacket (note: for now only one packet at a time. Fifo to be implemented)

