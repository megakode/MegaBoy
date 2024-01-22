
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

// /// ADD SP,dd
// /// opcode: 0xe8
// /// cycles: 16
// /// flags: 00hc
std::string Disassembler::ADD_SP_s8(const InstructionBytes &bytes)
{
    return std::format("ADD SP,{}", bytes.data[1]);
}