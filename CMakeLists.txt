cmake_minimum_required (VERSION 2.6)

project (IPC)

include_directories(
    ${CMAKE_SOURCE_DIR}/JsonParser
)

add_executable(IpcUnixSockets.exe 
    ${CMAKE_SOURCE_DIR}/UnixSockets/main.cpp
    ${CMAKE_SOURCE_DIR}/UnixSockets/Server.cpp
    ${CMAKE_SOURCE_DIR}/JsonParser/Observer.cpp
    ${CMAKE_SOURCE_DIR}/JsonParser/BaseMsg.cpp
)

add_executable(IpcJsonParser.exe 
    ${CMAKE_SOURCE_DIR}/JsonParser/main.cpp
    ${CMAKE_SOURCE_DIR}/JsonParser/BaseMsg.cpp
)