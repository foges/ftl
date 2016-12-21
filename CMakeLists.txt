cmake_minimum_required(VERSION 3.4)

project(ftl)

set(GTEST_PATH
  ~/code/googletest/googletest
  CACHE STRING "Directory search for gTest."
)

set(CMAKE_LIBRARY_PATH ${GTEST_PATH}/build ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH ${GTEST_PATH}/include ${CMAKE_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)

find_package(GTest)

file(GLOB GTEST_SRC "test/gtest_*.cpp" REQUIRED)

include_directories("include" ${GTEST_INCLUDE_DIRS})

add_executable(ftl_test ${GTEST_SRC})

target_link_libraries(ftl_test ${GTEST_BOTH_LIBRARIES})

