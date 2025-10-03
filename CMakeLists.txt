cmake_minimum_required(VERSION 3.10)
project(message_queue_ipc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(message_queue STATIC message_queue.cpp)
target_include_directories(message_queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(message_create message_create.cpp)
target_link_libraries(message_create PRIVATE message_queue)

add_executable(message_send message_send.cpp)
target_link_libraries(message_send PRIVATE message_queue)

add_executable(message_receive message_receive.cpp)
target_link_libraries(message_receive PRIVATE message_queue)

add_executable(message_chqbytes message_chqbytes.cpp)
target_link_libraries(message_chqbytes PRIVATE message_queue)

add_executable(message_rm message_rm.cpp)
target_link_libraries(message_rm PRIVATE message_queue)

add_executable(message_info message_info.cpp)
target_link_libraries(message_info PRIVATE message_queue)