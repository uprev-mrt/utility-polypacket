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
    "string" : 1,
    "uint16" : 2,
    "int16" : 2,
    "uint32" : 4,
    "int32" : 4,
    "int64" : 8,
    "uint64" : 8,
    "int" : 4,
    "float": 4,
    "double": 8
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
    def __init__(self, name, type, len):

        self.type = type.lower().replace('_t','')

        if not (self.type in cNameDict):
            print "INVALID DATA TYPE!:  " + type

        self.size = sizeDict[self.type] * len
        self.cType = cNameDict[self.type]
        self.cppType = self.cType

        self.isString = False
        self.isArray = False

        self.arrayLen=len

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
        self.globalName = "PF_"+self.name
        self.isVarLen = False
        self.format = 'FORMAT_DEFAULT'
        self.isRequired = False
        self.desc = ""

class packetDesc:
    def __init__(self, name):
        self.name = name
        self.globalName = "PP_" + name
        self.className = name.capitalize() +"Packet"
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

    def generateClass(self, base):
        output = StringIO.StringIO()
        output.write("/**********************************************************\n" )
        output.write("              " +self.className+ "                       \n" )
        output.write("**********************************************************/\n" )
        output.write("class "+self.className +" : public "+ base+"\n{\npublic:\n" )
        output.write("  "+self.className +"(poly_packet_t* packet = NULL);\n\n" )

        #generate value getters
        output.write("\n  //Value Getters\n")
        for field in self.fields:
            if(field.cppType == 'string'):
                output.write( "  " +field.cppType +" " +field.name.capitalize()+"() const {return std::string(m"+ field.name.capitalize()+");}\n" )
            else:
                if field.isArray:
                    output.write( "  " +field.cppType +" " +field.name.capitalize()+"() {return m"+ field.name.capitalize()+";}\n" )
                else:
                    output.write( "  " +field.cppType +" " +field.name.capitalize()+"() const {return m"+ field.name.capitalize()+";}\n" )


        #generate value setters
        output.write("\n  //Value Setters\n")
        for field in self.fields:
            output.write(  "  void " +field.name.capitalize()+"("+field.cppType+ " val );\n" )

        #generate value getters
        output.write("\n  //Present Getters\n")
        for field in self.fields:
            output.write( "  bool has"  +field.name.capitalize()+"() const { return hasField("+field.globalName+");}\n" )

        #generate value setters
        #output.write("\n  //Present Setters\n")
        #for field in self.fields:
        #    output.write(  "  void has"  +field.name.capitalize()+"( bool val) {hasField("+field.globalName+" , val);}\n" )

        output.write("\nprivate:\n")
        for field in self.fields:
            output.write("  "+field.cType+" m"+field.name.capitalize())
            if(field.isArray):
                output.write("["+str(field.arrayLen)+"]")
            if(field.desc != ""):
                output.write(";  //"+field.desc +"\n");
            else:
                output.write(";\n")

        output.write("};\n\n")
        return output.getvalue()

    def generateFunctions(self):
        #generate value getters
        output = StringIO.StringIO()
        output.write("/**********************************************************\n" )
        output.write("              " +self.className+ "                       \n" )
        output.write("**********************************************************/\n\n\n" )

        output.write(self.className+"::"+self.className +"(poly_packet_t* packet)\n:PolyPacket("+self.globalName+")\n{" )
        output.write("  //Bind all fields\n" )

        for field in self.fields:
            output.write("  getField("+field.globalName+")->mData = (uint8_t*) &m"+field.name.capitalize()+";\n" )

        output.write("  mPacket->mBound = true;\n  copyFrom(packet);\n}\n\n" )

        #create setters
        for field in self.fields:
            output.write( "void "+ self.className+"::" +field.name.capitalize()+"("+ field.cppType+"  val)\n{\n" )

            output.write( "  hasField("+field.globalName+",true);\n" )
            if(field.cppType == 'string'):
                output.write( "  int len= min((int)val.length() + 1,"+str(field.arrayLen)+");\n")
                output.write( "  getField(PF_sensorName)->mSize = len;\n")
                output.write( "  memcpy(m"+field.name.capitalize()+", val.c_str(), len);\n}\n" )
            else:
                if(field.isArray):
                    output.write( "  memcpy(m"+field.name.capitalize()+", val, "+ str(field.arrayLen)+" * sizeof("+ field.cType+"));\n}\n" )
                else:
                    output.write( "  m"+field.name.capitalize()+" = val;\n}\n\n" )

        return output.getvalue()







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

    def addField(self,field):
        field.id = self.fieldId
        self.fields.append(field)
        self.fieldIdx[field.name] = self.fieldId
        self.fieldId+=1


    def addPacket(self,packet):
        self.packets.append(packet)
        self.packetIdx[packet.name] = self.packetId
        self.packetId+=1

    def generateHeaderCommon(self, cpp):
        output = StringIO.StringIO()
        output.write('/**\n')
        output.write('  *@file '+self.fileName +'.h\n')
        output.write('  *@brief generated protocol source code\n')
        output.write('  *@author make_protocol.py\n')
        output.write('  *@date '+now.strftime("%m/%d/%y")+'\n')
        output.write('  */\n\n')
        output.write('/***********************************************************\n')
        output.write('        THIS FILE IS AUTOGENERATED. DO NOT MODIFY\n')
        output.write('***********************************************************/\n')

        if(cpp):
            output.write('#include "../poly_field.h"\n')
            output.write('#include "../PolyPacket.h"\n\n\n')
            output.write('using namespace Utilities::PolyPacket; \n\n')
        else:
            output.write('#include <poly_field.h>\n')
            output.write('#include <poly_packet.h>\n\n\n')

        #write packet descriptors
        output.write('//Declare extern packet descriptors\n')
        for packet in self.packets:
            output.write('extern poly_packet_desc_t* ' + packet.globalName+ ';\n')

        output.write('\n\n')

        #write field descriptors
        output.write('//Declare extern field descriptors\n')
        for field in self.fields:
            output.write('extern poly_field_desc_t* ' + field.globalName+ ';\n')

        output.write('\n\n')
        return output.getvalue()

    def generateSourceCommon(self, cpp):
        output = StringIO.StringIO()
        output.write('/**\n')
        if(cpp):
            output.write('  *@file '+self.fileName +'.cpp\n')
        else:
            output.write('  *@file '+self.fileName +'.c\n')
        output.write('  *@brief generated protocol source code\n')
        output.write('  *@author make_protocol.py\n')
        output.write('  *@date '+now.strftime("%m/%d/%y")+'\n')
        output.write('  */\n\n')
        output.write('/***********************************************************\n')
        output.write('        THIS FILE IS AUTOGENERATED. DO NOT MODIFY\n')
        output.write('***********************************************************/\n\n')


        output.write("#include \""+self.fileName+".h\"\n\n")
        #write packet descriptors
        output.write('//Declare extern packet descriptors\n')
        for packet in self.packets:
            output.write('poly_packet_desc_t* ' + packet.globalName+ ';\n')

        output.write('\n\n')

        #write field descriptors
        output.write('//Declare extern field descriptors\n')
        for field in self.fields:
            output.write('poly_field_desc_t* ' + field.globalName+ ';\n')

        output.write('\n\n')
        return output.getvalue()




    def generateHeaderC(self, file):
        output = StringIO.StringIO()
        output.write(self.generateHeaderCommon(False))

        #write structs for each packet
        output.write('//Structs for packet types')
        for packet in self.packets:
            output.write(packet.getStruct())

        output.write('\n\n')

        output.write('void '+self.name+'protocol_init();\n')

        text_file = open(file,"w")
        text_file.write(output.getvalue())
        text_file.close()

    def generateHeaderCPP(self, file):
        output = StringIO.StringIO()
        output.write(self.generateHeaderCommon(True))

        #write classes for each packet
        output.write('//Classes for packet types\n')
        for packet in self.packets:
            output.write(packet.generateClass("PolyPacket"))

        output.write('\n\n')

        output.write('void '+self.name+'_protocol_init();\n')

        text_file = open(file,"w")
        text_file.write(output.getvalue())
        text_file.close()

    def generateSourceCPP(self,file):
        output = StringIO.StringIO()
        output.write(self.generateSourceCommon(True))

        #init
        output.write("void "+self.name+"_protocol_init()\n{\n  //Packet Descriptors\n" )
        for packet in self.packets:
            output.write("  "+packet.globalName+" = new_poly_packet_desc(\""+ packet.name+"\", "+ str(len(packet.fields))+ " );\n" )

        output.write("\n\n  //Field Descriptos\n")

        for field in self.fields:
            output.write("  "+field.globalName+" = new_poly_field_desc(\""+ field.name+"\", TYPE_"+ field.type.upper()+" , "+ str(field.arrayLen)+ " , "+field.format.upper()+" );\n" )

        for packet in self.packets:
            output.write("\n\n  //Setting fields Descriptors for "+ packet.className+"\n")
            for field in packet.fields:
                output.write("  poly_packet_desc_add_field(" + packet.globalName +" , " + field.globalName+" , " + str(field.isRequired).lower() +" );\n")

        output.write("\n}\n" )

        #packets
        for packet in self.packets:
            output.write(packet.generateFunctions())

        text_file = open(file,"w")
        text_file.write(output.getvalue())
        text_file.close()




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

        arrayLen = 1
        #see if its an array
        m = re.search('\[([0-9]*)\]', strType)
        if(m):
            if(m.group(1) != ''):
                arrayLen = int(m.group(1))
            strType = strType[0:m.start()]


        newField = fieldDesc(name, strType, arrayLen)

        if('format' in field.attrib):
            format = field.attrib['format'].lower()
            if not format in formatDict:
                print "INVALID FORMAT :" + format

            newField.format = formatDict[format]

        if('desc' in field.attrib):
            newField.desc = field.attrib['desc']

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


def createDoc(protocol, filename):
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




    text_file = open(filename, "w")
    text_file.write(output.getvalue())
    text_file.close()



def main():
    global path
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
    #protocol.generateHeaderC(path+"/c_header.h")
    protocol.generateHeaderCPP(path+"/" + protocol.fileName+".h")
    protocol.generateSourceCPP(path+"/" + protocol.fileName+".cpp")
    #createSourceC(protocol)
    createDoc(protocol,path+"/" + protocol.fileName+".md")

if __name__ == "__main__":
    main()
