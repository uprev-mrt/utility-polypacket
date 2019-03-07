/**
  *@file PolyPacket.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "Utilities/PolyService.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{

class TestService : public PolyService
{
public:
  TestParser(int interfaceCount = 1);
  ~TestParser();

  virtual void processIncoming();
  virtual void processOutgoing();

  packetHandler(poly_packet_t* packet);
  message_0_Handler(const msg& in, msg& out);
  message_1_Handler(const msg& in, msg& out);

private:


};


}}
#endif
