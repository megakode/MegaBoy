//
// Created by sbeam on 03/12/2021.
//

#include "cpu.h"

void CPU::add(uint8_t srcValue, bool carry)
{

    // Page 107 in z80 technical manual.

    uint16_t result = regs.A + srcValue;
    if (carry && (regs.F & FlagBitmaskC))
    {
        result++;
    }

    if (carry)
    {
        setFlag(FlagBitmaskHalfCarry, (regs.A & 0x0f) + (srcValue & 0xf) + (getFlag(FlagBitmaskC) ? 1 : 0) > 0x0f);
    }
    else
    {
        setFlag(FlagBitmaskHalfCarry, (regs.A & 0x0f) + (srcValue & 0xf) > 0x0f);
    }

    setFlag(FlagBitmaskZero, (result & 0xff) == 0);
    setFlag(FlagBitmaskC, result > 0xff);
    setFlag(FlagBitmaskN, false);

    regs.A = static_cast<uint8_t>(result);
}

//  add 16 bit register pair
void CPU::add16(uint16_t &regPair, uint16_t value_to_add)
{
    uint32_t result = regPair + value_to_add;

    setFlag(FlagBitmaskN, false);
    setFlag(FlagBitmaskHalfCarry, ((regPair & 0x0fff) + (value_to_add & 0x0fff)) > 0x0FFF);
    setFlag(FlagBitmaskC, result > 0xffff);

    regPair = result & 0xffff;
}

/**
 * Subtracts a value from register A
 * @param srcValue the value to subtract
 * @param carry whether to subtract the carry flag
 * @param onlySetFlagsForComparison Only set the flags for usage by the `cp` compare instruction and don't perform the actual subtraction
 */
void CPU::sub(uint8_t srcValue, bool carry, bool onlySetFlagsForComparison)
{

    uint16_t result = regs.A - srcValue;
    if (carry && (regs.F & FlagBitmaskC))
    {
        result--;
    }
    setFlag(FlagBitmaskZero, (result & 0xff) == 0);
    setFlag(FlagBitmaskN, true);
    setFlag(FlagBitmaskHalfCarry, ((regs.A ^ srcValue ^ result) & 0x10) != 0);
    setFlag(FlagBitmaskC, (result & 0xff00));

    if (!onlySetFlagsForComparison)
    {
        regs.A = result & 0xff; // static_cast<uint8_t>(result);
    }

    /*
     * Fra GIIBIIAdvance emu:
     * #define gb_sbc_a_r8(reg8)
    {
        u32 result = regs.A - val - ((regs.F & F_CARRY) ? 1 : 0);
        cpu->R8.F = ((result & ~0xFF) ? F_CARRY : 0)
                    | ((result & 0xFF) ? 0 : F_ZERO) | F_SUBTRACT;
        cpu->F.H = ((regs.A ^ val ^ result) & 0x10) != 0;
        regs.A = temp;
        GB_CPUClockCounterAdd(4);
    }
     */
}

void CPU::INC_r(uint8_t &reg)
{
    reg++;
    set_INC_operation_flags(reg);
}

void CPU::DEC_r(uint8_t &reg)
{
    uint8_t oldval = reg;
    reg--;
    set_DEC_operation_flags(reg);
}

// *********************************************************************************
// Compare
// *********************************************************************************

void CPU::CP_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    sub(srcRegValue, false, true);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("CP %s", regName.c_str());
#endif
}

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 7
void CPU::CP_n()
{
    uint8_t value = fetch8BitValue();
    sub(value, false, true);

#ifdef DEBUG_LOG
    AddDebugLog("CP %02x", value);
#endif
}

// *********************************************************************************
// ADD
// *********************************************************************************

// add hl,hl
// opcode: 0x29
// cycles: 11
// flags: C H N
void CPU::ADD_HL_HL()
{
    add16(regs.HL, regs.HL);
#ifdef DEBUG_LOG
    AddDebugLog("ADD HL,HL");
#endif
}

// ADD HL,BC
// opcode: 0x09
// cycles: 11
// flags: H, N, C
void CPU::ADD_HL_BC()
{
    add16(regs.HL, regs.BC);
}

// add hl,de
// opcode: 0x19
// cycleS: 11
// flag: C N H
void CPU::ADD_HL_DE()
{
    add16(regs.HL, regs.DE);
}

// ADD HL,SP
// opcode: 0x39
// cycles: 11
// flags: - 0 H C
void CPU::ADD_HL_SP()
{
    add16(regs.HL, regs.SP);
}

// ADD a,r
// cycles: 4
// flags: s z h pv n c
void CPU::ADD_A_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);

    add(srcRegValue, false);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("ADD A,%s", regName.c_str());
#endif
}

// cycles: 7
void CPU::ADD_A_n()
{
    uint8_t srcRegValue = fetch8BitValue();
    add(srcRegValue, false);
#ifdef DEBUG_LOG
    AddDebugLog("ADD A,%x", srcRegValue);
#endif
}

void CPU::ADC_A_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);

    add(srcRegValue, true);
#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("ADC A,%s", regName.c_str());
#endif
}

// ADC A,N
// opcode: 0xce
// cycles: 7
void CPU::ADC_A_n()
{
    auto value = fetch8BitValue();
    add(value, true);
#ifdef DEBUG_LOG
    AddDebugLog("ADC A,%x", value);
#endif
}

// *********************************************************************************
// SUB
// *********************************************************************************

void CPU::SUB_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    sub(srcRegValue, false, false);
}

// SUB N
// opcode: 0xd6
// cycles: 7
void CPU::SUB_n()
{
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue, false, false);
#ifdef DEBUG_LOG
    AddDebugLog("SUB A,%x", srcRegValue);
#endif
}

void CPU::SBC_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    sub(srcRegValue, true, false);
#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("SBC A,%s", regName.c_str());
#endif
}

// SBC N
// opcode: 0xde
// cycles: 7
void CPU::SBC_n()
{
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue, true, false);
#ifdef DEBUG_LOG
    AddDebugLog("SBC A,%x", srcRegValue);
#endif
}

// *********************************************************************************
// And
// *********************************************************************************

void CPU::and_a_with_value(uint8_t value)
{
    regs.A &= value;
    set_AND_operation_flags();
}

// cycles: 4
void CPU::AND_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    and_a_with_value(srcRegValue);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("AND %s", regName.c_str());
#endif
}

void CPU::AND_n()
{
    auto value = fetch8BitValue();
    and_a_with_value(value);

#ifdef DEBUG_LOG
    AddDebugLog("AND %02x", value);
#endif
}

// *********************************************************************************
// XOR
// *********************************************************************************

// cycles:
// xor r 4
// xor n 7
// xor (hl) 7
// xor (IX+d) 19
// xor (IY+d) 19
void CPU::XOR_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    xor_a_with_value(srcRegValue);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("XOR %s", regName.c_str());
#endif
}

// opcode: 0xee
// cycles: 7
void CPU::XOR_n()
{
    auto value = fetch8BitValue();
    xor_a_with_value(value);

#ifdef DEBUG_LOG
    AddDebugLog("XOR %02x", value);
#endif
}

void CPU::xor_a_with_value(uint8_t value)
{
    regs.A ^= value;

    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry, false);
    setFlag(FlagBitmaskN, false);
    setFlag(FlagBitmaskC, false);
}

// *********************************************************************************
// Or
// *********************************************************************************

// cycles:
// or r 4
// or n 7
// or (hl) 7
void CPU::OR_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = read_from_register(srcRegCode);
    or_a_with_value(srcRegValue);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("OR %s", regName.c_str());
#endif
}

// OR N
// opcode: 0xf6
// cycles: 7
void CPU::OR_n()
{
    or_a_with_value(fetch8BitValue());
}

void CPU::or_a_with_value(uint8_t value)
{
    regs.A |= value;

    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN, 0);
    setFlag(FlagBitmaskC, 0);
}

// *********************************************************************************
// Inc / Dec
// *********************************************************************************

// DEC SP
// opcode: 0x3b
void CPU::DEC_SP()
{
    regs.SP--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC SP");
#endif
}

// opcode: 0x3c
// flags: -
void CPU::INC_A()
{
    INC_r(regs.A);
#ifdef DEBUG_LOG
    AddDebugLog("INC A");
#endif
}

// opcode: 0x3d
void CPU::DEC_A()
{
    DEC_r(regs.A);
#ifdef DEBUG_LOG
    AddDebugLog("DEC A");
#endif
}

// dec hl
// opcode: 0x2b
// cycles: 6
// flags: -
void CPU::DEC_HL()
{
    regs.HL--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC HL");
#endif
}

// inc l
// opcode: 0x2c
// cycles: 4
void CPU::INC_L()
{
    INC_r(regs.L);
#ifdef DEBUG_LOG
    AddDebugLog("INC L");
#endif
}

// DEC L
// opcode: 0x2d
// cycles: 4
void CPU::DEC_L()
{
    DEC_r(regs.L);
#ifdef DEBUG_LOG
    AddDebugLog("DEC L");
#endif
}

// INC SP
// opcode: 0x33
// flags: -
void CPU::INC_SP()
{
    regs.SP++;
#ifdef DEBUG_LOG
    AddDebugLog("INC SP");
#endif
}

// INC (HL)
// opcode: 0x34
// Z 0 H -
void CPU::INC_pHL()
{
    uint8_t result = mem.Read(regs.HL);
    mem.Write(regs.HL, ++result);
    set_INC_operation_flags(result);
}

// DEC (HL)
// opcode: 0x35
void CPU::DEC_pHL()
{
    uint8_t result = mem.Read(regs.HL);
    mem.Write(regs.HL, --result);
    set_DEC_operation_flags(result);
}

// INC BC
// Opcode: 03
// Cycles: 06
// Flags: -
void CPU::INC_BC()
{
    regs.BC++;
#ifdef DEBUG_LOG
    AddDebugLog("INC BC");
#endif
}

// inc b
// opcode: 04
// cycles: 4
// flags: S Z HC PV N
void CPU::INC_B()
{
    INC_r(regs.B);
#ifdef DEBUG_LOG
    AddDebugLog("INC B");
#endif
}

// dec b
// opcode: 0x05
// cycles: 4
void CPU::DEC_B()
{
    DEC_r(regs.B);
#ifdef DEBUG_LOG
    AddDebugLog("DEC B");
#endif
}

// DEC BC
// opcode: 0x0b
// cycles: 6
void CPU::DEC_BC()
{
    regs.BC--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC BC");
#endif
}

// INC C
// opcode: 0x0c
// cycles: 4
void CPU::INC_C()
{
    INC_r(regs.C);
#ifdef DEBUG_LOG
    AddDebugLog("INC C");
#endif
}

// DEC C
// opcode: 0x0d
// cycles: 4
void CPU::DEC_C()
{
    DEC_r(regs.C);
#ifdef DEBUG_LOG
    AddDebugLog("DEC C");
#endif
}

// inc de
// Opcode: 0x13
// Cycles: 06
// Flags: -
void CPU::INC_DE()
{
    regs.DE++;
#ifdef DEBUG_LOG
    AddDebugLog("INC DE");
#endif
}

// inc d
// opcode: 0x14
void CPU::INC_D()
{
    INC_r(regs.D);
#ifdef DEBUG_LOG
    AddDebugLog("INC D");
#endif
}

// dec d
// opcode: 0x15
void CPU::DEC_D()
{
    DEC_r(regs.D);
#ifdef DEBUG_LOG
    AddDebugLog("DEC D");
#endif
}

// INC HL
// Opcode: 0x23
// Cycles: 06
// Flags: -
void CPU::INC_HL()
{
    regs.HL++;
#ifdef DEBUG_LOG
    AddDebugLog("INC HL");
#endif
}

// INC H
// opcodE: 0x24
void CPU::INC_H()
{
    INC_r(regs.H);
#ifdef DEBUG_LOG
    AddDebugLog("INC H");
#endif
}

// DEC H
// opcode: 0x25
void CPU::DEC_H()
{
    DEC_r(regs.H);
#ifdef DEBUG_LOG
    AddDebugLog("DEC H");
#endif
}

// DEC DE
// opcode: 0x1b
// cycles: 6
void CPU::DEC_DE()
{
    regs.DE--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC DE");
#endif
}

// INC E
// opcode: 0x1c
// cycles 4
void CPU::INC_E()
{
    INC_r(regs.E);
#ifdef DEBUG_LOG
    AddDebugLog("INC E");
#endif
}

// DEC E
// opcode: 0x1d
// cycles: 4
void CPU::DEC_E()
{
    DEC_r(regs.E);
#ifdef DEBUG_LOG
    AddDebugLog("DEC E");
#endif
}

/// ADD SP,dd
/// opcode: 0xe8
/// cycles: 16
/// flags: 00hc
void CPU::ADD_SP_s8()
{
    auto value = static_cast<int8_t>(fetch8BitValue());
    uint16_t result = regs.SP + value;
    regs.F = 0;
    setFlag(FlagBitmaskC, (regs.SP & 0x00FF) + (value & 0xff) > 0x00FF);
    setFlag(FlagBitmaskHalfCarry, (regs.SP & 0x000f) + (value & 0x000f) > 0x000f);
    regs.SP = result & 0xffff;
}