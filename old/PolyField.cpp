/**
  *@file PolyField.cpp
  *@brief header for c++ implementation of variable packet field
  *@author Jason Berger
  *@date 02/19/2019
  */

#include "PolyField.h"

using namespace std;

namespace Utilities{



PolyFieldDesc::PolyFieldDesc(string name, uint8_t objSize, uint32_t len)
{

}

PolyField::PolyField(PolyFieldDesc& desc)
:mDesc(desc)
,mBlob(desc.mObjSize, desc.mLen)
{

}

}
