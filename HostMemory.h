//
// Created by sbeam on 09/12/2021.
//

#pragma once

#include <cstdint>

class HostMemory {

public:

    uint8_t& operator[] (int index);

    /// Read a byte from the bus
    [[nodiscard]] uint8_t Read(uint16_t address) const;

    /// Write a byte to a given address on the bus
    void Write(uint16_t address, uint8_t value);

    uint8_t& InterruptEnabled()
    {
        return memory[InterruptEnabledAddress];
    }

    uint8_t& InterruptFlag()
    {
        return memory[InterruptFlagAddress];
    }

private:

    static constexpr uint16_t InterruptFlagAddress = 0xff0f; // IF
    static constexpr uint16_t InterruptEnabledAddress = 0xffff; // IE

    uint8_t memory[UINT16_MAX+1];
};
