/**
  *@file PolyParser.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "PolyPacket.h"
#include "poly_parser.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{
class PolyParser
{
public:
  PolyParser(int maxPacketDescriptors, int interfaceCount = 1);
  ~PolyParser();

  void registerPacketDescriptor(poly_packet_desc_t* pDesc);
  void start(int fifoLen);

  int feed(uint8_t* data, int len, interface = 0);
  void feedJSON(std::string json);
  int available(int interface =0) const {return mParser->mInterfaces[interface].mPacketFifo.mCount;}
  bool next(PolyPacket& packet, int interface =0);
protected:
  poly_parser_t* mParser;
};


}}
#endif
