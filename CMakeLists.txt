cmake_minimum_required(VERSION 3.5)
project(PruebasUDP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp UDPSocket.cpp UDPSocket.h)
add_executable(PruebasUDP ${SOURCE_FILES})