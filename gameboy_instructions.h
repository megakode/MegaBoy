//
// Special Gameboy CPU instructions
// Created by sbeam on 08/12/2021.
//

#pragma once

/// Strange 1 or 2 byte instruction, that is not used in any commercial game, so dont bother implementing it correctly.
/// It is described with a flowchart here:
/// https://gbdev.io/pandocs/Reducing_Power_Consumption.html
void CPU::STOP()
{
    // Do nothing
}

/// LDI  (HL),A
/// opcode: 22
/// cycles: 8
void CPU::LDI_pHL_A()
{
    mem[regs.HL++] = regs.A;
}

/// LDI  A,(HL)
/// opcode: 2a
/// cycles: 8
void CPU::LDI_A_pHL()
{
    regs.A = mem[regs.HL++];
}

/// LDD (HL),A
/// opcode: 0x32
/// cycles: 8
void CPU::LDD_pHL_A()
{
    mem[regs.HL--] = regs.A;
}

/// LDD  A,(HL)
/// opcode: 0x3a
/// cycles: 8
void CPU::LDD_A_pHL()
{
    regs.A = mem[regs.HL--];
}

/// LD (0xff00 + n),A
/// opcode: e0 nn
/// cycles: 8
/// flags: -
void CPU::LD_ff00n_A()
{
    uint8_t lowbyte = fetch8BitValue();
    uint16_t addr = 0xff00 + lowbyte;
    mem[addr] = regs.A;
}

/// LD (FF00+C),A
/// opcode: 0xe2
void CPU::LD_ff00C_A()
{
    uint16_t addr = 0xff00 + regs.C;
    mem[addr] = regs.A;
}

/// LD A,(0xff00 + n)
/// opcode: f0 nn
/// cycles: 8
/// flags: -
void CPU::LD_A_ff00n()
{
    uint8_t lowbyte = fetch8BitValue();
    uint16_t addr = 0xff00 + lowbyte;
    regs.A = mem[addr];
}

/// LD A,(FF00+C)
/// opcode: 0xe2
void CPU::LD_A_ff00C()
{
    uint16_t addr = 0xff00 + regs.C;
    regs.A = mem[addr];
}

/// ADD SP,dd
/// opcode: 0xe8
/// cycles: 16
/// flags: 00hc
void CPU::ADD_SP_s8()
{
    auto value = static_cast<int8_t>(fetch8BitValue());
    uint32_t result = specialRegs.SP + value;
    regs.F = 0;
    setFlag(FlagBitmaskC, result & 0xff0000);
    setFlag(FlagBitmaskHalfCarry, (specialRegs.SP & 0x0fffu) > (result & 0x0fffu));
}

/// LD HL,SP+dd
/// HL = SP +/- dd ; dd is 8-bit signed number
/// cycles: 12
void CPU::LD_HL_SPs8()
{
    auto value = static_cast<int8_t>(fetch8BitValue());
    regs.HL = specialRegs.SP + value;
}

/// LD A,(nn)
/// opcode: 0xfa
/// cycles: 16
void CPU::LD_A_pnn()
{
    regs.A = mem[fetch16BitValue()];
}

//: opcode: 0xCB 0x30-0x37
/// SWAP r
/// swap low/hi-nibble
void CPU::SWAP_r(uint8_t regCode)
{
    uint8_t& reg = reg_from_regcode(regCode);
    uint8_t temp = (reg & 0xf) << 4;
    reg >>= 4;
    reg &= temp;
    regs.F = 0;
    setFlag(FlagBitmaskZero,reg == 0);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(regCode);
    AddDebugLog(std::format("SWAP {}",regName));
#endif
}

// TODO: implement all gameboy cpu codes:
// https://gbdev.io/pandocs/CPU_Comparison_with_Z80.html
