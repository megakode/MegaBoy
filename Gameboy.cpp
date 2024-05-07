//
// Created by sbeam on 09/12/2021.
//

#include "Gameboy.h"

Gameboy::Gameboy() : mem(cartridge), cpu(mem), lcd(mem), dma(mem), joypad(mem), timer(mem)
{
    // Set interrupt flag when timer overflows
    timer.didOverflow = [&]()
    {
        mem.SetInterruptFlag(Interrupt_Flag_Timer, true);
    };

    mem.didWriteToIOAddress = [&](uint16_t addr, uint8_t value)
    {
        switch (addr)
        {
        case (uint16_t)IOAddress::TimerCounter:
            timer.SetCounter(value);
            break;
        case (uint16_t)IOAddress::TimerControl:
            timer.SetTimerControl(value);
            break;
        case (uint16_t)IOAddress::TimerDivider:
            timer.SetDividerRegister();
            break;
        case (uint16_t)IOAddress::TimerModule:
            timer.SetModulo(value);
            break;
        case (uint16_t)IOAddress::DMATransferStartAddress:
            // Initiate DMA transfer
            dma.RequestTransfer(value);
            break;
        case (uint16_t)IOAddress::Joypad:
            joypad.WriteRegisterData(value);
            break;
        default:
            break;
        }
    };
}

void Gameboy::Reset()
{
    cpu.reset();
    timer.Reset();
    dma.Reset();
    mem[static_cast<uint16_t>(IOAddress::InterruptEnabled)] = 0;
    mem[static_cast<uint16_t>(IOAddress::InterruptFlag)] = 0;
}

///
/// \return Number of cycles the step took
uint16_t Gameboy::Step()
{

    mem[static_cast<uint16_t>(IOAddress::Joypad)] = joypad.ReadRegisterData();

    // handle interrupts
    HandleInterrupts();

    timer.Step(1);

    uint16_t cycles = cpu.step();

    timer.Step(cycles - 1);
    // timer.Step(cycles);

    lcd.Step(cycles);
    dma.Step(cycles);

    // print debug rom output

    if (cpu.mem[0xff02] == 0x81)
    {
        char c = cpu.mem[0xff01];
        printf("%c", c);
        // std::cout << c << std::endl; //
        cpu.mem[0xff02] = 0x0;
    }

    return cycles;
}

void Gameboy::HandleInterrupts()
{

    static const uint16_t irqJumpAddrs[] = {0x40, 0x48, 0x50, 0x58, 0x60};
    static const Interrupt_Flag flags[] = {Interrupt_Flag_VBlank, Interrupt_Flag_LCD_Stat, Interrupt_Flag_Timer, Interrupt_Flag_Serial, Interrupt_Flag_Joypad};

    // Interrupt handling should take ~5 cycles
    // TODO: wait some cycles

    // Priority:
    // Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
    // Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
    // Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
    // Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
    // Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)

    for (uint8_t i = 0; i < 5; i++)
    {
        auto ie = mem.Read(IOAddress::InterruptEnabled);
        auto irqf = mem.Read(IOAddress::InterruptFlag);
        if (mem.Read(IOAddress::InterruptEnabled) & mem.Read(IOAddress::InterruptFlag) & flags[i])
        {

            // TODO: maybe the one-instruction delay here (with !ime_just_enabled) must not there when an IRQ is waking up the cpu from HALT mode?
            if (cpu.interrupt_master_enabled && !cpu.ime_just_enabled && !cpu.is_halted)
            {
                cpu.interrupt_master_enabled = false;
                cpu.push_pc();
                // Clear the given interrupt flag again
                mem.SetInterruptFlag(flags[i], false);
                cpu.regs.PC = irqJumpAddrs[i];
            }

            cpu.is_halted = false;
            break;
        }
    }

    if (cpu.ime_just_enabled)
    {
        cpu.ime_just_enabled = false;
    }
}

void Gameboy::Start()
{
}
