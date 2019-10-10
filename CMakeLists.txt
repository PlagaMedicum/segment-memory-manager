project(segment_mmanager)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(UNIT_TESTS_FILES mmemory.c tests/unit_tests.c)
set(LOAD_TESTS_FILES mmemory.c tests/load_tests.c)
add_executable(unit ${UNIT_TESTS_FILES})
add_executable(load ${LOAD_TESTS_FILES})
