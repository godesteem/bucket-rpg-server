cmake_minimum_required(VERSION 3.7.2)

project(BucketServer)

set(CMAKE_CXX_STANDARD 11)
set(CMALE_CXX_STANDARD_REQUIRED True)

set(SOURCES
  server.cpp server.h
  utils.h
  )

set(MAIN_SOURCES
  main.cpp
  )

set(TEST_SOURCES
  tests/main.cpp
  tests/test_utils.h
  )

add_executable(BucketServer ${SOURCES} ${MAIN_SOURCES})
add_executable(BucketServer-test ${SOURCES} ${TEST_SOURCES})

find_library(ENET_LIB enet HINTS /usr/local/lib)
target_link_libraries(BucketServer PUBLIC ${ENET_LIB})
target_link_libraries(BucketServer-test PUBLIC ${ENET_LIB})

#target_link_libraries(ccc-test m)

