/**
  *@file PolyParser.cpp
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "PolyParser.h"
#include <string>

using namespace std;

namespace Utilities{
namespace PolyPacket{

PolyParser::PolyParser(int maxPacketDescriptors, int interfaceCount )
{
  mParser = new_poly_parser(maxPacketDescriptors, interfaceCount);
}

PolyParser::~PolyParser()
{

}

void PolyParser::registerPacketDescriptor(poly_packet_desc_t* pDesc)
{
  poly_parser_register_desc(mParser, pDesc);
}

void PolyParser::start(int fifoLen)
{
  poly_parser_start(mParser, fifoLen);
}

int PolyParser::feed(uint8_t* data, int len, interface = 0)
{
  poly_parser_feed(mParser,interface, data, len);
}

void PolyParser::feedJSON(std::string json)
{

}

int PolyParser::available(int interface =0) const
{
  poly_parser_try_parse(mParser,interface);
  return mParser->mInterfaces[interface].mPacketFifo.mCount;
}


bool PolyParser::next(PolyPacket& packet, int interface =0)
{
  bool retVal = false;
  poly_packet_t newPacket;

  //attempt to parse new packet
  poly_parser_try_parse(mParser,interface);

  if(poly_parser_next(mParser, interface, &newPacket)
  {
    retVal = true;
    packet.copyFrom(&newPacket)
  }

  return retVal;
}




}}
