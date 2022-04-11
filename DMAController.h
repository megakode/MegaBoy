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

    explicit DMAController( HostMemory& mem );

    void Step( uint16_t cycles );

    void RequestTransfer( uint8_t addr_hibyte );

    [[nodiscard]] bool IsTransferInProgress() const;

};

#endif //MEGABOY_DMACONTROLLER_H
