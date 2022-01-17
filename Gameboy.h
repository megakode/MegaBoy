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

    /*
    enum InterruptFlag : uint8_t
    {
        InterruptFlagVBlank  = 1 << 0,
        InterruptFlagLCDSTAT = 1 << 1,
        InterruptFlagTimer   = 1 << 2,
        InterruptFlagSerial  = 1 << 3,
        InterruptFlagJoypad  = 1 << 4,
    };
*/
    Gameboy();

    void Start();

    void Step();
    void HandleInterrupts();

};
