cmake_minimum_required(VERSION 3.3)
project(chatTCP_winbot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/skel.h src/etcp.h src/sofuncs.c src/wincompat.c src/miscfuncs.c src/main.cpp src/iofuncs.c src/ChatServer.cpp src/ChatServer.h src/automutex.h)
add_executable(chatTCP_winbot ${SOURCE_FILES})
target_link_libraries(chatTCP_winbot ws2_32)