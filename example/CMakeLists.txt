cmake_minimum_required(VERSION 2.6)


include_directories("../../../")


# Link runTests with what we want to test and the GTest and pthread library
add_definitions(-DUNIT_TESTING_ENABLED)

#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()

add_definitions(
    -fmax-errors=5
    -g
    )

add_library(
  PolyPacket
  ../poly_packet.h
  ../poly_packet.c
  ../poly_field.h
  ../poly_field.c
  ../poly_service.h
  ../poly_service.c
  ../../../Utilities/Fifo/Fifo.c
  )


add_executable(
  example
  main.c
  SampleProtocol.c
  SampleProtocol.h
  )
target_link_libraries(example PolyPacket)
