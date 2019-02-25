
#include "PolyPacket.h"

using namespace std;

namespace Utilities{
namespace PolyPacket{

PolyPacket::PolyPacket(poly_packet_desc_t* descriptor)
{
  mPacket = new_poly_packet(descriptor);
}

PolyPacket::~PolyPacket()
{
  poly_packet_destroy(mPacket);
}

poly_field_t* PolyPacket::getField(poly_field_desc_t* desc) const
{
  return poly_packet_get_field(mPacket, desc);
}

bool PolyPacket::hasField(poly_field_desc_t* desc) const
{
  return getField(desc)->mPresent;
}

void PolyPacket::hasField(poly_field_desc_t* desc, bool val)
{
  getField(desc)->mPresent= val;
}

string PolyPacket::toJSON(bool printMeta) const
{
  char buf[512];
  poly_packet_print_json(mPacket, buf, printMeta);
  return std::string(buf);
}


} //PolyPacket
} //Utilities
