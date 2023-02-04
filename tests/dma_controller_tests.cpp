//
// Created by sbeam on 1/18/22.
//

#include <catch2/catch_all.hpp>
#include <fstream>

#include "../HostMemory.h"
#include "../DMAController.h"


/*
TEST_CASE("DMA transfer")
{
    HostMemory mem;
    DMAController dma(mem);
    constexpr uint8_t src_add_hi_byte = 0x12;

    for (uint8_t i = 0; i != 160; i++) {
        mem.Write((src_add_hi_byte<<8) + i, i);
    }

    REQUIRE(mem[0x1200] == 0);
    REQUIRE(mem[0x1201] == 1);
    REQUIRE(mem[0x1202] == 2);

    REQUIRE(!dma.IsTransferInProgress());

    dma.RequestTransfer(src_add_hi_byte);

    REQUIRE(dma.IsTransferInProgress());

    dma.Step(1);

    REQUIRE(dma.IsTransferInProgress());

    dma.Step(200);

    REQUIRE(!dma.IsTransferInProgress());

    for(uint8_t i = 0 ; i < 160; i++ ){
        REQUIRE(mem[0xFE00 + i] == i);
    }

    REQUIRE(mem[0xFE00 + 160] == 0);

}
*/