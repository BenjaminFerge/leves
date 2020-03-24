cmake_minimum_required(VERSION 3.9)
project(LevesDebug)

file(COPY ../duktape/examples/debug-trans-socket/duk_trans_socket.h
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
file(COPY ../duktape/examples/debug-trans-socket/duk_trans_socket_unix.c
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(.)

set(SOURCES duk_trans_socket_unix.c)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ../inc_duktape)

