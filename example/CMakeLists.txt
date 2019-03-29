cmake_minimum_required(VERSION 2.6)


include_directories("../../../")
include_directories("src")
include_directories("src/lib")


# Link runTests with what we want to test and the GTest and pthread library


add_definitions(
    -fmax-errors=5
    -g
    -DPOLY_PACKET_DEBUG_LVL=1
    )

SET(CMAKE_C_FLAGS "-fsanitize=address -fsanitize=undefined")

add_library(
  PolyPacket
  ../cob_fifo.c
  ../poly_packet.c
  ../poly_field.c
  ../poly_service.c
  ../poly_spool.c
  )


add_executable(
  example
  src/main.c
  src/app_sample.c
  src/lib/SampleService.c
  src/lib/linux_uart.c
  )

target_link_libraries(example PolyPacket)
