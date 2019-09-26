project(segment_mmanager)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(run mmemory_test.c  mmemory.c)
