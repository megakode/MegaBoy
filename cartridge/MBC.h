#pragma once

#include "CartridgeHeader.h"

class MBC
{
public:
    MBC(ROMArray &rom) : rom_array(rom){};
    virtual void Reset() = 0;
    virtual void Write(uint8_t value, uint16_t address) = 0;
    virtual uint8_t Read(uint16_t address) = 0;
    virtual ~MBC(){};

protected:
    ROMArray &rom_array;
};

// ************************************************
// No MBC
// ************************************************

class NoMBC : public MBC
{
public:
    NoMBC(ROMArray &rom) : MBC(rom){};

    void Write(uint8_t value, uint16_t address) {
        // Do nothing
    };

    void Reset() {
        // Do nothing
    };

    uint8_t Read(uint16_t address)
    {
        return rom_array[address];
    }
};

/** ************************************************
 * @brief MBC1 - max 2MByte ROM and/or 32 KiB RAM)
 *************************************************** */

class MBC1 : public MBC
{
    uint8_t rom_bank_upper_bits = 0; // upper 2 bits
    uint8_t rom_bank_lower_bits = 0; // lower 5 bits
    uint8_t rom_bank_number = 0x01;

public:
    MBC1(ROMArray &rom) : MBC(rom){};

    void Reset()
    {
        rom_bank_lower_bits = 0;
        rom_bank_upper_bits = 0;
        rom_bank_number = 1;
    }

    void Write(uint8_t value, uint16_t address)
    {
        if (address >= 0x2000 && address <= 0x3fff)
        {
            // 2000–3FFF — ROM Bank Number (Write Only)
            rom_bank_lower_bits = value & 0b11111; // TODO number of bits differ depending on number of banks on cartridge
            rom_bank_number = rom_bank_lower_bits + (rom_bank_upper_bits << 5);
            if (rom_bank_number == 0)
            {
                rom_bank_number = 1;
            }

            std::cout << std::format("Set ROM Bank (lower) = {} \n", rom_bank_number);
        }
        else if (address >= 0x4000 && address <= 0x5fff)
        {
            // 4000–5FFF — RAM Bank Number — or — Upper Bits of ROM Bank Number (Write Only)
            rom_bank_upper_bits = value & 0b11;
            rom_bank_number = rom_bank_lower_bits + (rom_bank_upper_bits << 5);
            if (rom_bank_number == 0)
            {
                rom_bank_number = 1;
            }
            std::cout << std::format("Set ROM Bank (upper) = {} \n", rom_bank_number);
        }
        else if (address >= 0x6000 && address <= 0x7fff)
        {
            // 6000–7FFF — Banking Mode Select (Write Only)
            // Value:
            // 0 = simple (default): 0000–3FFF and A000–BFFF are locked to bank 0 of ROM and SRAM respectively.
            // 1 = advanced: 0000–3FFF and A000-BFFF can be bank-switched via the 4000–5FFF register.
            // TODO: implement ram switching if mode=1
            // if( romsize <= 512kb) then no effect
            std::cout << std::format("MBC1: Set banking mode= {} \n", value);
            // assert(value == 0);
        }
    };
    uint8_t Read(uint16_t address)
    {
        if (address <= 0x3fff)
        {
            return rom_array[address];
        }
        else if (address <= 0x7fff)
        {
            constexpr static int BankingAreaStartAddress = 0x4000;
            constexpr static int BankSizeInBytes = 16384;

            uint32_t translated_address = rom_bank_number * BankSizeInBytes + (address - BankingAreaStartAddress);
            // correct translation:
            // 3 * 0x4000 = c000
            // 7FF3 => EAB2
            if (rom_bank_number == 7)
            {
                // std::cout << std::format("MBC1 read addr {:X} => {:X} value: {:X} \n", address, translated_address, rom_array[translated_address]);
            }
            return rom_array[translated_address];
        }
        return 0;
    };
};