//
// Created by sbeam on 3/31/22.
//

#pragma once

#include <string>
#include <map>
#include <cstdint>

constexpr static long MAXIMUM_ROM_SIZE = 1048576;
using ROMArray = std::array<uint8_t, MAXIMUM_ROM_SIZE>;

/// MCB3 with RAM size 64KB refers to MBC30, used only in Ppcket Monster Crystal Version (jap)
enum CartridgeType : uint8_t
{
    ROM_Only = 0x0,
    MBC1 = 0x1,
    MBC1_RAM = 0x2,
    MBC1_RAM_BATTERY = 0x3,
    MBC2 = 0x5,
    MBC2_BATTERY = 0x6,
    ROM_RAM = 0x08,         // No official cartridge uses this
    ROM_RAM_BATTERY = 0x09, // No official cartridge uses this
    MMM01 = 0x0B,
    MMM01_RAM = 0x0c,
    MMM01_RAM_BATTERY = 0x0d,
    MBC3_TIMER_BATTERY = 0x0f,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3 = 0x11,
    MBC3_RAM = 0x12,
    MBC3_RAM_BATTERY = 0x13,
    MBC5 = 0x19,
    MBC5_RAM = 0x1a,
    MBC5_RAM_BATTERY = 0x1b,
    MBC5_RUMBLE = 0x1c,
    MBC5_RUMBLE_RAM = 0x1d,
    MBC5_RUMBLE_RAM_BATTERY = 0x1e,
    MBC6 = 0x20,
    MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    POCKET_CAMERA = 0xfc,
    BANDAI_TAMA5 = 0xfd,
    HuC3 = 0xfe,
    HuC1 = 0xff,
};

struct CartridgeHeader
{

    /// 0x0100 - 0x0103 Code that jumps to actual game entry point. Usually a NOP + JP instruction.
    uint8_t entry_point[4];

    /// 0x0134-0x143 - Nintendo Logo
    uint8_t nintendo_logo[48];

    /// 0x134-0x143 - Title (NOTE: CGB has some special bytes at the end of this field)
    char title[16];

    /// 0x0144-0x0145 New Licensee Code. two character ascii licensee code. Older games use the header entry at 0x014b instead.
    char licensee_code[2];

    /// 0x0146 SGB Flag - Specifies whether the game supports SGB functions. Common values are
    /// 0x00: No SGB functions
    /// 0x03: Game supports SGB functions
    uint8_t SGB_flag;

    /// 0x0147 - Cartridge Type
    CartridgeType cartridge_type;

    /// 0x0148 - ROM Size - Specifies the ROM Size of the cartridge. Typically calculated as “N such that 32 KiB << N”.
    /// $00	32 KByte	2 (No ROM banking)
    /// $01	64 KByte	4
    /// $02	128 KByte	8
    /// $03	256 KByte	16
    /// $04	512 KByte	32
    /// $05	1 MByte	64
    /// $06	2 MByte	128
    /// $07	4 MByte	256
    /// $08	8 MByte	512
    uint8_t ROM_size;

    /// 0x0149 - RAM Size - specifies the size of the external RAM in the cartridge (if any).
    /// $00	No RAM (When using a MBC2 chip, 0 must be specified as RAM size, even though the MBC2 includes a RAM of 512 x 4 bits)
    /// $01	Unused
    /// $02	8 KB	1 bank
    /// $03	32 KB	4 banks of 8 KB each
    /// $04	128 KB	16 banks of 8 KB each
    /// $05	64 KB	8 banks of 8 KB each
    uint8_t RAM_size;

    /// 0x014a - Destination Code - Specifies whether game is supposed to be sold in- or outside japan.
    /// 0x00 - Japanese
    /// 0x01 - Non-Japanese
    uint8_t destination_code;

    /// 0x014b - Old Licensee Code
    /// Specifies the games company/publisher code in range $00-FF.
    /// A value of $33 signals that the New Licensee Code (in header bytes $0144-0145) is used instead.
    /// (Super Game Boy functions won’t work if <> $33.)
    /// A list of licensee codes can be found here: https://raw.githubusercontent.com/gb-archive/salvage/master/txt-files/gbrom.txt
    uint8_t old_licensee_code;

    /// 0x014c - Specifies the version number of the game. Usually 0x00.
    uint8_t mask_ROM_version;

    /// 0x014d - Contains an 8 bit checksum across the cartridge header bytes $0134-014C.
    /// If the byte at $014D does not match the lower 8 bits of x, the boot ROM will lock up, and the cartridge program won’t run.
    /// The boot ROM computes x as follows:
    /// x = 0
    /// i = $0134
    /// while i <= $014C
    ///	    x = x - [i] - 1
    uint8_t header_checksum;

    /// 0x014e-0x014f - Global Checksum
    /// Contains a 16 bit checksum (upper byte first) across the whole cartridge ROM.
    /// Produced by adding all bytes of the cartridge (except for the two checksum bytes).
    /// The Game Boy doesn’t verify this checksum.
    uint16_t global_checksum;

    /// Get the ROM size in bytes, based on the cartridge header field "ROMSize" (0x0148).
    [[nodiscard]] uint32_t ROMSizeInBytes() const
    {
        return (32 * 1024) << ROM_size;
    }

} __attribute__((packed));
