/**
  *@file PolyPacket.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "poly_packet.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{
class PolyPacket
{
public:
  PolyPacket(poly_packet_desc_t* descriptor);

  ~PolyPacket();
  void copyFrom(poly_packet_t* packet);

  poly_field_t* getField(poly_field_desc_t* desc) const;

  bool hasField(poly_field_desc_t* desc) const;
  void hasField(poly_field_desc_t* desc, bool val);

  string toJSON(bool printMeta = false) const;
  int pack(uint8_t* data);

protected:
  poly_packet_t* mPacket;

};


}}
#endif
