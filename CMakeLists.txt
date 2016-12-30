cmake_minimum_required(VERSION 3.4)

project(ftl)

set(GTEST_PATH
  ~/code/googletest/googletest
  CACHE STRING "Directory search for gTest."
)

set(CMAKE_LIBRARY_PATH ${GTEST_PATH}/build ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH ${GTEST_PATH}/include ${CMAKE_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)

include_directories("include")

# Tests
find_package(GTest REQUIRED)
#file(GLOB GTEST_SRC "test/gtest_*.cpp")
file(GLOB GTEST_SRC "test/gtest_seq.cpp")
add_executable(ftl_test ${GTEST_SRC})
target_include_directories(ftl_test PUBLIC "${GTEST_INCLUDE_DIRS}")
target_link_libraries(ftl_test ${GTEST_BOTH_LIBRARIES})

# Examples
# file(GLOB EXAMPLES_SRC "examples/project_euler/problems.cpp" REQUIRED)
# include_directories("include")
# add_executable(ftl_examples ${EXAMPLES_SRC})

