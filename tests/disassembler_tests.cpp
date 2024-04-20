
#include <catch2/catch_all.hpp>
#include <fstream>

#include "../disassembler/disassembler.h"

TEST_CASE("Disassembler CP2")
{
    Cartridge cart;
    HostMemory mem{cart};

    Disassembler dis;

    auto line = dis.CP_r({0xb1});

    // std::cout << line.text;

    // REQUIRE(line.cycles == 4);
    //  REQUIRE(line == "CP C");

    // line = dis.LD_r_n({0x00, 0x01, 0x02});

    // std::cout << line.text;
}

TEST_CASE("ld instructions")
{
    Cartridge cart;
    HostMemory mem{cart};

    Disassembler dis;

    // "LD_pnn_SP"
    auto line = dis.LD_pnnnn_SP({0x00, 0x11, 0x22});

    // std::cout << line.text;
    // REQUIRE(line.cycles == 16);
}