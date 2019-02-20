#!/usr/bin/python
#
#@file make_protocol.py
#@brief python script to generate code for PolyPacket
#@author Jason Berger
#@date 02/19/2019
#

import sys
import xml.etree.ElementTree as ET
import re
import StringIO

class fieldDesc:
    def __init__(self, name, type):
        self.id = 0
        self.name = name
        self.type = type
        self.isArray = False
        self.format = 'default'
        self.arrayLen = 0

class packetDesc:
    def __init__(self, name):
        self.name = name
        self.fieldIds = []

class protocolDesc:
    def __init__(self, name):
        self.name = name
        self.fields = []
        self.packets = []

def parseXML(xmlfile):

    # create element tree object
    tree = ET.parse(xmlfile)

    # get root element
    root = tree.getroot()

    # create empty list for Fields
    protocol = protocolDesc(root.attrib['name'])

    # iterate field items
    count = 0;

    #parse out fields
    for field in root.findall('./Fields/Field'):

        strType = field.attrib['type'];
        arrayLen = 0
        array = False

        #see if its an array
        m = re.search('\[([0-9]*)\]', strType)
        if(m):
            array = True
            arrayLen = int(m.group(1))
            strType = strType[0:m.start()]

        newField = fieldDesc(field.attrib['name'], strType)
        fieldDesc.id = count
        count+=1

        if(array):
            newField.isArray = True
            newField.arrayLen = arrayLen

        if('format' in field.attrib):
            newField.format = field.attrib['format']

        protocol.fields.append(newField)

    for packet in root.findall('./Packets/Packet'):
        


    # return news items list
    return protocol

def createHeader(protocol):
    output = StringIO.StringIO()
    output.write('/**\n')
    output.write('  *@file var_packet.h\n')
    output.write('  *@brief generated protocol source code\n')
    output.write('  *@author make_protocol.py\n')
    output.write('  *@date 02/19/2019\n')
    output.write('  */\n\n')

    output.write('#include \"var_field.h\"\n')
    output.write('#include \"var_packet.h\"\n\n\n')

    output.write('//Declare extern field descriptors\n')
    for field in protocol.fields:
        output.write('extern field_desc_t* VF_' + field.name.upper()+ ';\n')

    output.write('\n\n')
    output.write('//Declare extern packet descriptors\n')
    for field in protocol.packets:
        output.write('extern packet_desc_t* VP_' + field.packet.upper()+ ';\n')



    print output.getvalue()







def main():
    # print command line arguments
    for arg in sys.argv[1:]:
        print arg

    xmlFile = sys.argv[1]
    protocol = parseXML(xmlFile)
    createHeader(protocol)

if __name__ == "__main__":
    main()
