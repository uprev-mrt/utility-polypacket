# SampleProtocol
* Generated: 02/26/19<br/>
* CRC: D06ABEBB

##### This is a sample protocol made up to demonstrate features of the PolyPacket code generation tool. The idea   is to have a tool that can automatically create parseable/serializable messaging for embedded systems.

----
### Header
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

### SetData
Message to set data in node

* *Requests: RespData*

|***Byte***|0|1|2|3|4|5|6|7|8|9|10| . . . . . . . |41
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>***src***<td colspan='2'>***dst***<td colspan='2'>sensorA<td colspan='4'>sensorB<td colspan='4'>sensorName
***Type***<td colspan='2'>uint16_t<td colspan='2'>uint16_t<td colspan='2'>int16<td colspan='4'>int<td colspan='4'>char[32]


>***src*** : Source address of message<br/>
>***dst*** : Desitination address of message<br/>
>***sensorA*** : Value of Sensor A<br/>
>***sensorB*** : Value of Sensor B<br/>
>***sensorName*** : Name of sensor<br/>

------
### GetData
Message to get data from node

* *Requests: RespData*

|***Byte***|0|1|2|3|4|5|6|7|8|9|10| . . . . . . . |41
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>***src***<td colspan='2'>***dst***<td colspan='2'>sensorA<td colspan='4'>sensorB<td colspan='4'>sensorName
***Type***<td colspan='2'>uint16_t<td colspan='2'>uint16_t<td colspan='2'>int16<td colspan='4'>int<td colspan='4'>char[32]


>***src*** : Source address of message<br/>
>***dst*** : Desitination address of message<br/>
>***sensorA*** : Value of Sensor A<br/>
>***sensorB*** : Value of Sensor B<br/>
>***sensorName*** : Name of sensor<br/>

------
### RespData
Response to get/set messages

* *Responds To: GetData, SetData*

|***Byte***|0|1|2|3|4|5|6|7|8|9|10| . . . . . . . |41
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>***src***<td colspan='2'>***dst***<td colspan='2'>sensorA<td colspan='4'>sensorB<td colspan='4'>sensorName
***Type***<td colspan='2'>uint16_t<td colspan='2'>uint16_t<td colspan='2'>int16<td colspan='4'>int<td colspan='4'>char[32]


>***src*** : Source address of message<br/>
>***dst*** : Desitination address of message<br/>
>***sensorA*** : Value of Sensor A<br/>
>***sensorB*** : Value of Sensor B<br/>
>***sensorName*** : Name of sensor<br/>

------
### blockReq
This packet is used to request a block of data from the host during Ota updates

* *Requests: blockResp*

|***Byte***|0|1|2|3|4|5|6|7|8|9|10|11|
|---|---|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>***src***<td colspan='2'>***dst***<td colspan='4'>***blockOffset***<td colspan='4'>***blockSize***
***Type***<td colspan='2'>uint16_t<td colspan='2'>uint16_t<td colspan='4'>uint32_t<td colspan='4'>uint32_t


>***src*** : Source address of message<br/>
>***dst*** : Desitination address of message<br/>
>***blockOffset*** : Offset of block being requested<br/>
>***blockSize*** : Size of block being requested <br/>

------
### blockResp
This packet sends a block of ota data to the node as a response to a block request

* *Responds To: blockReq*

|***Byte***|0|1|2|3|4|5|6|7|8|9|10|11|12| . . . . . . . |75
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|***Field***<td colspan='2'>***src***<td colspan='2'>***dst***<td colspan='4'>***blockOffset***<td colspan='4'>***blockSize***<td colspan='4'>***blockData***
***Type***<td colspan='2'>uint16_t<td colspan='2'>uint16_t<td colspan='4'>uint32_t<td colspan='4'>uint32_t<td colspan='4'>uint8_t[64]


>***src*** : Source address of message<br/>
>***dst*** : Desitination address of message<br/>
>***blockOffset*** : Offset of block in memory<br/>
>***blockSize*** : size of memory block<br/>
>***blockData*** : actual data from memory<br/>

------
