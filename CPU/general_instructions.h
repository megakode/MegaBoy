//
// Created by sbeam on 03/12/2021.
//

#pragma once

// nop
// opcode: 00
// Cycles: 4
void CPU::NOP(){
#ifdef DEBUG_LOG
    AddDebugLog("NOP");
#endif
};

/// Strange 1 or 2 byte instruction, that is not used in any commercial game, so dont bother implementing it correctly.
/// It is described with a flowchart here:
/// https://gbdev.io/pandocs/Reducing_Power_Consumption.html
void CPU::STOP()
{
    // Do nothing
}

void CPU::disable_interrupts()
{
    interrupt_master_enabled = false;
#ifdef DEBUG_LOG
    AddDebugLog("DI");
#endif
}

void CPU::enable_interrupts()
{
    interrupt_master_enabled = true;
    ime_just_enabled = true;
#ifdef DEBUG_LOG
    AddDebugLog("EI");
#endif
}

void CPU::halt()
{
    do_halt_bug = false;

    if(interrupt_master_enabled)
    {
        // Halt is executed normally
        is_halted = true;
        just_halted = true;
    }
    else
    {
        if( ( mem.Read(IOAddress::InterruptEnabled) & mem.Read(IOAddress::InterruptFlag) & 0b11111 ) == 0)
        {
            // HALT mode is entered. It works like the IME = 1 case, but when a IF flag is set and
            // the corresponding IE flag is also set, the CPU doesn't jump to the interrupt vector, it
            // just continues executing instructions. The IF flags aren't cleared.
            is_halted = true;
            just_halted = true;
        }
        else
        {
            // HALT mode is not entered. HALT bug occurs: The CPU fails to increase PC when
            // executing the next instruction. The IF flags aren't cleared. This results on weird
            // behaviour. For example:
            is_halted = false;
            do_halt_bug = true;
        }
    }

#ifdef DEBUG_LOG
    AddDebugLog("HALT");
#endif
}

// DAA
// opcode: 0x27
// cycles: 4
// flags: Z H C
void CPU::daa(){

    if (!getFlag(FlagBitmaskN)) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
        if (getFlag(FlagBitmaskC) || regs.A > 0x99) { regs.A += 0x60; setFlag(FlagBitmaskC,true); }
        if (getFlag(FlagBitmaskHalfCarry) || (regs.A & 0x0f) > 0x09) { regs.A += 0x6; }
    } else {  // after a subtraction, only adjust if (half-)carry occurred
        if (getFlag(FlagBitmaskC)) { regs.A -= 0x60; }
        if (getFlag(FlagBitmaskHalfCarry)) { regs.A -= 0x6; }
    }

    setFlag(FlagBitmaskZero, (regs.A == 0) ); // the usual z flag
    setFlag(FlagBitmaskHalfCarry,false); // h flag is always cleared

    // Implemented according to the truth-table in the Z80 Technical manual p.141
    // test with:
    //
    // ld a,$15 ; represents 15 (decimal)
    // ld b,$27 ; represents 27 (decimal)
    // add a,b  ; a now contains $3C (not a valid BCD number)
    // daa      ; a now contains $42, representing 42 (decimal) = 15+27
    //
    // or test with:
    //
    // ld a,$09 ; represents 9 (decimal)
    // inc a    ; a now contains $0a (not a valid BCD number)
    // daa      ; a now contains $10, representing 10 (decimal) = 9+1


#ifdef DEBUG_LOG
    AddDebugLog("DAA");
#endif
}

// CPL
// Complement A (bitwise invert)
// opcode: 0x2f
// cycles: 4
// flags: N H
void CPU::cpl(){
    regs.A = ~regs.A;
    setFlag(FlagBitmaskHalfCarry,true);
    setFlag(FlagBitmaskN,true);
#ifdef DEBUG_LOG
    AddDebugLog("CPL");
#endif
}

// NEG : A = 0-A
// opcode: 0xed 0x44
// flags S Z H PV N C
// cycles: 8
void CPU::NEG()
{
    uint8_t result = 0-regs.A;
    setFlag(FlagBitmaskC,regs.A != 0);
    setFlag(FlagBitmaskHalfCarry, (0xf0 & regs.A) < (result & 0xf0) );
    setFlag(FlagBitmaskZero, result == 0);
    regs.A = result;
#ifdef DEBUG_LOG
    AddDebugLog("NEG");
#endif
}

// SCF - Set carry flag
// opcode: 0x37
// cycles: 4
// flags: C H N
void CPU::scf()
{
    setFlag(FlagBitmaskC,true);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskN,0);
#ifdef DEBUG_LOG
    AddDebugLog("SCF");
#endif
}

// CCF - invert carry flag
// opcode: 0x3f
// cycles: 4
// flags: C N H
void CPU::ccf()
{
    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,!(regs.F & FlagBitmaskC));
#ifdef DEBUG_LOG
    AddDebugLog("CCF");
#endif
}
