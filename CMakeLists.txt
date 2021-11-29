cmake_minimum_required(VERSION 3.0.0)
project(MegaBoy VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc)
set(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(cputests cputests.cpp cpu.cpp bit_instructions.h)
target_link_libraries(cputests GTest::GTest GTest::Main)
gtest_discover_tests(cputests)

add_executable(MegaBoy main.cpp cpu.cpp)

target_compile_features(MegaBoy PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 