cmake_minimum_required(VERSION 3.9)

project(Leves VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

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
	src/Response.hpp
	src/Response.cpp
	src/ActionHandler.hpp
	src/ActionHandler.cpp
	src/Persistance/Entities/Stream.hpp
	src/Persistance/Repositories/StreamRepository.hpp
	src/Persistance/Repositories/StreamRepository.cpp
)
target_include_directories(leves PRIVATE "${CMAKE_BINARY_DIR}/include")

add_subdirectory(test)

target_link_libraries(leves PRIVATE ${CONAN_LIBS})

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
