# Minimal
* Generated: 03/24/19<br/>
* CRC: A0CADB6E

-----

Every Packet has a standard Header before the data

|***Byte***|0|1|2|3|4|5|6|
|---|---|---|---|---|---|---|---|
|***Field***<td colspan='1'>***Id***<td colspan='2'>***Len***<td colspan='2'>***Token***<td colspan='2'>***Checksum***
|***Type***<td colspan='1'>uint8<td colspan='2'>uint16<td colspan='2'>uint16<td colspan='2'>uint16

>***Id*** : The ID used to identify the type of packet, packet Ids are assigned and managed automatically<br/>
>***Len*** : This is the length of the packet data, this does not include the header and checksum<br/>
>***Token*** : A unique randomly generated token. Each packet is tokenized to provide functions like ack/retry and preventing duplicates <br/>
>***Checksum*** : A calculated checksum of the data in the packet
----
# Packet Types:


### AmbientData
Message containing sensor data

* *Responds To: RequestAmbient*

|***Byte***|0|1|2|3|
|---|---|---|---|---|
|***Field***<td colspan='4'>***ambient***
|***Type***<td colspan='4'>int


>***ambient*** : value from ambient light sensor<br/>

------
