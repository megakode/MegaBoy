//
// Created by sbeam on 09/12/2021.
//

#include "Gameboy.h"

void Gameboy::Step() {

    uint16_t cycles = cpu.step();

    timer.Step(cycles);
    lcd.Step(cycles);

    // handle interrupts

    HandleInterrupts();

    // print debug rom output

    if (cpu.mem[0xff02] == 0x81) {
        char c = cpu.mem[0xff01];
        std::cout << c << std::endl; // printf("%c", c);
        cpu.mem[0xff02] = 0x0;
    }


}

void Gameboy::HandleInterrupts() {

    static const uint16_t irqJumpAddrs[] = { 0x40, 0x48, 0x50, 0x58, 0x60 };
    static const Interrupt_Flag flags[] = { Interrupt_Flag_VBlank, Interrupt_Flag_LCD_Stat, Interrupt_Flag_Timer, Interrupt_Flag_Serial, Interrupt_Flag_Joypad};

    // Interrupt handling should take ~5 cycles
    // TODO: wait some cycles

    if(cpu.interrupt_master_enabled)
    {
        // Priority:
        // Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
        // Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
        // Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
        // Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
        // Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)

        for(uint8_t i = 0 ; i < 5 ; i++ )
        {
            if(mem.InterruptEnabled() & mem.GetInterruptFlag(flags[i]) )
            {
                cpu.is_halted = false;
                cpu.push_pc();
                // Clear the given interrupt flag again
                mem.SetInterruptFlag(flags[i],false);
                cpu.regs.PC = irqJumpAddrs[i];
            }
        }

    }

}

void Gameboy::Start() {

    // Set interrupt flag when timer overflows
    timer.overflow_delegate = [&](){
        mem.SetInterruptFlag(Interrupt_Flag_Timer,true);
    };

}
