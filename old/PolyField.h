/**
  *@file PolyField.h
  *@brief header for c implementation of variable packet field
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once
#include "Blob.h"
using namespace std;

namespace Utilities{

#define DEF_FIELD(n,t,l) PolyField* n##_desc = new PolyField("#n", sizeof(t), l);

  enum eFieldFormat {
    FORMAT_DEC =0,
    FORMAT_HEX,
    FORMAT_ASCII
  };

class PolyFieldDesc
{
public:
  PolyFieldDesc(string name, uint8_t objSize, uint32_t len);
  string mName;            //friendly name of field
  uint32_t mHash;          //unique identifier of field
  eFieldFormat mFormat;   //format used for printing and jsonifying field
  uint8_t mObjSize;       //size of object in field
  uint8_t mLen;          //number of objects in field (max)
  bool mVarLen;          //indicates that field can have variable sizes
};

class PolyField
{
public:
  PolyField(PolyFieldDesc& desc);

  template <typename T> T get(uint32_t idx =0) {return mBlob.get<T>(idx);}
  template <typename T> void set(T val) { mBlob.set<T>(val);}
  template <typename T> void set(T* val, int len) {mBlob.set<T>(val, len);}
  void setString(const string& str) {mBlob.setString(str);}
  string getString() {return mBlob.getString();}

  Blob& Data() {return mBlob;}
private:
  Blob mBlob;
  PolyFieldDesc& mDesc;    //pointer to descriptor
  bool mPresent;          //indicated field is present in packet
};

}
