//
// Created by sbeam on 11/12/2021.
//
#include <chrono>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include "HostMemory.h"

#pragma once

/// Based on the most detailed description of the GB timing system i could find: https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf
class Timer
{

private:
    /// 0xFF04: Divider register value (internally in the GB it is a 16 bit value, but only the upper 8 bits are mapped to memory at 0xFF04
    uint16_t divider_register = 0;

    /// 0xFF07: Timer control register
    uint8_t timer_control_register = 0;

    /// FF05: TIMA - Timer counter (R/W)
    /// This timer is incremented at the clock frequency specified by the TAC register ($FF07).
    /// When the value overflows (exceeds $FF) it is reset to the value specified in TMA (FF06) and an interrupt is requested, as described below.
    uint8_t timer_counter = 0;

    /// FF06: TMA - Timer Modulo (R/W)
    /// When TIMA overflows, it is reset to the value in this register and an interrupt is requested.
    /// Example of use: if TMA is set to $FF, an interrupt is requested at the clock frequency selected in TAC (because every increment is an overflow).
    /// However, if TMA is set to $FE, an interrupt is only requested every two increments, which effectively divides the selected clock by two.
    /// Setting TMA to $FD would divide the clock by three, and so on.
    ///
    /// If a TMA write is executed on the same cycle as the content of TMA is transferred to TIMA due to a timer overflow, the old value is transferred to TIMA.
    ///
    uint8_t timer_modulo = 0;

    constexpr static uint8_t TimerEnabledBitmask = 0b100;
    constexpr static uint8_t TimerMultiplierBitmask = 0b011;

    HostMemory &mem;

public:
    std::function<void()> didOverflow = nullptr;

    Timer() = delete;

    explicit Timer(HostMemory &mem) : mem(mem)
    {
    }

    void Reset()
    {
        divider_register = 0;
        timer_control_register = 0;
        timer_counter = 0;
        timer_modulo = 0;
    }

    void Step(uint16_t ticks)
    {

        // divider is ALWAYS increasing, regardless of TimerEnabled state in control register
        uint16_t previous_divider_value = divider_register;
        divider_register += ticks;

        mem[static_cast<uint16_t>(IOAddress::TimerModule)] = GetModulo();
        mem[static_cast<uint16_t>(IOAddress::TimerDivider)] = GetDividerRegister();
        mem[static_cast<uint16_t>(IOAddress::TimerControl)] = GetTimerControl();
        mem[static_cast<uint16_t>(IOAddress::TimerCounter)] = GetCounter();

        if (!(timer_control_register & TimerEnabledBitmask))
        {
            return;
        }

        bool increase_timer = false;

        switch (timer_control_register & TimerMultiplierBitmask)
        {
        case 0b00: // CPU Clock / 1024 = detect overflow from bits 7-0 in divider register
            increase_timer = (previous_divider_value & 512) && !(divider_register & 512);
            break;
        case 0b01: // CPU Clock / 16 = detect overflow from bits 3-0 in divider register
            // increase_timer = (divider_register & 0b111) < (previous_divider_value & 0b111);
            increase_timer = (previous_divider_value & 8) && !(divider_register & 8);
            break;
        case 0b10: // CPU Clock / 64 = detect overflow from bits 5-0 in divider register
            // increase_timer = (divider_register & 0b11111) < (previous_divider_value & 0b11111);
            increase_timer = (previous_divider_value & 32) && !(divider_register & 32);
            break;
        case 0b11: // CPU Clock / 256 = detect overflow from bits 7-0 in divider register
            // increase_timer = (divider_register & 0b1111111) < (previous_divider_value & 0b1111111);
            increase_timer = (previous_divider_value & 128) && !(divider_register & 128);
            break;
        }

        if (increase_timer)
        {
            uint8_t last_timer_counter = timer_counter;
            timer_counter++;

            // Did timer overflow?
            if (timer_counter < last_timer_counter)
            {
                timer_counter = timer_modulo;
                // Set timer interrupt flag (Request timer interrupt)
                if (didOverflow != nullptr)
                {
                    didOverflow();
                }
            }
        }

        mem[static_cast<uint16_t>(IOAddress::TimerModule)] = GetModulo();
        mem[static_cast<uint16_t>(IOAddress::TimerDivider)] = GetDividerRegister();
        mem[static_cast<uint16_t>(IOAddress::TimerControl)] = GetTimerControl();
        mem[static_cast<uint16_t>(IOAddress::TimerCounter)] = GetCounter();
    }

    /// FF05: TIMA - Timer counter (R/W)
    void SetCounter(uint8_t value)
    {
        timer_counter = value;
    }

    /// FF05: TIMA - Timer counter (R/W)
    uint8_t GetCounter()
    {
        return timer_counter;
    }

    /// FF06: TMA - Timer Modulo (R/W)
    void SetModulo(uint8_t value)
    {
        timer_modulo = value;
    }

    /// FF06: TMA - Timer Modulo (R/W)
    uint8_t GetModulo()
    {
        return timer_modulo;
    }

    [[nodiscard]] uint8_t GetDividerRegister() const
    {
        return static_cast<uint8_t>(divider_register >> 8);
    }

    /// The divider is reset to 0 every time its value is set. (hence the missing value parameters of this function)
    void SetDividerRegister()
    {
        divider_register = 0;
    }

    /// FF07 - TAC - Timer Control (R/W)
    /// Bit  2   - Timer Enable
    /// Bits 1-0 - Input Clock Select
    /// 00: CPU Clock / 1024 (DMG, SGB2, CGB Single Speed Mode:   4096 Hz, SGB1:   ~4194 Hz, CGB Double Speed Mode:   8192 Hz)
    /// 01: CPU Clock / 16   (DMG, SGB2, CGB Single Speed Mode: 262144 Hz, SGB1: ~268400 Hz, CGB Double Speed Mode: 524288 Hz)
    /// 10: CPU Clock / 64   (DMG, SGB2, CGB Single Speed Mode:  65536 Hz, SGB1:  ~67110 Hz, CGB Double Speed Mode: 131072 Hz)
    /// 11: CPU Clock / 256  (DMG, SGB2, CGB Single Speed Mode:  16384 Hz, SGB1:  ~16780 Hz, CGB Double Speed Mode:  32768 Hz)
    void SetTimerControl(uint8_t value)
    {
        timer_control_register = value;
    }

    [[nodiscard]] uint8_t GetTimerControl() const
    {
        return timer_control_register;
    }
};
