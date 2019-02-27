# PolyPacket

Poly Packet is backend and code generation tool for creating messaging protocols and modeled objects. Protocols are described in an XML document which can be easily shared with all components of a system. A python script is used to parse the XML file and generate code as well as documentation.

## Protocol Generation

Protocols are generated using XML. The messaging structure is made up of two type:

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

Then it describes all of the message type:
