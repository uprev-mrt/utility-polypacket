# Sample ICD
* Generated: 03/30/19<br/>
* CRC: AD1ABF4A
* Transport Encoding: (COBS) [Consistent Overhead ByteStuffing](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing)

** This is a sample protocol made up to demonstrate features of the PolyPacket code generation tool. The idea   is to have a tool that can automatically create parseable/serializable messaging for embedded systems. **

---
## Header

All packets begin with a standard header:

|***Byte***|0|1|2|3|4|5|6|
|---|---|---|---|---|---|---|---|
|***Field***<td colspan='1'>***Id***<td colspan='2'>***Len***<td colspan='2'>***Token***<td colspan='2'>***Checksum***
|***Type***<td colspan='1'>uint8<td colspan='2'>uint16<td colspan='2'>uint16<td colspan='2'>uint16

>***Id*** : The ID used to identify the type of packet, packet Ids are assigned and managed automatically<br/>
>***Len*** : This is the number of data bytes in the packet data, this does not include the header<br/>
>***Token*** : A randomly generated token. Each packet is tokenized to provide functions like ack/retry and preventing duplicates <br/>
>***Checksum*** : A calculated checksum of the data in the packet
----
# Packet Types:


### Ping
This message requests an Ack from a remote device to test connectivity

* Packet ID: *[00]*
* *Requests: Ack*


>This Packet type does not contain any data fields


------




### Ack
Acknowledges any packet that does not have an explicit response

* Packet ID: *[01]*
* *Responds To: Any Packet without a defined response*


>This Packet type does not contain any data fields


------




### SendCmd
Message to set command in node

* Packet ID: *[02]*

|***Byte***|0|
|---|---|
|***Field***<td colspan='1'>***cmd***
|***Type***<td colspan='1'>uint8_t


Fields:
>***cmd*** : <br/>
>> **0x00** : LED_ON - turns on led<br/>
>> **0x01** : LED_OFF - turns off led<br/>
>> **0x02** : RESET - resets the device<br/>
>

------




### GetData


* Packet ID: *[03]*
* *Requests: Data*


>This Packet type does not contain any data fields


------




### Data
Message containing data from sensor

* Packet ID: *[04]*
* *Responds To: GetData*

|***Byte***|0|1|2|3|4|5|6| . . . . . . . |37
|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>sensorA<td colspan='4'>sensorB<td colspan='4'>sensorName
|***Type***<td colspan='2'>int16_t<td colspan='4'>int<td colspan='4'>char[32]


Fields:
>***sensorA*** : Value of Sensor A<br/>
>***sensorB*** : Value of Sensor B<br/>
>***sensorName*** : Name of sensor responding to message <br/>

------


