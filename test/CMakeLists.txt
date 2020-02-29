find_package(GTest CONFIG REQUIRED)

file(GLOB SRCS *.cpp)

add_executable(testleves ${SRCS})

target_link_libraries(testleves PRIVATE GTest::gtest GTest::gtest_main
                                        GTest::gmock GTest::gmock_main)

add_test(AllTestsInMain testleves)