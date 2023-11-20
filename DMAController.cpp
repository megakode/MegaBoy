//
// Created by sbeam on 1/18/22.
//

#include "DMAController.h"

DMAController::DMAController(HostMemory &mem) : mem(mem)
{
}

void DMAController::Step(uint16_t cycles)
{

    // If using mem.Write we trigger didWriteToIOAddress, which create an infinite loop and triggering another DMA transfer request
    mem.memory[(uint16_t)IOAddress::DMATransferStartAddress] = last_requested_source_addr;

    if (!transfer_in_progress)
    {
        return;
    }

    while (current_bytes_transferred < 160 && cycles > 0)
    {
        mem.memory[DestinationBaseAddress + current_bytes_transferred] = mem.Read(current_source_address);
        current_source_address++;
        current_bytes_transferred++;
        cycles--;
    }

    if (current_bytes_transferred == 160)
    {
        transfer_in_progress = false;
    }
}

void DMAController::RequestTransfer(uint8_t addr_hibyte)
{

    // Set the last request address regardless of it being legal or a transfer already in progress.
    // This is tested in mooneye-test-suite/acceptance/oam_dma/reg_read.s
    last_requested_source_addr = addr_hibyte;

    // 0xE0-0xff if not allowed
    if (transfer_in_progress)
    { // addr_hibyte >= 0xE0 ||
        return;
    }

    transfer_in_progress = true;
    current_bytes_transferred = 0;
    current_source_address = addr_hibyte << 8;
}

bool DMAController::IsTransferInProgress() const
{
    return transfer_in_progress;
}
