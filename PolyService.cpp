/**
  *@file PolyService.cpp
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "PolyService.h"
#include <string>

using namespace std;

namespace Utilities{
namespace PolyPacket{

PolyService::PolyService(int maxPacketDescriptors, int interfaceCount )
{
  mParser = new_poly_parser(maxPacketDescriptors, interfaceCount);
}

PolyService::~PolyService()
{

}

void PolyService::registerPacketDescriptor(poly_packet_desc_t* pDesc)
{
  poly_parser_register_desc(mParser, pDesc);
}

void PolyService::start(int fifoLen)
{
  poly_parser_start(mParser, fifoLen);
}

int PolyService::feed(uint8_t* data, int len, interface = 0)
{
  poly_parser_feed(mParser,interface, data, len);
}

void PolyService::feedJSON(std::string json)
{

}

int PolyService::available(int interface =0) const
{
  poly_parser_try_parse(mParser,interface);
  return mParser->mInterfaces[interface].mPacketFifo.mCount;
}


bool PolyService::next(PolyPacket& packet, int interface =0)
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
