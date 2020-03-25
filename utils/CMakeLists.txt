cmake_minimum_required(VERSION 3.9)
project(yessutils)

include_directories(.)

file(GLOB SOURCES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

