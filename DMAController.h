//
// Created by sbeam on 1/18/22.
//

#ifndef MEGABOY_DMACONTROLLER_H
#define MEGABOY_DMACONTROLLER_H


#include "HostMemory.h"

class DMAController {

    /// OAM memory is always the destination
    static constexpr uint16_t DestinationBaseAddress = 0xFE00;

    HostMemory& mem;

    /// The high-byte of the last requested source address. Returned when reading the DMA transfer 0xff46 address.
    uint8_t last_requested_source_addr = 0;
    uint8_t current_bytes_transferred = 0;
    uint16_t current_source_address = 0;
    bool transfer_in_progress = false;

public:

    explicit DMAController( HostMemory& mem ) : mem(mem){

    }

    [[nodiscard]] uint8_t LastRequestedSourceAddress() const
    {
        return last_requested_source_addr;
    }

    void RequestTransfer( uint8_t addr_hibyte )
    {
        // 0xE0-0xff if not allowed
        if( addr_hibyte >= 0xE0 || transfer_in_progress ){
            return;
        }

        transfer_in_progress = true;
        current_bytes_transferred = 0;
        last_requested_source_addr = addr_hibyte;
        current_source_address = addr_hibyte << 8;
    }

    [[nodiscard]] bool IsTransferInProgress() const
    {
        return transfer_in_progress;
    }

    void Step( uint16_t cycles ){

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

};


#endif //MEGABOY_DMACONTROLLER_H
