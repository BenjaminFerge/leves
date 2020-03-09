file(GLOB SRCS *.cpp)
file(GLOB JSON_CMDS ./commands/*.json)

# add_executable(testleves ${SRCS})
# add_executable(test${PROJECT_NAME} $<TARGET_OBJECTS:${PROJECT_NAME}_objects>)
add_executable(testleves ${SRCS}  ${JSON_CMDS} $<TARGET_OBJECTS:${PROJECT_NAME}_objects>)

file(COPY ${CMAKE_SOURCE_DIR}/test/commands
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(testleves PRIVATE ${CONAN_LIBS})

add_test(NAME AllTestsInMain COMMAND testleves)
