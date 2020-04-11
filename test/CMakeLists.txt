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

target_link_libraries(testyess PRIVATE ${CONAN_LIBS} yessutils yessdebug)
target_include_directories(testyess PRIVATE ../inc_duktape)

add_test(NAME all_tests COMMAND testyess)
