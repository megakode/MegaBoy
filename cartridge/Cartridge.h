//
// Created by sbeam on 4/4/22.
//

#ifndef MEGABOY_CARTRIDGE_H
#define MEGABOY_CARTRIDGE_H

#include <cstdint>
#include <array>

class Cartridge {

    constexpr static long MAXIMUM_ROM_SIZE = 1048576;

private:

    /// The offset into the ROM reflected by the selected BANK.
    int rom_offset = 0;
    std::array<uint8_t,MAXIMUM_ROM_SIZE> rom{}; // Fit the largest ROM size (8Mbit = 1MB)
    // 0xa00 - 0xbfff RAM

public:

    bool load( uint8_t *data, long size );

    uint8_t read( uint16_t address );
    void write( uint8_t value, uint16_t address );

};

#endif //MEGABOY_CARTRIDGE_H
