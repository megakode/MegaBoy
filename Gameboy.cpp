//
// Created by sbeam on 09/12/2021.
//

#include "Gameboy.h"

void Gameboy::Step() {

    uint16_t cycles = cpu.step();

    timer.Step(cycles);

    // handle interrupts

    HandleInterrupts();

}

void Gameboy::HandleInterrupts() {

    static const uint16_t irqJumpAddrs[] = { 0x40, 0x48, 0x50, 0x58, 0x60 };

    // Interrupt handling should take ~5 cycles
    // TODO: wait some cycles

    if(ime)
    {
        // Priority:
        // Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
        // Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
        // Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
        // Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
        // Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)

        for(int i = 0 ; i < 5 ; i++ )
        {
            if(mem.InterruptEnabled() & mem.InterruptFlag() & (1<<i) )
            {
                ime = false;
                cpu.push_pc();
                // Clear the given interrupt flag again
                mem.InterruptFlag() = mem.InterruptFlag() & ~(1<<i);
                cpu.regs.PC = irqJumpAddrs[i];
            }
        }

    }

}

void Gameboy::Start() {

}