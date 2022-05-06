//
// Created by sbeam on 1/18/22.
//

#include "DMAController.h"

DMAController::DMAController(HostMemory &mem) : mem(mem) {

}

void DMAController::Step( uint16_t cycles ){

    mem[static_cast<uint16_t>(IOAddress::DMATransferStartAddress)] = last_requested_source_addr;

    if(!transfer_in_progress){
        return;
    }

    while(current_bytes_transferred<160 && cycles > 0)
    {
        mem[DestinationBaseAddress+current_bytes_transferred] = mem[current_source_address];
        current_source_address++;
        current_bytes_transferred++;
        cycles--;
    }

    if(current_bytes_transferred == 160){
        transfer_in_progress = false;
    }

}

void DMAController::RequestTransfer(uint8_t addr_hibyte){

    // 0xE0-0xff if not allowed
    if( addr_hibyte >= 0xE0 || transfer_in_progress ){
        return;
    }

    transfer_in_progress = true;
    current_bytes_transferred = 0;
    last_requested_source_addr = addr_hibyte;
    current_source_address = addr_hibyte << 8;
}

bool DMAController::IsTransferInProgress() const {
    return transfer_in_progress;
}
