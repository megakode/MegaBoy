//
// Created by sbeam on 09/12/2021.
//

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "HostMemory.h"
#include "DMG_ROM.h"

HostMemory::HostMemory( Cartridge &cartridge ) noexcept : cartridge(cartridge) {
    std::cout << "HostMemory()";
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

    if(index <= 0x7fff && memory[static_cast<uint16_t>(IOAddress::Boot_ROM_Disabled)] ) {
        return memory[index];
    }
    return memory[index];

}

uint8_t HostMemory::Read( uint16_t address ) const
{
    if(!memory[static_cast<uint16_t>(IOAddress::Boot_ROM_Disabled)] && address <= 0xff){
        return DMG_ROM_bin[address];
    }

    if(address <= 0x7fff) {
        return cartridge.read(address);
    }

    return memory[address];
}

void HostMemory::Write( const uint16_t address, const uint8_t value )
{
    if(address <= 0x7fff) {
        cartridge.write(value,address);
    } else {
        memory[address] = value;
    }


    // todo:

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
