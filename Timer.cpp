//
// Created by sbeam on 11/12/2021.
//

#include "Timer.h"

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

void Timer::Reset()
{
    divider_register = 0;
    timer_control_register = 0;
    timer_counter = 0;
    timer_modulo = 0;
}

void Timer::Step(uint16_t ticks)
{
    for (int i = 0; i < ticks; i++)
    {
        Step();
    }
}

void Timer::Step()
{
    // divider is ALWAYS increasing, regardless of TimerEnabled state in control register
    uint16_t previous_divider_value = divider_register;
    divider_register += 1;

    mem[static_cast<uint16_t>(IOAddress::TimerModule)] = GetModulo();
    mem[static_cast<uint16_t>(IOAddress::TimerDivider)] = GetDividerRegister();
    mem[static_cast<uint16_t>(IOAddress::TimerControl)] = GetTimerControl();
    mem[static_cast<uint16_t>(IOAddress::TimerCounter)] = GetCounter();

    DividerWasUpdated(previous_divider_value, divider_register);

    mem[static_cast<uint16_t>(IOAddress::TimerModule)] = GetModulo();
    mem[static_cast<uint16_t>(IOAddress::TimerDivider)] = GetDividerRegister();
    mem[static_cast<uint16_t>(IOAddress::TimerControl)] = GetTimerControl();
    mem[static_cast<uint16_t>(IOAddress::TimerCounter)] = GetCounter();
}

void Timer::DividerWasUpdated(uint16_t previous_divider_value, uint16_t new_divider_value)
{
    if (!(timer_control_register & TimerEnabledBitmask))
    {
        return;
    }

    bool increase_timer = false;

    switch (timer_control_register & TimerMultiplierBitmask)
    {
    case 0b00: // CPU Clock / 1024 = detect overflow from bits 7-0 in divider register
        increase_timer = (previous_divider_value & 512) && !(new_divider_value & 512);
        break;
    case 0b01: // CPU Clock / 16 = detect overflow from bits 3-0 in divider register
        // increase_timer = (divider_register & 0b111) < (previous_divider_value & 0b111);
        increase_timer = (previous_divider_value & 8) && !(new_divider_value & 8);
        break;
    case 0b10: // CPU Clock / 64 = detect overflow from bits 5-0 in divider register
        // increase_timer = (divider_register & 0b11111) < (previous_divider_value & 0b11111);
        increase_timer = (previous_divider_value & 32) && !(new_divider_value & 32);
        break;
    case 0b11: // CPU Clock / 256 = detect overflow from bits 7-0 in divider register
        // increase_timer = (divider_register & 0b1111111) < (previous_divider_value & 0b1111111);
        increase_timer = (previous_divider_value & 128) && !(new_divider_value & 128);
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
}

/// FF05: TIMA - Timer counter (R/W)
void Timer::SetCounter(uint8_t value)
{
    timer_counter = value;
}

/// FF05: TIMA - Timer counter (R/W)
uint8_t Timer::GetCounter()
{
    return timer_counter;
}

/// FF06: TMA - Timer Modulo (R/W)
void Timer::SetModulo(uint8_t value)
{
    timer_modulo = value;
}

/// FF06: TMA - Timer Modulo (R/W)
uint8_t Timer::GetModulo()
{
    return timer_modulo;
}

uint8_t Timer::GetDividerRegister() const
{
    return static_cast<uint8_t>(divider_register >> 8);
}

/// The divider is reset to 0 every time its value is set. (hence the missing value parameters of this function)
void Timer::SetDividerRegister()
{
    auto previous_value = divider_register;
    divider_register = 0;
    DividerWasUpdated(previous_value, divider_register);
    // odd behaviour: if setting the divider to 0 makes one of the "detect overflow" bits in the divider go from 1 to 0,
    // it is detected as an overflow, as the circiutry probably just detect falling edge on these bits.
}

/// FF07 - TAC - Timer Control (R/W)
/// Bit  2   - Timer Enable
/// Bits 1-0 - Input Clock Select
/// 00: CPU Clock / 1024 (DMG, SGB2, CGB Single Speed Mode:   4096 Hz, SGB1:   ~4194 Hz, CGB Double Speed Mode:   8192 Hz)
/// 01: CPU Clock / 16   (DMG, SGB2, CGB Single Speed Mode: 262144 Hz, SGB1: ~268400 Hz, CGB Double Speed Mode: 524288 Hz)
/// 10: CPU Clock / 64   (DMG, SGB2, CGB Single Speed Mode:  65536 Hz, SGB1:  ~67110 Hz, CGB Double Speed Mode: 131072 Hz)
/// 11: CPU Clock / 256  (DMG, SGB2, CGB Single Speed Mode:  16384 Hz, SGB1:  ~16780 Hz, CGB Double Speed Mode:  32768 Hz)
void Timer::SetTimerControl(uint8_t value)
{
    timer_control_register = value;
}

[[nodiscard]] uint8_t Timer::GetTimerControl() const
{
    return timer_control_register;
}
