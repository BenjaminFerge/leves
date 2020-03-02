file(GLOB SRCS *.cpp)

add_executable(testleves ${SRCS})

target_link_libraries(testleves PRIVATE ${CONAN_LIBS})

add_test(AllTestsInMain testleves)
