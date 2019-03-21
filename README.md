# PolyPacket

Poly Packet is backend and code generation tool for creating messaging protocols and modeled objects. Protocols are described in an XML document which can be easily shared with all components of a system. A python script is used to parse the XML file and generate code as well as documentation.

## Protocol Generation

Protocols are generated using XML. The messaging structure is made up of two types:

* Fields
* Packets

## Fields
 A field is a data object within a message


example field:

```
<Field name="src" type="uint16_t" format="hex" desc="Source address of message" />
```
> **name**: The name of the field <br/>
> **type**: The data type for the field  <br/>
> **format**: (optional)  This sets the display format used for the toString and toJsonString methods [ hex , dec , assci ]  <br/>
> **desc**: (optional)  The description of the field. This is used to create the documentation  <br/>

## Packets
A Packet describes an entire message and is made up of fields

example Packet:

```
<Packet name="GetData" desc="Message to get data from node" response="RespData">
  <Field name="src" req="true"/>
  <Field name="dst" req="true" desc="address of node to retrieve data from"/>
</Packet>
```

> **name**: The name of the packet <br/>
> **desc**: (optional)  description of the packet for documentation <br/>
> **response**: (optional) name of the packet type expected in response to this message (if any)

within the packet we reference Fields which have already been declared in the Fields section. these references contain 3 attributes:

> **name**: The name of the field<br/>
> **req**: (optional)  makes the field a requirement for this packet type <br/>
> **desc**: (optional) description of this field for this packet type, will override fields description in the documentation for this packet type only


## Example:

The following example show the XML for a simple message protocol for a lighting system. In the system there are several nodes which have 16 bit addresses. Each node contains two values, a friendly name, an a light value.


```
<Protocol name="SampleProtocol"
  desc="This is a sample protocol made up to demonstrate features of the PolyPacket code generation tool. The idea
  is to have a tool that can automatically create parseable/serializable messaging for embedded systems.">
  <!--First we declare all Field descriptors-->
  <Fields>
    <!--Common -->
    <Field name="src" type="uint16_t" format="hex" desc="Source address of message" />
    <Field name="dst" type="uint16_t" format="hex" desc="Desitination address of message"/>
    <Field name="name" type="string[16]" desc="friendly name of node" />
    <Field name="light" type="uint8_t" format="dec" />

  </Fields>
  <!--Declare all Packet Types-->
  <Packets>

    <!-- Set Message -->
    <Packet name="SetData" desc="Message to set data in node" response="RespData">
      <Field name="src" req="true"/>
      <Field name="dst" req="true"/>
      <Field name="name"/>
      <Field name=light />
    </Packet>

    <!-- Get Message -->
    <Packet name="GetData" desc="Message to get data from node" response="RespData">
      <Field name="src" req="true"/>
      <Field name="dst" req="true"/>
    </Packet>

    <!-- Response Message -->
    <Packet name="RespData" desc="Message to respond to get/set message" >
      <Field name="src" req="true"/>
      <Field name="dst" req="true"/>
      <Field name="name"/>
      <Field name=light />
    </Packet>

  </Packets>
</Protocol>

```
The XML set up 3 Fields:

**src** - source address where message </br>
**dst** - destination where message will be sent <br/>
**name** - Friendly name of node <br/>
**light** - light value of node <br/>

## Using Poly Packet

To use poly packet, write your xml to define the fields and packets in your protocol. The use the make_protocol.py python script to generate the source code.
>the mako module is required (pip install mako)
```
python3 make_protocol.py -i sample_protocol.xml -o ../example -c
```
* -i is for input file, this will be the xml file used
* -o is the output directory, this is where the code and documentation will be generated
* -c tells the script to generate pure C code for embedded

>all functions will start with the prefix 'pp'. but the 'prefix' attribute can be used in the xml to set a different prefix. this allows the use of multiple services/protocols in a single project without conflict

Code Example:
```
#include "SampleProtocol.h"

uint8_t buffer[1024];
char printBuf[1024];



//Handlers for packets are declared weak in service
HandlerStatus_e sp_SetData_handler(sp_packet_t * packet)
{
  // do something
  sp_print_json(msg, printBuf);
  sprintf("Handled: %s",printBuf);
  return PACKET_HANDLED;
}

//mock uart receive handler
void platform_uart_handler(uint8_t* data ,int len)
{
  sp_service_feed(0, data, len); // feed received byte into interface 0 of service
}

//mock function to mimic sending data over uart
void platform_uart_send(uint8_t* data, int len)
{

}

int main()
{
  sp_service_init(1); //initialize the service with 1 interface

  sp_packet_t* msg = new_sp_packet(SP_SETDATA_PACKET);  //creates a new message (must be cleaned when done)

  //set values in message
  sp_setSrc(msg,0xABCD );
  sp_setDst(msg,0xCDEF);
  sp_setSensora(msg,32500);
  sp_setSensorb(msg,898989);
  sp_setSensorname(msg, "This is my test string");

  sp_service_register_tx(0, &platform_uart_send ); // register sending function


  sp_send(0,msg); // Send message over interface 0


  sp_clean(msg); //clean when done


  while(1)
  {
    sp_service_process();
  }

  return 0;
}
```

This example shows how to use the code to create a service. The service is initialized with 1 interface:

### Initializing service

```
sp_service_init(1); //initialize the service with 1 interface
```
For devices where multiple hardware ports are being used by the same protocol, you can use more interfaces

---
### Creating a message

This creates a new message with the packet type 'SetData' defined in our xml
```
sp_packet_t* msg = new_sp_packet(SP_SETDATA_PACKET);
```

next we set fields in the message

```
sp_setSrc(msg,0xABCD );
sp_setDst(msg,0xCDEF);
```

---
### Register Tx functions

For each interface we can register a send function this allows us to automate things like acknowledgements

```
sp_service_register_tx(0, &platform_uart_send ); // register sending function
```
once we have registered a callback for an interface, we can send messages to it
```
sp_send(0,msg)
```

---

### Receive Handlers
The generated service creates a handler for each packet type, they are created with weak attributes, so they can be overridden by just declaring them again in our code.

The following is our handler for 'SetData' type packets

```
HandlerStatus_e sp_SetData_handler(sp_packet_t * packet)
{
  //do something with received packet
}
```

### Process

The service is meant to be run on many platforms, so it does not have built in threading/tasking. For it to continue handling messages, we have to call its process function either in a thread/task or in our super-loop

```
while(1)
{
  sp_service_process();
}
```
