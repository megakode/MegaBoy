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
add_compile_definitions(DEBUG_LOG)
FetchContent_MakeAvailable(Catch2)

# Direct X ####################################

# IMGUI #####################################

# Set where the ImGui files are stored
set(IMGUI_PATH  "imgui")

# Compile as static library
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_link_libraries("ImGui" d3d9.lib)

        # Testing ###################################

enable_testing()
add_executable(cputests cputests.cpp cpu.cpp bit_instructions.h jump_instructions.h load_instructions.h in_out_instructions.h arithmetic_instructions.h general_instructions.h ex_block_search_instruction.h jump_tests.h nuklear_d3d9.h UI/RegisterWindow.cpp UI/RegisterWindow.h)
target_link_libraries(cputests PRIVATE Catch2::Catch2WithMain)
target_compile_features(cputests PRIVATE cxx_std_17)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(cputests)

add_executable(MegaBoy main.cpp cpu.cpp imgui/backends/imgui_impl_win32.cpp imgui/backends/imgui_impl_dx9.cpp imgui/imgui.cpp MegaBoyDebugger.cpp MegaBoyDebugger.h UI/RegisterWindow.cpp UI/RegisterWindow.h UI/UIConfig.h)
target_compile_features(MegaBoy PRIVATE cxx_std_20)
target_link_libraries(MegaBoy PUBLIC ImGui d3d9.lib)

#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/imgui.h"

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 