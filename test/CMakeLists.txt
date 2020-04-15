file(GLOB SRCS *.cpp)
file(GLOB JSON_CMDS ./commands/*.json)

add_executable(testyess ${SRCS} ${JSON_CMDS}
	$<TARGET_OBJECTS:${PROJECT_NAME}_objects>
)

file(COPY ${CMAKE_SOURCE_DIR}/test/commands
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
file(COPY ${CMAKE_SOURCE_DIR}/test/projections
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(Threads REQUIRED)

find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

target_link_libraries(testyess PRIVATE
	${CONAN_LIBS}
	yessutils
	yessdebug
	gRPC::grpc++
	gRPC::grpc++_reflection
	protobuf::libprotobuf
	-lsystemd
)

target_include_directories(testyess PRIVATE ../inc_duktape ../protos)

add_test(NAME all_tests COMMAND testyess)
