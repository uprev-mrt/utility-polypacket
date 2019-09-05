cmake_minimum_required(VERSION 2.6)


add_library(
  libPolyPacket
  poly_packet.c
  poly_field.c
  poly_service.c
  poly_spool.c
  )

target_link_libraries(libPolyPacket libMRT-JSON libMRT-COBS)
