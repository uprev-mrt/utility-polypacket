PolyPacket
==========

This Module Contains the back-end C code for protocols and services generated using the PolyPacket tool.

To generate services for this module, install the PolyPacket Tool:

.. code-block:: bash

    pip3 install polypacket


Packing
-------

This section describes how packets are serialized. Each packet contains a header and an optional data section. If a packet contains no fields (for instance an Ack) there is no data section and the Data Len is 0.


Header
~~~~~~

+----------+------------+------------+------------+------------+------------+------------+------------+------------+
| Byte     | 0          | 1          | 2          | 3          | 4          | 5          | 6          | 7          |                
+==========+============+============+============+============+============+============+============+============+
| Field    | typeId     | reserved   | Data Len                | Token                   | Checksum                |   
+----------+------------+------------+-------------------------+-------------------------+-------------------------+
| Type     | uint8      | uint8      | uint16                  | uint16                  |uint16                   |   
+----------+------------+------------+-------------------------+-------------------------+-------------------------+




When a packet contains fields, the fields are serialized as field blocks and placed in the data section.

Field Block
~~~~~~~~~~~

Simple (single value) fields contain a typeId and the value. The parser determines the type of the value by looking up the typeId in the field descriptor dictionary


+----------+------------+--------------------------------+------------+------------+------------+
| Byte     | 0          | 1: 1+ (n/127)                  | *          |*           |     *      |    
+==========+============+================================+============+============+============+
| Field    | typeId     | Array Len (n)                  | Value[0]   | Value[1]   | Value[2]   |
+----------+------------+--------------------------------+------------+------------+------------+
| Type     | uint8      | varsize                        | DataType   | DataType   | DataType   |
+----------+------------+--------------------------------+------------+------------+------------+



If the field is an array/ string, it contains a Length and all of the values present in the array. The Length indicates the number of values present in the array. Again we get the size of each value by looking up the typeId in the field descriptor dictionary


varSize
~~~~~~~

The varSize type stores a number between 0 and 2^28, but uses the least amount of bytes required. each byte contains 7 value bits, and one 'continue' bit. to read the value, you shift in the lower 7 bits, if the highest bit is set, then the value is continued on the second byte. This repeats until you get a 0 for the 'continue' bit. 

.. code-block:: c

    /*    Variable Size value packing
    *    These functions are used for packing and reading variable sized values
    *    This allows effecient packing of small values with the flexibility to still use larger values (up to 2^28). anything under 7bits is not affected
    *    each packed byte represents 7bits of the value, the most signifacant bit is used to indicate if the value is continued on the next byte
    *    example 0x0321 would be packed to [0xA1, 0x06]
    *            0X21 & 0X80 = 0XA1
    *            0x03 << 1 = 0x06 //We shift one bit for each byte to compensate for the bit used as the continuation flag
    */

    int poly_var_size_pack(uint32_t val, uint8_t* buf)
    {
    uint8_t  tmp = 0;
    int idx =0;

    do{
        tmp = val & 0x7F;
        val >>= 7;
        if(val > 0)
        {
        tmp |= 0x80;
        }

        buf[idx++] = tmp;

    } while(val > 0);

    return idx;
    }

