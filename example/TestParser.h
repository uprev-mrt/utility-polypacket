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

class PolyParser : public PolyParser
{
public:
  PolyParser(int interfaceCount = 1);
  ~PolyParser();

};


}}
#endif
