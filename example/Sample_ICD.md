# Sample
* Generated: 03/24/19<br/>
* CRC: AD1ABF4A

-----

Every Packet has a standard Header before the data

|***Byte***|0|1|2|3|4|5|6|
|---|---|---|---|---|---|---|---|
|***Field***<td colspan='1'>***Id***<td colspan='2'>***Len***<td colspan='2'>***Token***<td colspan='2'>***Checksum***
|***Type***<td colspan='1'>uint8<td colspan='2'>uint16<td colspan='2'>uint16<td colspan='2'>uint16

>***Id*** : The ID used to identify the type of packet, packet Ids are assigned and managed automatically<br/>
>***Len*** : This is the len of the packet data, this does not include the header and checksum<br/>
>***Token*** : A unique randomly generated token. Each packet is tokenized to provide functions like ack/retry and preventing duplicates <br/>
>***Checksum*** : A calculated checksum of the data in the packet
----
# Packet Types:


### SendCmd
Message to set command in node

|***Byte***|0|
|---|---|
|***Field***<td colspan='1'>***cmd***
|***Type***<td colspan='1'>uint8_t


>***cmd*** : <br/>
>> **0x00** : LED_ON - turns on led<br/>
>> **0x01** : LED_OFF - turns off led<br/>
>> **0x02** : RESET - resets the device<br/>
>

------



### Data
Message containing data from sensor

* *Responds To: GetData*

|***Byte***|0|1|2|3|4|5|6| . . . . . . . |37
|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>sensorA<td colspan='4'>sensorB<td colspan='4'>sensorName
|***Type***<td colspan='2'>int16_t<td colspan='4'>int<td colspan='4'>char[32]


>***sensorA*** : Value of Sensor A<br/>
>***sensorB*** : Value of Sensor B<br/>
>***sensorName*** : Name of sensor responding to message <br/>

------

