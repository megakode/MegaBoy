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

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

#target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)


add_compile_definitions(DEBUG_LOG)

# Direct X ####################################

if(WIN32)
set(MainFile "main_windows.cpp" "imgui/backends/imgui_impl_win32.cpp" "imgui/backends/imgui_impl_dx9.cpp")
set(MainLibs "d3d9.lib")
elseif(APPLE)
set(MainFile "main_macos.mm" "imgui/backends/imgui_impl_sdl.cpp" "imgui/backends/imgui_impl_metal.mm")
set(MainLibs "-framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore")
add_compile_options(-fobjc-arc)
else()
set(MainFile "main_linux.cpp" "imgui/backends/imgui_impl_sdl.cpp" "imgui/backends/imgui_impl_opengl3.cpp")
set(MainLibs "GLESv2")
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

# enable_testing()
add_executable(cputests 
    tests/cputests.cpp 
    CPU/cpu.cpp 
    HostMemory.cpp
    cartridge/Cartridge.cpp
    CPU/bit_instructions.h 
    CPU/jump_instructions.h 
    CPU/load_instructions.h 
    CPU/arithmetic_instructions.h 
    CPU/general_instructions.h 
    CPU/disassembler.cpp
    CPU/disassembler_arithmetic.cpp
    CPU/disassembler_load_instructions.cpp
    tests/lcd_tests.cpp 
    tests/timer_tests.cpp 
    tests/dma_controller_tests.cpp
    tests/disassembler_tests.cpp
    
    )

target_link_libraries(cputests Catch2::Catch2WithMain)
target_compile_features(cputests PRIVATE cxx_std_20)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(cputests)


add_executable(MegaBoy ${MainFile})
target_sources(MegaBoy PRIVATE
        LCD/lcd.cpp
        CPU/cpu.cpp
        imgui/imgui.cpp
        MegaBoyDebugger.cpp
        UI/RegisterWindow.cpp
        Gameboy.cpp
        HostMemory.cpp
        Timer.cpp
        LCD/lcd.cpp
        DMAController.cpp
        UI/DisassemblyWindow.cpp
        Joypad.cpp
        cartridge/Cartridge.cpp)
target_compile_features(MegaBoy PRIVATE cxx_std_20)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(MegaBoy PUBLIC ImGui ${SDL2_LIBRARIES} ${MainLibs})

# std::format is still experimental in Clang 15, so set the compiler and linker flags to enable it
if(APPLE)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fexperimental-library")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --lc++experimental")
endif()

# profilling
# does not work on MacOS
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -Og ") # -fsanitize=address
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -O0")

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=bounds -fsanitize=pointer-subtract")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=bounds -fsanitize=pointer-subtract")

#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/imgui.h"

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 