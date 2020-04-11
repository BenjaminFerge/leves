cmake_minimum_required(VERSION 3.9)

project(yess VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(YESS_VERSION_TWEAK "pre-alpha")
configure_file(
        src/version.h.in
	include/version.h
)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
set(MAIN_FILE src/main.cpp)
set(SRC_FILES 
	src/server.cpp
	src/service_handler.cpp
	src/response.cpp
	src/action_handler.cpp
	src/db/entities/stream.cpp
	src/db/entities/event.cpp
	src/db/repositories/stream_repository.cpp
	src/db/repositories/sqlite_stream_repo.cpp
	src/msg/serializer.cpp
	inc_duktape/duktape.c
	src/ext/duk_context.cpp
)
set(INC_FILES
	src/server.hpp
	src/service_handler.hpp
	src/response.hpp
	src/action_handler.hpp
	src/db/entities/stream.hpp
	src/db/entities/event.hpp
	src/db/repositories/stream_repository.hpp
	src/db/repositories/sqlite_stream_repo.hpp
	src/msg/serializer.hpp
	src/ext/context.hpp
	src/ext/duk_context.hpp
	src/log.hpp
)
add_library(${PROJECT_NAME}_objects OBJECT ${SRC_FILES} ${INCL_FILES})
add_subdirectory(utils)
add_subdirectory(debug)

target_include_directories(${PROJECT_NAME}_objects PRIVATE "${CMAKE_BINARY_DIR}/include" inc_duktape)
add_executable(${PROJECT_NAME} ${MAIN_FILE} $<TARGET_OBJECTS:${PROJECT_NAME}_objects>)
target_include_directories(${PROJECT_NAME} PRIVATE inc_duktape)

add_subdirectory(test)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS} yessutils)

add_custom_target(
  run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
