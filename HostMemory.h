//
// Created by sbeam on 09/12/2021.
//

#pragma once

#include <cstdint>

enum Interrupt_Flag: uint8_t
{
    Interrupt_Flag_VBlank = 1,
    Interrupt_Flag_LCD_Stat = 1 << 1,
    Interrupt_Flag_Timer = 1 << 2,
    Interrupt_Flag_Serial = 1 << 3,
    Interrupt_Flag_Joypad = 1 << 4
};

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



    void SetInterruptFlag( Interrupt_Flag flag , bool enabled )
    {
        if( enabled ){
            memory[InterruptFlagAddress] |= static_cast<uint8_t>(flag);
        } else {
            memory[InterruptFlagAddress] &= ~static_cast<uint8_t>(flag);
        }
    }

    bool GetInterruptFlag( Interrupt_Flag flag )
    {
        return memory[InterruptFlagAddress] & flag;
    }

private:

    static constexpr uint16_t InterruptFlagAddress = 0xff0f; // IF
    static constexpr uint16_t InterruptEnabledAddress = 0xffff; // IE

    uint8_t memory[UINT16_MAX+1];
};
