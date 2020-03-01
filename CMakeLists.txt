cmake_minimum_required(VERSION 3.9)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
  message(" [INFO] VCPKG CMAKE_TOOLCHAIN_FILE =
 ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(Leves VERSION 0.1.0)

set(LEVES_VERSION_TWEAK "pre-alpha")
configure_file(
        src/Version.h.in
	include/Version.h
)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
add_executable(leves 
	src/main.cpp
	src/Server.hpp
	src/Server.cpp
	src/ServiceHandler.hpp
	src/ServiceHandler.cpp
)
target_include_directories(leves PRIVATE "${CMAKE_BINARY_DIR}/include")

add_subdirectory(test)

find_package(Poco CONFIG REQUIRED COMPONENTS Net Util)
target_link_libraries(leves PRIVATE Poco::Net Poco::Util)

add_custom_target(
  run
  COMMAND leves
  DEPENDS leves
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

set_target_properties(
  leves
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
