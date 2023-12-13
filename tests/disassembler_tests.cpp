
#include <catch2/catch_all.hpp>
#include <fstream>

#include "../CPU/disassembler.h"

TEST_CASE("LCD_Control_Register flags")
{
    Cartridge cart;
    HostMemory mem{cart};

    Disassembler dis;

    auto line = dis.CP_r({0xb1});

    std::cout << line.text;
    
    REQUIRE(line.cycles == 8);
    REQUIRE(line.text =="CP");
}