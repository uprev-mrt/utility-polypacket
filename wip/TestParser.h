/**
  *@file PolyPacket.h
  *@brief "Description"
  *@author "Your name"
  *@date "DD"/"Month"/"Year"
  */
#pragma once
#include "../PolyParser.h"
#include <string>


#ifdef __cplusplus

using namespace std;

namespace Utilities{
namespace PolyPacket{

class TestService : public PolyParser
{
public:
  TestParser(int interfaceCount = 1);
  ~TestParser();

private:
  

};


}}
#endif
