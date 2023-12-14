
//
// Created by sbeam on 03/12/2021.
//
#include <format>
#include <cstdint>
#include "disassembler.h"

#define ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(function_name, opcode_str, r_cycles, hl_cycles) \
    DisassemblyLine Disassembler::function_name(const InstructionBytes &bytes)                  \
    {                                                                                           \
        uint8_t srcRegCode = bytes.data[0] & 0b111;                                             \
        auto regName = reg_name_from_regcode(srcRegCode);                                       \
        return {                                                                                \
            .cycles = (uint8_t)(srcRegCode == RegisterCode::HLPtr ? hl_cycles : r_cycles),      \
            .text = std::format(opcode_str, regName)};                                          \
    }

// *********************************************************************************
// Compare
// *********************************************************************************

// CP r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(CP_r, "CP {}", 4, 8);

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 8
DisassemblyLine Disassembler::CP_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("CP {:#x}", bytes.data[1])};
}

// *********************************************************************************
// ADD
// *********************************************************************************

// add hl,hl
// opcode: 0x29
// cycles: 8
// flags: C H N
DisassemblyLine Disassembler::ADD_HL_HL(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = "ADD HL,HL"};
}

// ADD HL,BC
// opcode: 0x09
// cycles: 8
// flags: H, N, C
DisassemblyLine Disassembler::ADD_HL_BC(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = "ADD HL,BC"};
}

// add hl,de
// opcode: 0x19
// cycleS: 8
// flag: C N H
DisassemblyLine Disassembler::ADD_HL_DE(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = "ADD HL,DE"};
}

// ADD HL,SP
// opcode: 0x39
// cycles: 8
// flags: - 0 H C
DisassemblyLine Disassembler::ADD_HL_SP(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = "ADD HL,SP"};
}

// ADD a,r
// cycles: 4 / 8
// flags: s z h pv n c
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(ADD_A_r, "ADD A,{}", 4, 8);

// // cycles: 8
DisassemblyLine Disassembler::ADD_A_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("ADD A,{}", bytes.data[1])};
}

ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(ADC_A_r, "ADC A,{}", 4, 8);

// // ADC A,N
// // opcode: 0xce
// // cycles: 8
DisassemblyLine Disassembler::ADC_A_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("ADC A,{}", bytes.data[1])};
}

// // *********************************************************************************
// // SUB
// // *********************************************************************************

// SUB r
// Cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(SUB_r, "SUB {}", 4, 8);

// // SUB A,n
// // opcode: 0xd6
// // cycles: 8
DisassemblyLine Disassembler::SUB_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("SUB A,{}", bytes.data[1])};
}

// SBC A,r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(SBC_r, "SBC {}", 4, 8);

// // SBC A,n
// // opcode: 0xde
// // cycles: 8
DisassemblyLine Disassembler::SBC_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("SBC A,{}", bytes.data[1])};
}

// // *********************************************************************************
// // And
// // *********************************************************************************

// AND r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(AND_r, "AND {}", 4, 8);

// AND n
// cycles: 8
DisassemblyLine Disassembler::AND_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("AND {}", bytes.data[1])};
}

// // *********************************************************************************
// // XOR
// // *********************************************************************************

// XOR r
// Cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(XOR_r, "XOR {}", 4, 8);

// XOR n
// cycles: 8
DisassemblyLine Disassembler::XOR_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("XOR {}", bytes.data[1])};
}

// // *********************************************************************************
// // Or
// // *********************************************************************************

// OR r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(OR_r, "OR {}", 4, 8);

// OR N
// opcode: 0xf6
// cycles: 8
DisassemblyLine Disassembler::OR_n(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("OR {}", bytes.data[1])};
}

// // *********************************************************************************
// // Inc / Dec
// // *********************************************************************************

// DEC SP
// opcode: 0x3b
// cycles: 8
DisassemblyLine Disassembler::DEC_SP(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC SP")};
}

// INC A
// cycles: 4
// opcode: 0x3c
// flags: -
DisassemblyLine Disassembler::INC_A(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC A")};
}

// DEC A
// cycles: 4
// opcode: 0x3d
DisassemblyLine Disassembler::DEC_A(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC A")};
}

// // dec hl
// // opcode: 0x2b
// // cycles: 8
// // flags: -
DisassemblyLine Disassembler::DEC_HL(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC HL")};
}

// // inc l
// // opcode: 0x2c
// // cycles: 4
DisassemblyLine Disassembler::INC_L(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC L")};
}
// // DEC L
// // opcode: 0x2d
// // cycles: 4
DisassemblyLine Disassembler::DEC_L(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC L")};
}

// INC SP
// opcode: 0x33
// flags: -
// cycles: 8
DisassemblyLine Disassembler::INC_SP(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC SP")};
}

// INC (HL)
// opcode: 0x34
// cycles: 12
// Z 0 H -
DisassemblyLine Disassembler::INC_pHL(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC (HL)")};
}

// DEC (HL)
// opcode: 0x35
// cycles: 12
DisassemblyLine Disassembler::DEC_pHL(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC (HL)")};
}
// // INC BC
// // Opcode: 03
// // Cycles: 8
// // Flags: -
DisassemblyLine Disassembler::INC_BC(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC BC")};
}

// // INC B
// // opcode: 04
// // cycles: 4
// // flags: S Z HC PV N
DisassemblyLine Disassembler::INC_B(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC B")};
}

// DEC B
// opcode: 0x05
// cycles: 4
DisassemblyLine Disassembler::DEC_B(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC B")};
}

// // DEC BC
// // opcode: 0x0b
// // cycles: 8
DisassemblyLine Disassembler::DEC_BC(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC BC")};
}

// // INC C
// // opcode: 0x0c
// // cycles: 4
DisassemblyLine Disassembler::INC_C(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC C")};
}

// // DEC C
// // opcode: 0x0d
// // cycles: 4
DisassemblyLine Disassembler::DEC_C(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC C")};
}

// INC DE
// Opcode: 0x13
// Cycles: 08
// Flags: -
DisassemblyLine Disassembler::INC_DE(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC DE")};
}

// // inc d
// // opcode: 0x14
DisassemblyLine Disassembler::INC_D(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC D")};
}

// // dec d
// // opcode: 0x15
DisassemblyLine Disassembler::DEC_D(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC D")};
}

// // INC HL
// // Opcode: 0x23
// // Cycles: 08
// // Flags: -
DisassemblyLine Disassembler::INC_HL(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("INC HL")};
}

// // INC H
// // opcodE: 0x24
DisassemblyLine Disassembler::INC_H(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC H")};
}

// // DEC H
// // opcode: 0x25
DisassemblyLine Disassembler::DEC_H(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC H")};
}

// // DEC DE
// // opcode: 0x1b
// // cycles: 8
DisassemblyLine Disassembler::DEC_DE(const InstructionBytes &bytes)
{
    return {.cycles = 8, .text = std::format("DEC DE")};
}

// // INC E
// // opcode: 0x1c
// // cycles 4
DisassemblyLine Disassembler::INC_E(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("INC E")};
}

// // DEC E
// // opcode: 0x1d
// // cycles: 4
DisassemblyLine Disassembler::DEC_E(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("DEC E")};
}

// /// ADD SP,dd
// /// opcode: 0xe8
// /// cycles: 16
// /// flags: 00hc
// void Disassembler::ADD_SP_s8(const InstructionBytes &bytes)
// {
//     auto value = static_cast<int8_t>(fetch8BitValue());
//     uint16_t result = regs.SP + value;
//     regs.F = 0;
//     setFlag(FlagBitmaskC,(regs.SP & 0x00FF) + (value & 0xff) > 0x00FF);
//     setFlag(FlagBitmaskHalfCarry, (regs.SP & 0x000f) + (value & 0x000f) > 0x000f);
//     regs.SP = result & 0xffff;

// }
DisassemblyLine Disassembler::ADD_SP_s8(const InstructionBytes &bytes)
{
    return {.cycles = 4, .text = std::format("ADD SP,{}", bytes.data[1])};
}