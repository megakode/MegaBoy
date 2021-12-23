//
// Created by sbeam on 09/12/2021.
//

#pragma once

#include "CPU/cpu.h"
#include "HostMemory.h"
#include "Timer.h"
#include "LCD/lcd.h"

class Gameboy {

public:

    bool IsDoubleSpeedMode = false;

    HostMemory mem{};
    CPU cpu;
    Timer timer;
    LCD lcd;

    /// FFFF - IE - Interrupt Enable (R/W)
    ///
    /// Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
    /// Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
    /// Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
    /// Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
    /// Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)

    /// FF0F - IF Interrupt Flag (R/W)
    ///
    /// When an interrupt signal changes from low to high, the corresponding bit in the IF register becomes set.
    /// For example, Bit 0 becomes set when the LCD controller enters the VBlank period.
    ///
    /// Any set bits in the IF register are only requesting an interrupt to be executed.
    /// The actual execution happens only if both the IME flag and the corresponding bit in the IE register are set,
    /// otherwise the interrupt “waits” until both IME and IE allow its execution.
    ///
    enum InterruptFlag : uint8_t
    {
        InterruptFlagVBlank  = 1 << 0,
        InterruptFlagLCDSTAT = 1 << 1,
        InterruptFlagTimer   = 1 << 2,
        InterruptFlagSerial  = 1 << 3,
        InterruptFlagJoypad  = 1 << 4,
    };

    Gameboy() : cpu(mem), lcd(mem) {

    }

    void Start();

    void Step();
    void HandleInterrupts();

};
