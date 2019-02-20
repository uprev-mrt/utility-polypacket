# SampleProtocol

## Packet Types:


------
### blockResp
This packet sends a block of ota data to the node as a response to a block request

|***Field***|src|dst|blockData|blockSize|blockOffset|
|---|---|---|---|---|---|
|***Type***|uint16_t|uint16_t|uint8_t[64]|uint32_t|uint32_t|

***src*** : Source address of message<br/>
***dst*** : Desitination address of message<br/>
***blockData*** : actual data from memory<br/>
***blockSize*** : size of memory block<br/>
***blockOffset*** : Offset of block in memory<br/>

------
### Data
Data messages can be used to get data, set data, or respond to a get data message depending on the cmd field

|***Field***|src|sensorB|sensorName|sensorA|dst|cmd|
|---|---|---|---|---|---|---|
|***Type***|uint16_t|int|char[n*]|int16|uint16_t|uint8_t|

***src*** : Source address of message<br/>
***sensorB*** : Value of Sensor B<br/>
***sensorName*** : Name of sensor<br/>
***sensorA*** : Value of Sensor A<br/>
***dst*** : Desitination address of message<br/>
***cmd*** : <br/>

------
### blockReq
This packet is used to request a block of data from the host during Ota updates

|***Field***|src|dst|blockSize|blockOffset|
|---|---|---|---|---|
|***Type***|uint16_t|uint16_t|uint32_t|uint32_t|

***src*** : Source address of message<br/>
***dst*** : Desitination address of message<br/>
***blockSize*** : Size of block being requested <br/>
***blockOffset*** : Offset of block being requested<br/>
