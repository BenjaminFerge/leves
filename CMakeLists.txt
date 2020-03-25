cmake_minimum_required(VERSION 3.9)

project(yess VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(YESS_VERSION_TWEAK "pre-alpha")
configure_file(
        src/Version.h.in
	include/Version.h
)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
set(MAIN_FILE src/main.cpp)
set(SRC_FILES 
	src/Server.cpp
	src/ServiceHandler.cpp
	src/Response.cpp
	src/ActionHandler.cpp
	src/db/Entities/Stream.cpp
	src/db/Entities/Event.cpp
	src/db/Repositories/StreamRepository.cpp
	src/msg/Serializer.cpp
	inc_duktape/duktape.c
	src/ext/DukContext.cpp
)
set(INC_FILES
	src/Server.hpp
	src/ServiceHandler.hpp
	src/Response.hpp
	src/ActionHandler.hpp
	src/db/Entities/Stream.hpp
	src/db/Entities/Event.hpp
	src/db/Repositories/StreamRepository.hpp
	src/msg/Serializer.hpp
	src/ext/IContext.hpp
	src/ext/DukContext.hpp
)
add_library(${PROJECT_NAME}_objects OBJECT ${SRC_FILES} ${INCL_FILES})
add_subdirectory(utils)
add_subdirectory(debug)

target_include_directories(${PROJECT_NAME}_objects PRIVATE "${CMAKE_BINARY_DIR}/include" inc_duktape)
add_executable(${PROJECT_NAME} ${MAIN_FILE} $<TARGET_OBJECTS:${PROJECT_NAME}_objects>)
target_include_directories(${PROJECT_NAME} PRIVATE inc_duktape)

add_subdirectory(test)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS})

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
