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
import copy
import datetime
import zlib

now = datetime.datetime.now()
path ="./"

sizeDict = {
    "uint8" : 1,
    "int8" : 1,
    "char" : 1,
    "uint16" : 2,
    "int16" : 2,
    "uint32" : 4,
    "int32" : 4,
    "int" : 4,
    "float": 4,
    "double": 8
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
    def __init__(self, name, type):
        self.id = 0
        self.name = name
        self.type = type
        self.isArray = False
        self.isVarLen = False
        self.format = 'default'
        self.arrayLen = 1
        self.size = 1
        self.isRequired = False
        self.desc = ""

class packetDesc:
    def __init__(self, name):
        self.name = name
        self.desc =""
        self.fields = []
        self.fieldCount=0
        self.respondsTo = {}
        self.requests = {}

    def addField(self, field):
        field.id = self.fieldCount
        self.fields.append(field)
        self.fieldCount+=1

    def getDocMd(self):
        output = StringIO.StringIO()
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

        rowBytes = StringIO.StringIO()
        rowBorder = StringIO.StringIO()
        rowFields = StringIO.StringIO()
        rowTypes = StringIO.StringIO()

        rowBytes.write('|***Byte***|')
        rowBorder.write('|---|')
        rowFields.write('|***Field***')
        rowTypes.write('***Type***')

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
            rowTypes.write(pfield.type)
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

    def getStructName(self):
        return self.name.lower() + "_struct_t"

    def getStruct(self):
        output = StringIO.StringIO()
        output.write("//Struct for "+ self.name+" Packet\n")
        output.write("//"+ self.desc+" Packet\n")
        output.write("typedef struct{\n")
        for field in self.fields:
            output.write("  "+field.type+" m"+field.name)
            if(field.arrayLen > 1):
                output.write("["+str(field.arrayLen)+"]")

            if(field.desc != ""):
                output.write(";\t//"+field.desc +"\n")
            else:
                output.write(";\n")


        output.write("} " + self.name.lower() + "_struct_t;\n\n")
        output.write(self.name.lower() + "_struct_t* new_"+self.name.lower() +"();\n\n")

        return output.getvalue()

    def getStructConstructor(self):
        output = StringIO.StringIO()
        output.write(self.name.lower() + "_struct_t* new_"+self.name.lower() +"()\n{\n")





class protocolDesc:
    def __init__(self, name):
        self.name = name
        self.desc = ""
        self.hash = ""
        self.fields = []
        self.fieldIdx = {}
        self.fieldId =0
        self.packets = []
        self.packetIdx ={}
        self.packetId =0

    def addField(self,field):
        field.id = self.fieldId
        self.fields.append(field)
        self.fieldIdx[field.name] = self.fieldId
        self.fieldId+=1

    def addPacket(self,packet):
        self.packets.append(packet)
        self.packetIdx[packet.name] = self.packetId
        self.packetId+=1

def parseXML(xmlfile):

    # create element tree object
    tree = ET.parse(xmlfile)

    # get root element
    root = tree.getroot()

    # create empty list for Fields
    protocol = protocolDesc(root.attrib['name'])

    if('desc' in root.attrib):
        protocol.desc = root.attrib['desc']


    #parse out fields
    for field in root.findall('./Fields/Field'):

        name = field.attrib['name']
        strType = field.attrib['type'];

        if(strType == 'string'):
            strType = 'char[]'
        arrayLen = 0
        array = False

        #see if its an array
        m = re.search('\[([0-9]*)\]', strType)
        if(m):
            array = True
            if(m.group(1) != ''):
                arrayLen = int(m.group(1))
            strType = strType[0:m.start()]


        newField = fieldDesc(name, strType)

        strType = strType.replace('_t','')

        if not (strType in sizeDict):
            print "Unsupported type: " + strType

        newField.size = sizeDict[strType]

        if(array):
            newField.isArray = True
            if(arrayLen>0):
                newField.arrayLen = arrayLen
            else:
                newField.isVarLen = True
                if('max-len' in field.attrib):
                    newField.arrayLen = int(field.attrib['max-len'])
                else:
                    newField.arrayLen = 255

            newField.size = newField.arrayLen


        if('format' in field.attrib):
            newField.format = field.attrib['format']

        if('desc' in field.attrib):
            newField.desc = field.attrib['desc']

        if('max-len' in field.attrib):
            newField.arrayLen = field.attrib['max-len']

        if(name in protocol.fields):
            print 'ERROR Duplicate Field Name!: ' + name

        protocol.addField(newField)


    #get all packet types
    for packet in root.findall('./Packets/Packet'):
        name = packet.attrib['name']
        desc =""
        newPacket = packetDesc(name)

        if(name in protocol.packetIdx):
            print 'ERROR Duplicate Packet Name!: ' + name

        if('desc' in packet.attrib):
            desc = packet.attrib['desc']

        if('response' in packet.attrib):
            newPacket.requests[packet.attrib['response']] = 0

        #get all fields declared for packet
        for pfield in packet:

            pfname = pfield.attrib['name']
            strReq =""
            if not (pfname in protocol.fieldIdx):
                print 'ERROR Field not declared: ' + pfname

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

def createHeaderC(protocol):
    global path
    output = StringIO.StringIO()
    output.write('/**\n')
    output.write('  *@file '+protocol.name +'.h\n')
    output.write('  *@brief generated protocol source code\n')
    output.write('  *@author make_protocol.py\n')
    output.write('  *@date '+now.strftime("%m/%d/%y")+'\n')
    output.write('  */\n\n')
    output.write('/***********************************************************\n')
    output.write('        THIS FILE IS AUTOGENERATED. DO NOT MODIFY\n')
    output.write('***********************************************************/\n')

    output.write('#include \"var_field.h\"\n')
    output.write('#include \"var_packet.h\"\n\n\n')

    #write packet descriptors
    output.write('//Declare extern packet descriptors\n')
    for packet in protocol.packets:
        output.write('extern packet_desc_t* VP_' + packet.name.upper()+ ';\n')

    output.write('\n\n')

    #write field descriptors
    output.write('//Declare extern field descriptors\n')
    for field in protocol.fields:
        output.write('extern field_desc_t* VF_' + field.name.upper()+ ';\n')

    output.write('\n\n')

    #write structs for each packet
    output.write('//Structs for packet types')
    for packet in protocol.packets:
        output.write(packet.getStruct())

    output.write('\n\n')

    output.write('void '+protocol.name+'protocol_init();\n')

    text_file = open(path + protocol.name+"_proto.h", "w")
    text_file.write(output.getvalue())
    text_file.close()



def createSourceC(protocol):
    output = StringIO.StringIO()
    output.write('/**\n')
    output.write('  *@file '+protocol.name +'.c\n')
    output.write('  *@brief generated protocol source code\n')
    output.write('  *@author make_protocol.py\n')
    output.write('  *@date '+now.strftime("%m/%d/%y")+'\n')
    output.write('  */\n\n')
    output.write('***********************************************************\n')
    output.write('        THIS FILE IS AUTOGENERATED. DO NOT MODIFY\n')
    output.write('***********************************************************\n\n')
    output.write('#include \"'+ protocol.name + '.h\"\n\n')

    output.write('//Declare extern field descriptors\n')
    for field in protocol.fields:
        output.write('field_desc_t* VF_' + field.name.upper()+ ';\n')

    output.write('\n\n')
    output.write('//Declare extern packet descriptors\n')
    for packet in protocol.packets:
        output.write('packet_desc_t* VP_' + packet.name.upper()+ ';\n')
    output.write('\n\n\n')

    #init function
    output.write('void protocol_init()\n{\n\n')
    output.write('//Set up Field descriptors\n')
    #fields
    for field in protocol.fields:
        output.write('\tVF_' + field.name.upper()+ '= new_field_desc( \"'+field.name+'\" , sizeof('+ field.type +') , '+str(field.arrayLen)+');\n')

        if(field.isVarLen):
            output.write('\tVF_' + field.name.upper()+ '->mVarLen = true;\n')

        if (field.format != 'default'):
            output.write('\tVF_' + field.name.upper()+ '->mFormat = FORMAT_'+ field.format.upper()+';\n')
        output.write('\n')

    output.write('\n\n\n')
    output.write('//Set up packet descriptors\n')
    for packet in protocol.packets:
        output.write('\tVP_' + packet.name.upper()+ '= new_packet_desc(\"'+ packet.name+'\") ;\n')

        for pfield in packet.fields:
            output.write('\t\tpacket_desc_add_field( VP_'+ packet.name.upper() +', VF_'+ pfield.name.upper()+', '+ str(pfield.isRequired).lower()+');\n')
            #packet_desc_add_field(packet_desc_t* desc, field_desc_t* fieldDesc);
        output.write('\n')
    output.write('}\n\n')

#    text_file = open("Output.c", "w")
#    text_file.write(output.getvalue())
#    text_file.close()

    #print output.getvalue()


def createDoc(protocol):
    global path
    output = StringIO.StringIO()
    output.write('# ' + protocol.name + '\n')
    output.write('* Generated: '+now.strftime("%m/%d/%y")+'<br/>\n')
    output.write('* CRC: '+protocol.hash+'\n\n')
    output.write('##### ' + protocol.desc + '\n\n')
    output.write('----\n')
    output.write(metaDoc +'')
    output.write('# Packet Types:\n\n')



    for packet in protocol.packets:
        output.write(packet.getDocMd())




    text_file = open(path+protocol.name+"_doc.md", "w")
    text_file.write(output.getvalue())
    text_file.close()



def main():

    argCount = len(sys.argv)

    # print command line arguments
    for arg in sys.argv[1:]:
        print arg



    xmlFile = sys.argv[1]
    fileCrc = crc(xmlFile)

    if(argCount >2):
        path = sys.argv[2]

    protocol = parseXML(xmlFile)
    protocol.hash = fileCrc
    createHeaderC(protocol)
    createSourceC(protocol)
    createDoc(protocol)

if __name__ == "__main__":
    main()
