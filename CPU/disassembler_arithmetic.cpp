
//
// Created by sbeam on 03/12/2021.
//
#include <format>
#include <cstdint>
#include "disassembler.h"

#define ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(function_name, opcode_str) \
    std::string Disassembler::function_name(const InstructionBytes &bytes) \
    {                                                                      \
        uint8_t srcRegCode = bytes.data[0] & 0b111;                        \
        auto regName = reg_name_from_regcode(srcRegCode);                  \
        return std::format(opcode_str, regName);                           \
    }

// *********************************************************************************
// Compare
// *********************************************************************************

// CP r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(CP_r, "CP {}");

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 8
std::string Disassembler::CP_n(const InstructionBytes &bytes)
{
    return std::format("CP {:#x}", bytes.data[1]);
}

// *********************************************************************************
// ADD
// *********************************************************************************

// add hl,hl
// opcode: 0x29
// cycles: 8
// flags: C H N
std::string Disassembler::ADD_HL_HL(const InstructionBytes &bytes)
{
    return "ADD HL,HL";
}

// ADD HL,BC
// opcode: 0x09
// cycles: 8
// flags: H, N, C
std::string Disassembler::ADD_HL_BC(const InstructionBytes &bytes)
{
    return "ADD HL,BC";
}

// add hl,de
// opcode: 0x19
// cycleS: 8
// flag: C N H
std::string Disassembler::ADD_HL_DE(const InstructionBytes &bytes)
{
    return "ADD HL,DE";
}

// ADD HL,SP
// opcode: 0x39
// cycles: 8
// flags: - 0 H C
std::string Disassembler::ADD_HL_SP(const InstructionBytes &bytes)
{
    return "ADD HL,SP";
}

// ADD a,r
// cycles: 4 / 8
// flags: s z h pv n c
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(ADD_A_r, "ADD A,{}");

// // cycles: 8
std::string Disassembler::ADD_A_n(const InstructionBytes &bytes)
{
    return std::format("ADD A,{}", bytes.data[1]);
}

ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(ADC_A_r, "ADC A,{}");

// // ADC A,N
// // opcode: 0xce
// // cycles: 8
std::string Disassembler::ADC_A_n(const InstructionBytes &bytes)
{
    return std::format("ADC A,{}", bytes.data[1]);
}

// // *********************************************************************************
// // SUB
// // *********************************************************************************

// SUB r
// Cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(SUB_r, "SUB {}");

// // SUB A,n
// // opcode: 0xd6
// // cycles: 8
std::string Disassembler::SUB_n(const InstructionBytes &bytes)
{
    return std::format("SUB A,{}", bytes.data[1]);
}

// SBC A,r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(SBC_r, "SBC {}");

// // SBC A,n
// // opcode: 0xde
// // cycles: 8
std::string Disassembler::SBC_n(const InstructionBytes &bytes)
{
    return std::format("SBC A,{}", bytes.data[1]);
}

// // *********************************************************************************
// // And
// // *********************************************************************************

// AND r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(AND_r, "AND {}");

// AND n
// cycles: 8
std::string Disassembler::AND_n(const InstructionBytes &bytes)
{
    return std::format("AND {}", bytes.data[1]);
}

// // *********************************************************************************
// // XOR
// // *********************************************************************************

// XOR r
// Cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(XOR_r, "XOR {}");

// XOR n
// cycles: 8
std::string Disassembler::XOR_n(const InstructionBytes &bytes)
{
    return std::format("XOR {}", bytes.data[1]);
}

// // *********************************************************************************
// // Or
// // *********************************************************************************

// OR r
// cycles: 4 / 8
ARITHMETIC_FUNCTION_WITH_REGISTER_PARAM(OR_r, "OR {}");

// OR N
// opcode: 0xf6
// cycles: 8
std::string Disassembler::OR_n(const InstructionBytes &bytes)
{
    return std::format("OR {}", bytes.data[1]);
}

// // *********************************************************************************
// // Inc / Dec
// // *********************************************************************************

// DEC SP
// opcode: 0x3b
// cycles: 8
std::string Disassembler::DEC_SP(const InstructionBytes &bytes)
{
    return std::format("DEC SP");
}

// INC A
// cycles: 4
// opcode: 0x3c
// flags: -
std::string Disassembler::INC_A(const InstructionBytes &bytes)
{
    return std::format("INC A");
}

// DEC A
// cycles: 4
// opcode: 0x3d
std::string Disassembler::DEC_A(const InstructionBytes &bytes)
{
    return std::format("DEC A");
}

// // dec hl
// // opcode: 0x2b
// // cycles: 8
// // flags: -
std::string Disassembler::DEC_HL(const InstructionBytes &bytes)
{
    return std::format("DEC HL");
}

// // inc l
// // opcode: 0x2c
// // cycles: 4
std::string Disassembler::INC_L(const InstructionBytes &bytes)
{
    return std::format("INC L");
}
// // DEC L
// // opcode: 0x2d
// // cycles: 4
std::string Disassembler::DEC_L(const InstructionBytes &bytes)
{
    return std::format("DEC L");
}

// INC SP
// opcode: 0x33
// flags: -
// cycles: 8
std::string Disassembler::INC_SP(const InstructionBytes &bytes)
{
    return std::format("INC SP");
}

// INC (HL)
// opcode: 0x34
// cycles: 12
// Z 0 H -
std::string Disassembler::INC_pHL(const InstructionBytes &bytes)
{
    return std::format("INC (HL)");
}

// DEC (HL)
// opcode: 0x35
// cycles: 12
std::string Disassembler::DEC_pHL(const InstructionBytes &bytes)
{
    return std::format("DEC (HL)");
}
// // INC BC
// // Opcode: 03
// // Cycles: 8
// // Flags: -
std::string Disassembler::INC_BC(const InstructionBytes &bytes)
{
    return std::format("INC BC");
}

// // INC B
// // opcode: 04
// // cycles: 4
// // flags: S Z HC PV N
std::string Disassembler::INC_B(const InstructionBytes &bytes)
{
    return std::format("INC B");
}

// DEC B
// opcode: 0x05
// cycles: 4
std::string Disassembler::DEC_B(const InstructionBytes &bytes)
{
    return std::format("DEC B");
}

// // DEC BC
// // opcode: 0x0b
// // cycles: 8
std::string Disassembler::DEC_BC(const InstructionBytes &bytes)
{
    return std::format("DEC BC");
}

// // INC C
// // opcode: 0x0c
// // cycles: 4
std::string Disassembler::INC_C(const InstructionBytes &bytes)
{
    return std::format("INC C");
}

// // DEC C
// // opcode: 0x0d
// // cycles: 4
std::string Disassembler::DEC_C(const InstructionBytes &bytes)
{
    return std::format("DEC C");
}

// INC DE
// Opcode: 0x13
// Cycles: 08
// Flags: -
std::string Disassembler::INC_DE(const InstructionBytes &bytes)
{
    return std::format("INC DE");
}

// // inc d
// // opcode: 0x14
std::string Disassembler::INC_D(const InstructionBytes &bytes)
{
    return std::format("INC D");
}

// // dec d
// // opcode: 0x15
std::string Disassembler::DEC_D(const InstructionBytes &bytes)
{
    return std::format("DEC D");
}

// // INC HL
// // Opcode: 0x23
// // Cycles: 08
// // Flags: -
std::string Disassembler::INC_HL(const InstructionBytes &bytes)
{
    return std::format("INC HL");
}

// // INC H
// // opcodE: 0x24
std::string Disassembler::INC_H(const InstructionBytes &bytes)
{
    return std::format("INC H");
}

// // DEC H
// // opcode: 0x25
std::string Disassembler::DEC_H(const InstructionBytes &bytes)
{
    return std::format("DEC H");
}

// // DEC DE
// // opcode: 0x1b
// // cycles: 8
std::string Disassembler::DEC_DE(const InstructionBytes &bytes)
{
    return std::format("DEC DE");
}

// // INC E
// // opcode: 0x1c
// // cycles 4
std::string Disassembler::INC_E(const InstructionBytes &bytes)
{
    return std::format("INC E");
}

// // DEC E
// // opcode: 0x1d
// // cycles: 4
std::string Disassembler::DEC_E(const InstructionBytes &bytes)
{
    return std::format("DEC E");
}

// /// ADD SP,dd
// /// opcode: 0xe8
// /// cycles: 16
// /// flags: 00hc
std::string Disassembler::ADD_SP_s8(const InstructionBytes &bytes)
{
    return std::format("ADD SP,{}", bytes.data[1]);
}