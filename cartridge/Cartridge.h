//
// Created by sbeam on 4/4/22.
//

#ifndef MEGABOY_CARTRIDGE_H
#define MEGABOY_CARTRIDGE_H

#include <cstdint>
#include <array>
#include <iostream>
#include <format>
#include "CartridgeHeader.h"
#include "MBC.h"
#include <cassert>

class Cartridge
{

    constexpr static long MAXIMUM_ROM_SIZE = 1048576;

private:
    /// The offset into the ROM reflected by the selected BANK.
    int rom_offset = 0;
    ROMArray rom{}; // Fit the largest ROM size (8Mbit = 1MB)
    std::unique_ptr<MBC> mbc;
    // 0xa00 - 0xbfff RAM
    CartridgeHeader header;
    bool is_loaded = false;

public:
    Cartridge() {}

    CartridgeHeader &GetHeader();

    const std::string ROMSizeString();
    const std::string CartridgeTypeString();

    bool IsLoaded();
    // Reset the cartridge but does not remove it. Currently only affects the MBC bank.
    void Reset();
    bool Load(uint8_t *data, long size);
    uint8_t Read(uint16_t address);
    void Write(uint8_t value, uint16_t address);
};

#endif // MEGABOY_CARTRIDGE_H
