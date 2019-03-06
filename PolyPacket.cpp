
#include "PolyPacket.h"

using namespace std;

namespace Utilities{
namespace PolyPacket{


PolyPacket::PolyPacket(poly_packet_desc_t** descriptor, void (*init_function)())
{
  //if we havent already initialized the protocol, do it now
    (*init_function)();

  mPacket = new_poly_packet(*descriptor, false);
}

PolyPacket::PolyPacket(poly_packet_desc_t** descriptor)
{
  mPacket = new_poly_packet(*descriptor, false);
}

PolyPacket::~PolyPacket()
{
  poly_packet_destroy(mPacket);
}

void PolyPacket::copyFrom(poly_packet_t* packet)
{
  if(packet != NULL)
  {
    poly_packet_copy(packet, mPacket);
  }
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

int PolyPacket::pack(uint8_t* data)
{
  return poly_packet_pack(mPacket, data);
}

bool PolyPacket::parse(uint8_t* data, int len)
{
  return (poly_packet_parse_buffer(mPacket, mPacket->mDesc , data, len ) == PACKET_VALID);
}


} //PolyPacket
} //Utilities
