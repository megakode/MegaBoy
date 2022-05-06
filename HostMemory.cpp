//
// Created by sbeam on 09/12/2021.
//

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "HostMemory.h"
#include "DMG_ROM.h"

HostMemory::HostMemory() noexcept{
    //memcpy(memory,DMG_ROM_bin,DMG_ROM_bin_len);
    Write(IOAddress::Boot_ROM_Disabled,0);
}

uint8_t& HostMemory::operator[] (uint16_t index)
{
    /*
    if (index > (UINT16_MAX+1)) {
        std::cout << "Array index out of bound, exiting";
        exit(0);
    }
     */
    return memory[index];
}

uint8_t HostMemory::Read( uint16_t address ) const
{
    if(!memory[static_cast<uint16_t>(IOAddress::Boot_ROM_Disabled)] && address <= 0xff){
        return DMG_ROM_bin[address];
    }

    return memory[address];
}

void HostMemory::Write( const uint16_t address, const uint8_t value )
{
    memory[address] = value;

    if(address >= 0xff00 )
    {
        if(didWriteToIOAddress!= nullptr)
        {
            didWriteToIOAddress(address,value);
        }
    }
}

void HostMemory::Write(IOAddress address, uint8_t value )
{
    Write(static_cast<uint16_t>(address), value );
}
