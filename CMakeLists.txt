cmake_minimum_required (VERSION 2.6)

project (Buddha)

# Set the build type if it isn't already
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -Wall -pedantic")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set (SRC_FILES
  src/main.cpp
  src/BuddhaWorker.cpp
)

add_executable (buddha ${SRC_FILES})
