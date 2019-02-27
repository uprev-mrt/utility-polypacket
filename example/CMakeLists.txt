cmake_minimum_required(VERSION 2.6)


#include_directories("../")
#include_directories(".")

# Link runTests with what we want to test and the GTest and pthread library
add_definitions(-DUNIT_TESTING_ENABLED)

add_library(
  PolyPacket
  ../PolyPacket.cpp
  ../PolyPacket.h
  ../poly_packet.h
  ../poly_packet.c
  ../poly_field.h
  ../poly_field.c
  )
add_executable(
  example
  main.cpp
  SampleProtocol_proto.cpp
  SampleProtocol_proto.h
  )
target_link_libraries(example PolyPacket)
