cmake_minimum_required(VERSION 2.6)



add_library(
  PolyPacket
  poly_packet.c
  poly_field.c
  poly_service.c
  poly_spool.c
  ../Fifo/fifo.c
  )
