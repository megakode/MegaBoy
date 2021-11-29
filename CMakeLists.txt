cmake_minimum_required(VERSION 3.0.0)
project(MegaBoy VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_C_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc)
# set(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)
if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
endif(MSVC)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(cputests cputests.cpp cpu.cpp bit_instructions.h)
target_link_libraries(cputests PRIVATE Catch2::Catch2WithMain)
target_compile_features(cputests PRIVATE cxx_std_17)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(cputests)

add_executable(MegaBoy main.cpp cpu.cpp)
target_compile_features(MegaBoy PRIVATE cxx_std_20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 