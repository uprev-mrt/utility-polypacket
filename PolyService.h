/**
  *@file PolyParser.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "PolyPacket.h"
#include "poly_parser.h"
#include "Utilities/MessageSpoo/spool.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{
class PolyService
{
public:
  PolyService(int maxPacketDescriptors, int interfaceCount = 1);
  ~PolyService();

  void start(int fifoLen);

  int feed(uint8_t* data, int len, interface = 0);
  void feedJSON(std::string json, interface =0);
  int available(int interface =0) const {return mParser->mInterfaces[interface].mPacketFifo.mCount;}
  bool next(PolyPacket& packet, int interface =0);
  int InterfaceCount() const {return mParser->mInterfaceCount;}

  virtual void processIncoming();
  virtual void processOutgoing();

  void process(int ms);
protected:
  void registerPacketDescriptor(poly_packet_desc_t* pDesc);
  poly_parser_t* mParser;
  spool_t* mSpool;
private:
  bool mAutoHandle;
  bool mAutoAck;
};


}}
#endif
