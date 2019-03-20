#!/usr/bin/python3
#
#@file make_protocol.py
#@brief python script to generate code for PolyPacket
#@author Jason Berger
#@date 02/19/2019
#

import sys
import xml.etree.ElementTree as ET
import re
import io
import copy
import datetime
import zlib
import argparse
from mako.template import Template

args = None
parser = None

now = datetime.datetime.now()
path ="./"


sizeDict = {
    "uint8" : 1,
    "int8" : 1,
    "char" : 1,
    "string" : 1,
    "uint16" : 2,
    "int16" : 2,
    "uint32" : 4,
    "int32" : 4,
    "int64" : 8,
    "uint64" : 8,
    "int" : 4,
    "float": 4,
    "double": 8,
}

cNameDict = {
    "uint8" : "uint8_t",
     "int8" : "int8_t",
     "char" : "char",
     "string" : "char",
     "uint16" : "uint16_t",
     "int16" : "int16_t",
     "uint32" : "uint32_t",
     "int32" : "int32_t",
     "int64" : "int64_t",
     "uint64" : "uint64_t",
     "int" : "int",
     "float" : "float",
     "double" : "double",
 }

formatDict = {
    "hex" : "FORMAT_HEX",
    "dec" : "FORMAT_DEC",
    "default" : "FORMAT_DEFAULT",
    "ascii" : "FORMAT_ASCII",
    "none" : "FORMAT_NONE"
}


metaDoc = "### Header\n" +\
          "Every Packet has a standard Header before the data\n\n" +\
          "|***Byte***|0|1|2|3|4|5|6|\n" +\
          "|---|---|---|---|---|---|---|---|\n" +\
          "|***Field***<td colspan=\'1\'>***Id***<td colspan=\'2\'>***Len***<td colspan=\'2\'>***Token***<td colspan=\'2\'>***Checksum***\n" +\
          "|***Type***<td colspan=\'1\'>uint8<td colspan=\'2\'>uint16<td colspan=\'2\'>uint16<td colspan=\'2\'>uint16\n\n" +\
          ">***Id*** : The ID used to identify the type of packet, packet Ids are assigned and managed automatically<br/>\n" +\
          ">***Len*** : This is the len of the packet data, this does not include the header and checksum<br/>\n" +\
          ">***Token*** : A unique randomly generated token. Each packet is tokenized to provide functions like ack/retry and preventing duplicates <br/>\n" +\
          ">***Checksum*** : A calculated checksum of the data in the packet\n" +\
          "----\n"

def crc(fileName):
    prev = 0
    for eachLine in open(fileName,"rb"):
        prev = zlib.crc32(eachLine, prev)
    return "%X"%(prev & 0xFFFFFFFF)


class fieldDesc:
    def __init__(self, name, strType):
        self.enums = []
        self.arrayLen = 1
        self.isEnum = False;

        m = re.search('\[([0-9]*)\]', strType)
        if(m):
            if(m.group(1) != ''):
                self.arrayLen = int(m.group(1))
            strType = strType[0:m.start()]

        #check if its an enum
        m = re.search('enum\[(.*)\]', strType)
        if(m):
            strEnums = m.group(1)
            if(strEnums != ''):
                self.enums = [x.strip() for x in strEnums.split(',')]
                self.isEnum = True
            strType = 'uint8'



        self.type = strType.lower().replace('_t','')

        if not (self.type in cNameDict):
            print( "INVALID DATA TYPE!:  " + type)

        self.size = sizeDict[self.type] * self.arrayLen
        self.cType = cNameDict[self.type]
        self.cppType = self.cType

        self.isString = False
        self.isArray = False

        if(self.arrayLen > 1):
            self.isArray = True

        if(self.type == 'string'):
            self.cppType = "string"
            self.isString = True
        else:
            if(self.isArray):
                self.cppType = self.cppType +"*"

        self.id = 0
        self.name = name
        self.globalName = "PP_"+self.name.upper()+"_FIELD"
        self.isVarLen = False
        self.format = 'FORMAT_DEFAULT'
        self.isRequired = False
        self.desc = ""
        self.memberName = "m"+ self.name.capitalize()

    def setPrefix(self, prefix):
        self.globalName = prefix.upper()+"_"+self.name.upper()+"_FIELD"

    def getFieldDeclaration(self):
        output = io.StringIO()
        output.write("{0} {1}".format(self.cType, self.memberName))
        if(self.arrayLen > 1):
            output.write("["+str(self.arrayLen)+"]")

        if(self.desc != ""):
            output.write(";\t//"+self.desc +"")
        else:
            output.write(";")
        return output.getvalue()

    def getParamType(self):
        if self.isArray:
            return self.cType +"*"
        else:
            return self.cType;

class packetDesc:
    def __init__(self, name):
        self.name = name
        self.globalName =  "PP_"+name.upper()+"_PACKET"
        self.className = name.capitalize() +"Packet"
        self.desc =""
        self.fields = []
        self.fieldCount=0
        self.respondsTo = {}
        self.requests = {}
        self.standard = False
        self.structName = name.lower() + '_packet_t'

    def setPrefix(self, prefix):
        self.globalName = prefix.upper()+"_"+self.name.upper()+"_PACKET"

    def addField(self, field):
        field.id = self.fieldCount
        self.fields.append(field)
        self.fieldCount+=1

    def getDocMd(self):
        output = io.StringIO()
        output.write('### ' + self.name + '\n')
        output.write(self.desc + '\n\n')
        requestCount = len(self.requests)
        respondsToCount = len(self.respondsTo)

        #write response packets
        if(requestCount > 0):
            output.write('* *Requests: ')
            first = True
            for req in self.requests:
                if(first):
                    first = False
                else:
                    output.write(', ')
                output.write(req)
            output.write('*\n\n')

        #write request packets
        if(respondsToCount > 0):
            output.write('* *Responds To: ')
            first = True
            for resp in self.respondsTo:
                if(first):
                    first = False
                else:
                    output.write(', ')
                output.write(resp)
            output.write('*\n\n')

        rowBytes = io.StringIO()
        rowBorder = io.StringIO()
        rowFields = io.StringIO()
        rowTypes = io.StringIO()

        rowBytes.write('|***Byte***|')
        rowBorder.write('|---|')
        rowFields.write('|***Field***')
        rowTypes.write('|***Type***')

        count =0

        for pfield in self.fields:

            #write bytes
            if(pfield.size > 4):
                rowBytes.write(str(count)+'| . . . . . . . |'+str(count+pfield.size -1))
                count+=pfield.size
            else:
                for x in range(pfield.size):
                    rowBytes.write(str(count) + '|')
                    count+=1

            #write border
            span = pfield.size
            if(span > 4):
                span = 4
            for x in range(span):
                rowBorder.write('---|')

            #write fields
            span = pfield.size
            if(span > 4):
                span = 4
            rowFields.write('<td colspan=\''+str(span)+'\'>')
            if(pfield.isRequired):
                rowFields.write('***'+pfield.name+'***')
            else:
                rowFields.write(pfield.name)

            #write types
            span = pfield.size
            if(span > 4):
                span = 4
            rowTypes.write('<td colspan=\''+str(span)+'\'>')
            rowTypes.write(pfield.cType)
            if(pfield.isArray):
                if(pfield.isVarLen):
                    rowTypes.write('[0-'+ str(pfield.size)+' ]')
                else:
                    rowTypes.write('['+str(pfield.size)+']')

        #combine rows for table
        output.write(rowBytes.getvalue() + "\n");
        output.write(rowBorder.getvalue() + "\n");
        output.write(rowFields.getvalue() + "\n");
        output.write(rowTypes.getvalue() + "\n");

        output.write('\n\n')

        #write field description table
        for pfield in self.fields:
            output.write('>***'+ pfield.name+'*** : ' + pfield.desc +'<br/>\n')
        output.write('\n------\n')

        return output.getvalue();









class protocolDesc:
    def __init__(self, name):
        self.name = name
        self.fileName = name
        self.desc = ""
        self.hash = ""
        self.fields = []
        self.fieldIdx = {}
        self.fieldId =0
        self.packets = []
        self.packetIdx ={}
        self.packetId =0
        self.prefix = "pp";

    def service(self):
        return self.prefix.upper() +'_SERVICE'

    def addField(self,field):
        field.id = self.fieldId
        self.fields.append(field)
        self.fieldIdx[field.name] = self.fieldId
        self.fieldId+=1


    def addPacket(self,packet):
        packet.packetId = self.packetId
        packet.setPrefix(self.prefix)
        self.packets.append(packet)
        self.packetIdx[packet.name] = self.packetId
        self.packetId+=1


def addStandardPackets(protocol):
    ack = packetDesc("ack")
    ack.standard = True
    protocol.addPacket(ack)


def parseXML(xmlfile):

    # create element tree object
    tree = ET.parse(xmlfile)

    # get root element
    root = tree.getroot()

    # create empty list for Fields
    protocol = protocolDesc(root.attrib['name'])


    if('desc' in root.attrib):
        protocol.desc = root.attrib['desc']

    if('prefix' in root.attrib):
        protocol.prefix = root.attrib['prefix']

    addStandardPackets(protocol)

    #parse out fields
    for field in root.findall('./Fields/Field'):

        name = field.attrib['name']
        strType = field.attrib['type'];


        newField = fieldDesc(name, strType)
        newField.setPrefix(protocol.prefix)

        if('format' in field.attrib):
            format = field.attrib['format'].lower()
            if not format in formatDict:
                print( "INVALID FORMAT :" + format)

            newField.format = formatDict[format]

        if('desc' in field.attrib):
            newField.desc = field.attrib['desc']

        if(name in protocol.fields):
            print( 'ERROR Duplicate Field Name!: ' + name)

        protocol.addField(newField)


    #get all packet types
    for packet in root.findall('./Packets/Packet'):
        name = packet.attrib['name']
        desc =""
        newPacket = packetDesc(name)
        newPacket.setPrefix(protocol.prefix)

        if(name in protocol.packetIdx):
            print( 'ERROR Duplicate Packet Name!: ' + name)

        if('desc' in packet.attrib):
            desc = packet.attrib['desc']

        if('response' in packet.attrib):
            newPacket.requests[packet.attrib['response']] = 0

        #get all fields declared for packet
        for pfield in packet:

            pfname = pfield.attrib['name']
            strReq =""
            if not (pfname in protocol.fieldIdx):
                print( 'ERROR Field not declared: ' + pfname)

            #get id of field and make a copy
            idx = protocol.fieldIdx[pfname]
            fieldCopy = copy.deepcopy(protocol.fields[idx])

            if('req' in pfield.attrib):
                strReq = pfield.attrib['req']
                if(strReq.lower() == "true" ):
                    fieldCopy.isRequired = True

            if('desc' in pfield.attrib):
                fieldCopy.desc = pfield.attrib['desc']

            newPacket.addField(fieldCopy)
            newPacket.desc = desc

        protocol.addPacket(newPacket)


    for packet in protocol.packets:
        for request in packet.requests:
            idx = protocol.packetIdx[request]
            protocol.packets[idx].respondsTo[packet.name] = 0

    # return news items list
    return protocol


def buildTemplate(protocol, templateFile, outputFile):
    template = Template(filename= templateFile )
    text_file = open( outputFile , "w")
    text_file.write("\n".join(template.render(proto = protocol).splitlines()))
    #text_file.write(template.render(proto = protocol))
    text_file.close()

def createDoc(protocol, filename):
    global path
    output = io.StringIO()
    output.write('# ' + protocol.name + '\n')
    output.write('* Generated: '+now.strftime("%m/%d/%y")+'<br/>\n')
    output.write('* CRC: '+protocol.hash+'\n\n')
    output.write('##### ' + protocol.desc + '\n\n')
    output.write('----\n')
    output.write(metaDoc +'')
    output.write('# Packet Types:\n\n')



    for packet in protocol.packets:
        output.write(packet.getDocMd())




    text_file = open(filename, "w")
    text_file.write(output.getvalue())
    text_file.close()

# Initialize the argument parser
def init_args():
    global parser
    parser = argparse.ArgumentParser("Tool to generate code and documentation for PolyPacket protocol")
    parser.add_argument('-i', '--input', type=str, help='Xml file to parse', required=True)
    parser.add_argument('-o', '--output', type=str, help='Output path', default="")
    parser.add_argument('-c', '--pure_c', action='store_true', help='generate pure c code', default=False)
    parser.add_argument('-d', '--document', action='store_true', help='Enable documentation', default=False)

def main():
    global path
    global parser
    global args

    init_args()
    args= parser.parse_args()
    argCount = len(sys.argv)

    xmlFile = args.input
    path = args.output

    fileCrc = crc(xmlFile)


    protocol = parseXML(xmlFile)
    protocol.hash = fileCrc

    if(args.pure_c):
        buildTemplate(protocol, 'templates/c_header_template.h', path+"/" + protocol.fileName+".h")
        buildTemplate(protocol, 'templates/c_source_template.c', path+"/" + protocol.fileName+".c")
    else:
        buildTemplate(protocol, 'templates/cpp_header_template.h', path+"/" + protocol.fileName+".hpp")
        buildTemplate(protocol, 'templates/cpp_source_template.cpp', path+"/" + protocol.fileName+".cpp")



    if(args.document):
        createDoc(protocol,path+"/" + protocol.fileName+".md")

if __name__ == "__main__":
    main()
