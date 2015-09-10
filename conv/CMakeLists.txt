cmake_minimum_required(VERSION 3.1)

# Project Declaration
project(Convo C CXX)

# Source Declaration
add_executable(Convo
  conv.cpp
  stdafx.cpp
)

# Boost definition
find_package( Boost REQUIRED )

# Pthreads Support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )

target_link_libraries(Convo Threads::Threads png jpeg)

# C++ 11 forced on
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
