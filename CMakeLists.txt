cmake_minimum_required(VERSION 2.6)



add_library(
  mrtPolyPacket
  poly_packet.c
  poly_field.c
  poly_service.c
  poly_spool.c
  cob_fifo.c
  )

target_link_libraries(mrtPolyPacket mrtJSON)
