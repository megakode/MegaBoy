//
// Created by sbeam on 4/4/22.
//

#include <vector>
#include <cstring>
#include "Cartridge.h"

bool Cartridge::isLoaded()
{
    return is_loaded;
}

uint8_t Cartridge::read(uint16_t address)
{
    return rom[address + rom_offset];
}

void Cartridge::write(uint8_t value, uint16_t address)
{
}

bool Cartridge::load(uint8_t *data, long size)
{

    if (size <= MAXIMUM_ROM_SIZE)
    {
        for (int i = 0; i < size; i++)
        {
            rom[i] = *(data++);
        }
        // memcpy(rom.data(),data,size);

        memcpy(&header, &rom[0x100], sizeof(CartridgeHeader));
        is_loaded = true;
    }

    return true;
}

CartridgeHeader &Cartridge::GetHeader()
{
    return header;
}

const std::string Cartridge::CartridgeTypeName()
{

    static std::map<CartridgeType, const std::string> names = {
        {ROM_Only, "ROM Only (No MBC)"},
        {MBC1, "MBC1"},
        {MBC1_RAM, "MBC1 + RAM"},
        {MBC1_RAM_BATTERY, "MBC1 + RAM + Battery"},
        {MBC2, "MBC2"},
        {MBC2_BATTERY, "MBC2 + Battery"},
        {ROM_RAM, "ROM + RAM (No official cartridge uses this!)"},
        {ROM_RAM_BATTERY, "ROM + RAM + Battery (No official cartridge uses this!"},
        {MMM01, "MMM01"},
        {MMM01_RAM, "MMM01 + RAM"},
        {MMM01_RAM_BATTERY, "MMM01 + RAM + Battery"},
        {MBC3_TIMER_BATTERY, "MBC3 + Timer + Battery"},
        {MBC3_TIMER_RAM_BATTERY, "MBC3 + Timer + RAM + Battery"},
        {MBC3, "MBC3"},
        {MBC3_RAM, "MBC3 + RAM"},
        {MBC3_RAM_BATTERY, "MBC3 + RAM + Battery"},
        {MBC5, "MBC5"},
        {MBC5_RAM, "MBC5 + RAM"},
        {MBC5_RAM_BATTERY, "MBC5 + RAM + Battery"},
        {MBC5_RUMBLE, "MBC5 + Rumble"},
        {MBC5_RUMBLE_RAM, "MBC5 + Rumble + RAM"},
        {MBC5_RUMBLE_RAM_BATTERY, "MBC5 + Rumble + RAM + Battery"},
        {MBC6, "MBC6"},
        {MBC7_SENSOR_RUMBLE_RAM_BATTERY, "MBC7 + Sensor + Rumble + RAM + Battery"},
        {POCKET_CAMERA, "Pocket Camera"},
        {BANDAI_TAMA5, "Bandai TAMA5 (Only used in Tamagotchi 3)"},
        {HuC3, "HuC3 - Hudson Soft MBC with RTC and piezo"},
        {HuC1, "HuC1 - Hudson Soft MBC with IR"}};

    return names[header.cartridge_type];
};
