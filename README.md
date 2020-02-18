Smartport library

A smartport library for the frsky protocol

This library can be used at low level or high level:

 - Low level. Use sendData, sendVoid and formatdata to send smartport packets
 - Hihg level. Encapsulate the communication and control the smartport with sensors in a circular buffer and manage receiveing and sending packets easily 

Initalize the smartport object passing the serial connection stream. Usually passed a softwareserial object with a single and inverted line

Initialize sensorId needed to send telemetry. Initialize sensorIdTx to process incoming packets (do not confuse with polls)

At low level use sendData (if frameId is not defined, defaults to 0x10 which is used for telemetry) to send packets. If no new telemerty data available use sendVoid

Format the telemetry values with formatData. Usualy is composed of one element (valueL), but some cases is composed of two (valueL, valueM)

To check the smartport port use read. It returns the type of data (poll, packet or null) and the packet itself if applicable

At a higher level use the abstract class sensor to create any type of sensor, then add sensors with addSensor to a circular buffer

Each time update is called a sensor is updated and telemetry packet is sent (if the refrash rate of any sensor is over). For each sensor define a refresh rate (0 for instant rate) and the EMA alpha (alpha = 1 for no averaging)

Maintenance mode is handled. When activated, the telemetry stops. To send packets in maintenance mode use sendPacket  (note: for now only one packet at a time. Fifo to be implemented)

