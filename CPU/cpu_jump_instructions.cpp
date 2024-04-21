//
// Created by sbeam on 03/12/2021.
//
#include "cpu.h"

/// jp cc nn
/// cycles: 10
void CPU::JP_cc_nn()
{
    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if (is_condition_true(conditionCode))
    {
        regs.PC = location;
    }
}

// JR NC - Jump if not carry
// opcode: 0x30
void CPU::JR_nc()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    if (!(regs.F & FlagBitmaskC))
    {
        regs.PC += offset;
    }
}

// JR C - jump if carry flag is set
// opcode: 0x38
// flags: -
void CPU::JR_c()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    if ((regs.F & FlagBitmaskC))
    {
        regs.PC += offset;
    }
}

/// jr z,n
/// opcode: 0x28
/// cycles: 12/7
/// flags: -
void CPU::JR_z()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    if ((regs.F & FlagBitmaskZero))
    {
        regs.PC += offset;
    }
}

/// JR NZ
/// opcode: 0x20
/// cycles: 12/7
void CPU::JR_nz()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    if (!(regs.F & FlagBitmaskZero))
    {                      // If not zero
        regs.PC += offset; // start calculation from beginning of this instruction
    }
}

/// jr n - Jump relative
/// opcode: 0x18
/// cycles: 12
void CPU::JR_n()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    regs.PC += offset;
}

/// JP nn - Jump absolute
/// opcode: 0xc3
/// cycles: 10
void CPU::JP_nn()
{
    regs.PC = fetch16BitValue();
}

// JP HL
// opcode: 0xe9
// cycles: 4
// flags: -
void CPU::JP_pHL()
{
    regs.PC = regs.HL;
}

// CALL cc,nn
// opcode: 0xcc nn nn
// cycles: 17 (true) 10 (false)
// flags: -
void CPU::CALL_cc_nn()
{
    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if (is_condition_true(conditionCode))
    {
        mem.Write(--regs.SP, regs.PC >> 8);                  // (SP-1) = PC_h
        mem.Write(--regs.SP, static_cast<uint8_t>(regs.PC)); // (SP-2) = PC_h
        regs.PC = location;
    }
}

// CALL
// opcode: 0xCD
// cycles: 17
void CPU::CALL()
{
    uint16_t location = fetch16BitValue();
    mem.Write(--regs.SP, regs.PC >> 8);                  // (SP-1) = PC_h
    mem.Write(--regs.SP, static_cast<uint8_t>(regs.PC)); // (SP-2) = PC_h
    regs.PC = location;
}

// RET
// PC_l = (SP), PC_h = (SP+1)
// opcode: 0xc9
// cycles: 10
// flags: -
void CPU::RET()
{
    uint8_t lobyte = mem.Read(regs.SP++);
    uint8_t hibyte = mem.Read(regs.SP++);
    regs.PC = (hibyte << 8) + lobyte;
}

// RETI
// opcode: 0xed 0x4d
// cycles: 14
// flags: -
void CPU::RETI()
{
    interrupt_master_enabled = true;
    RET();
}

// RET cc (NZ/Z/NC/C/PO/PE/P/N)
// cycles: 11 (true) 5 (false)
// flags: -
void CPU::RET_cc()
{

    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    bool conditionValue = false;

    switch (conditionCode)
    {
    case 0:
        conditionValue = !(regs.F & FlagBitmaskZero);
        break; // Non zero
    case 1:
        conditionValue = (regs.F & FlagBitmaskZero);
        break; // zero
    case 2:
        conditionValue = !(regs.F & FlagBitmaskC);
        break; // non carry
    case 3:
        conditionValue = (regs.F & FlagBitmaskC);
        break; // carry
    default:
        break;
    }

    if (conditionValue)
    {
        uint8_t lobyte = mem.Read(regs.SP++);
        uint8_t hibyte = mem.Read(regs.SP++);
        regs.PC = (hibyte << 8) + lobyte;
    }
}

// RST
//
// current PC is pushed onto stack, and PC is reset to an offset based on t,
// where t is contained in the opcode: (11 ttt 111).
//
// cycles: 11
void CPU::RST()
{
    uint8_t location[] = {0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};
    uint8_t locationCode = (current_opcode & 0b00111000) >> 3;

    push_pc();

    regs.PC = location[locationCode];
}