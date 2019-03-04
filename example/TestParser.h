/**
  *@file PolyPacket.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "../PolyPacket.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{
class PolyParser
{
public:
  PolyParser(int interfaceCount = 1);
  ~PolyParser();

  void feed(uint8_t* data, int len, interface = 0);
  void feedJSON(std::string json);
  int available(int interface =0) const {return mParser->mInterfaces[interface].mPacketFifo.mCount;}
  PolyPacket next(int interface =0);
protected:
  poly_parser_t* mParser;
};


}}
#endif
