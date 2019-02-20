/**
  *@file SampleProtocol.c
  *@brief generated protocol source code
  *@author make_protocol.py
  *@date 02/20/19
  */

***********************************************************
        THIS FILE IS AUTOGENERATED. DO NOT MODIFY
***********************************************************

#include "SampleProtocol.h"

//Declare extern field descriptors
field_desc_t* VF_SRC;
field_desc_t* VF_BLOCKDATA;
field_desc_t* VF_SENSORB;
field_desc_t* VF_SENSORNAME;
field_desc_t* VF_BLOCKOFFSET;
field_desc_t* VF_SENSORA;
field_desc_t* VF_BLOCKSIZE;
field_desc_t* VF_DST;
field_desc_t* VF_CMD;


//Declare extern packet descriptors
packet_desc_t* VP_BLOCKRESP;
packet_desc_t* VP_DATA;
packet_desc_t* VP_BLOCKREQ;



void protocol_init()
{

//Set up Field descriptors
	VF_SRC= new_field_desc( "src" , sizeof(uint16_t) , 1);
	VF_SRC->mFormat = FORMAT_HEX;

	VF_BLOCKDATA= new_field_desc( "blockData" , sizeof(uint8_t) , 64);
	VF_BLOCKDATA->mFormat = FORMAT_NONE;

	VF_SENSORB= new_field_desc( "sensorB" , sizeof(int) , 1);
	VF_SENSORB->mFormat = FORMAT_DEC;

	VF_SENSORNAME= new_field_desc( "sensorName" , sizeof(char) , 1);
	VF_SENSORNAME->mVarLen = true;
	VF_SENSORNAME->mFormat = FORMAT_ASCII;

	VF_BLOCKOFFSET= new_field_desc( "blockOffset" , sizeof(uint32_t) , 1);
	VF_BLOCKOFFSET->mFormat = FORMAT_HEX;

	VF_SENSORA= new_field_desc( "sensorA" , sizeof(int16) , 1);
	VF_SENSORA->mFormat = FORMAT_DEC;

	VF_BLOCKSIZE= new_field_desc( "blockSize" , sizeof(uint32_t) , 1);
	VF_BLOCKSIZE->mFormat = FORMAT_DEC;

	VF_DST= new_field_desc( "dst" , sizeof(uint16_t) , 1);
	VF_DST->mFormat = FORMAT_HEX;

	VF_CMD= new_field_desc( "cmd" , sizeof(uint8_t) , 1);
	VF_CMD->mFormat = FORMAT_HEX;




//Set up packet descriptors
	VP_BLOCKRESP= new_packet_desc("blockResp") ;
		packet_desc_add_field( VP_BLOCKRESP, VF_SRC, true);
		packet_desc_add_field( VP_BLOCKRESP, VF_DST, true);
		packet_desc_add_field( VP_BLOCKRESP, VF_BLOCKDATA, true);
		packet_desc_add_field( VP_BLOCKRESP, VF_BLOCKSIZE, true);
		packet_desc_add_field( VP_BLOCKRESP, VF_BLOCKOFFSET, true);

	VP_DATA= new_packet_desc("Data") ;
		packet_desc_add_field( VP_DATA, VF_SRC, true);
		packet_desc_add_field( VP_DATA, VF_SENSORB, false);
		packet_desc_add_field( VP_DATA, VF_SENSORNAME, false);
		packet_desc_add_field( VP_DATA, VF_SENSORA, false);
		packet_desc_add_field( VP_DATA, VF_DST, true);
		packet_desc_add_field( VP_DATA, VF_CMD, true);

	VP_BLOCKREQ= new_packet_desc("blockReq") ;
		packet_desc_add_field( VP_BLOCKREQ, VF_SRC, true);
		packet_desc_add_field( VP_BLOCKREQ, VF_DST, true);
		packet_desc_add_field( VP_BLOCKREQ, VF_BLOCKSIZE, true);
		packet_desc_add_field( VP_BLOCKREQ, VF_BLOCKOFFSET, true);

}

