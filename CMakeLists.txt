cmake_minimum_required(VERSION 3.0.0)
project(MegaBoy VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_C_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc)
# set(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)
if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
endif(MSVC)

find_package(SDL2 REQUIRED)

#Include(FetchContent)

#FetchContent_Declare(
#        Catch2
#        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#        GIT_TAG        v2.13.7
#)
# add_compile_definitions(DEBUG_LOG)
#FetchContent_MakeAvailable(Catch2)

# Direct X ####################################

if(WIN32)
set(MainFile "main.cpp" "imgui/backends/imgui_impl_win32.cpp" "imgui/backends/imgui_impl_dx9.cpp")
set(MainLibs "d3d9.lib")
else()
set(MainFile "main_linux.cpp" "imgui/backends/imgui_impl_sdl.cpp" "imgui/backends/imgui_impl_opengl3.cpp")
set(MainLibs "")
endif()

# IMGUI #####################################

# Set where the ImGui files are stored
set(IMGUI_PATH  "./imgui/")

# Compile as static library
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
if(WIN32)
target_link_libraries("ImGui" d3d9.lib)
endif()

# Testing ###################################
find_package(Catch2 REQUIRED)
# enable_testing()
add_executable(cputests tests/cputests.cpp CPU/cpu.cpp HostMemory.cpp CPU/bit_instructions.h CPU/jump_instructions.h CPU/load_instructions.h CPU/arithmetic_instructions.h CPU/general_instructions.h tests/jump_tests.h)
target_link_libraries(cputests Catch2::Catch2WithMain)
target_compile_features(cputests PRIVATE cxx_std_17)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(cputests)


add_executable(MegaBoy ${MainFile} LCD/lcd.cpp CPU/cpu.cpp imgui/imgui.cpp MegaBoyDebugger.cpp MegaBoyDebugger.h UI/RegisterWindow.cpp UI/RegisterWindow.h UI/UIConfig.h Gameboy.cpp Gameboy.h HostMemory.cpp HostMemory.h Timer.cpp Timer.h LCD/lcd.cpp)
target_compile_features(MegaBoy PRIVATE cxx_std_20)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(MegaBoy PUBLIC ImGui SDL2 GLESv2 ${MainLibs})

# profilling
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/imgui.h"

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 