/**
  *@file PolyPacket.h
  *@brief header for c implementation of variable packet
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once
#include <vector>
#include "PolyField.h"

using namespace std;

namespace Utilities{

typedef struct{
  uint8_t mTypeId;    //id of payload type
  uint16_t mDataLen;  //expected len of packet data (not including header and footer)
  uint16_t mToken;    //token for packet (used for acknowledgement/ echo cancellation in mesh nets)
}poly_hdr_t;

typedef struct{
  uint16_t mCheckSum; //checksum of packet data
}poly_footer_t;


class PolyPacketDesc
{
public:
  PolyPacketDesc(string name);
  void addField( PolyFieldDesc& pDesc, bool required = true);

  uint8_t TypeId() const {return mTypeId;}
  string Name() const {return mName;}
  int Count() const {return mFields.size();}
  PolyFieldDesc& Field(int idx) {return *mFields[idx];}

private:
  uint8_t mTypeId;                //unique id for payload type
  string mName;                   //friendly name of packet type
  vector<PolyFieldDesc*> mFields; //collection of field descriptors
  vector<bool> mFieldReqMap;      //map of which fields are required
  int mOptionalFieldCount=0;            //number of optional fields, used to calculate manifest size
  uint8_t mManifestSize =0;          //size of manifest in bytes
};

class PolyPacket
{
public:
  PolyPacket(PolyPacketDesc& desc);
  void reset();
  PolyField& field(const PolyFieldDesc& desc);

protected:
  PolyPacketDesc& mDesc;      //ref to pacekt descriptor
  poly_hdr_t mHdr;            //packet header
  vector<PolyField> mFields;  //packet fields
  poly_footer_t mFooter;      //packet footer
};

}
