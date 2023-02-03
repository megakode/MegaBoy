//
// Created by sbeam on 4/4/22.
//

#include <vector>
#include <cstring>
#include "Cartridge.h"



uint8_t Cartridge::read(uint16_t address) {
    return rom[address + rom_offset];
}

void Cartridge::write(uint8_t value, uint16_t address) {

}

bool Cartridge::load( uint8_t *data, long size ) {

    if(size <= MAXIMUM_ROM_SIZE ){
        for(int i = 0 ; i < size ; i++ )
        {
            rom[i] = *(data++);
        }
        //memcpy(rom.data(),data,size);
    }

    return true;
}
