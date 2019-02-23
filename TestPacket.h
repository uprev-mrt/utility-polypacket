/**
  *@file PolyField.h
  *@brief header for c implementation of variable packet field
  *@author Jason Berger
  *@date 02/19/2019
  */

#pragma once
#include "PolyPacket.h"
using namespace std;

namespace Utilities{


class TestPacket : public PolyPacket
{
public:
  TestPacket(PolyPacketDesc& pDesc)
  :PolyPacket(pDesc)
  {

  }

  // void F1(string str ) {mFields[0].setString(str);}
  // string F1() {return mFields[0].getString();}

   void F2(int val) {mFields[1].set<int>(val);}
   int F2() {return mFields[1].get<int>();}
  //
  // void F3(uint8_t val) {mFields[2].set<uint8_t>(val);}
  // uint8_t F3() {return mFields[2].get<uint8_t>();}
  //
  // void F4(double val) {mFields[3].set<double>(val);}
  // double F4() {return mFields[3].get<double>();}
  //
  // void F5(int val) {mFields[4].set<int>(val);}
  // int F5() {return mFields[4].get<int>();}

private:
};

}
