/**
  *@file PolyPacket.h
  *@brief header for c implementation of variable packet
  *@author Jason Berger
  *@date 02/19/2019
  */
#include "PolyPacket.h"

using namespace std;

namespace Utilities{

PolyPacketDesc::PolyPacketDesc(string name)
{
  static int id =0;

  mTypeId = id++;
  mName = name;
}

void PolyPacketDesc::addField(PolyFieldDesc& pDesc, bool required)
{
  mFields.push_back(&pDesc);
  mFieldReqMap.push_back(required);

  if(required)
    mOptionalFieldCount++;

  //keep manifest big enough for 1 bit per optional field
  mManifestSize = (mOptionalFieldCount + 8)/8;
}

PolyPacket::PolyPacket(PolyPacketDesc& desc)
:mDesc(desc)
{
  for(int i=0; i< desc.Count(); i++)
  {
    mFields.push_back(PolyField(desc.Field(i)));
  }
}

void PolyPacket::reset()
{
  mHdr.mTypeId = 0;
  mHdr.mDataLen =0;
  mHdr.mToken =0;
  mFooter.mCheckSum =0;
  mFields.clear();
}


}
