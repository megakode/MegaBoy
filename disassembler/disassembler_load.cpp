//
// Created by sbeam on 03/12/2021.
//

#include "disassembler.h"
#include <format>

// ***********************************************************************************
// Load Instructions
// ***********************************************************************************

// LD (nnnn),SP
// Cycles: 16
std::string Disassembler::LD_pnnnn_SP(const InstructionBytes &bytes)
{
    return std::format("LD (0x{:02X}{:02X}),SP", bytes.data[2], bytes.data[1]);
}

// LD HL,nnnn
// opcode: 0x21
// cycles: 10
// flags: -
std::string Disassembler::LD_HL_nnnn(const InstructionBytes &bytes)
{
    return std::format("LD HL,0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

// // ld de,nnnn
// // opcode: 11 n  n
// // cycles: 12
std::string Disassembler::LD_DE_nnnn(const InstructionBytes &bytes)
{
    return std::format("LD BC,0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

// ld bc,nnnn
// cycles: 10
std::string Disassembler::LD_BC_nnnn(const InstructionBytes &bytes)
{
    return std::format("LD BC,0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

// // LD SP,nnnn
// // opcode: 0x31
// // flags: -
std::string Disassembler::LD_SP_nnnn(const InstructionBytes &bytes)
{
    return std::format("LD SP,0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

// LD r,n
// cycles: 8 / 12
std::string Disassembler::LD_r_n(const InstructionBytes &bytes)
{
    uint8_t dstRegCode = (bytes.data[0] >> 3) & 0b111;
    return std::format("LD {},{:#x}", reg_name_from_regcode(dstRegCode), bytes.data[1]);
}

// LD (NN),A
// opcode: 0x32
// flags: -
std::string Disassembler::LD_pnnnn_A(const InstructionBytes &bytes)
{
    return std::format("LD (0x{:02X}{:02X}),A", bytes.data[2], bytes.data[1]);
}

/// LD (0xff00 + n),A
/// opcode: e0 nn
std::string Disassembler::LD_ff00n_A(const InstructionBytes &bytes)
{
    return std::format("LD (0xff00 + {:#x}),A", bytes.data[1]);
}

/// LD A,(0xff00 + n)
std::string Disassembler::LD_A_ff00n(const InstructionBytes &bytes)
{
    return std::format("LD A,(0xff00 + {:#x})", bytes.data[1]);
}

/// Opcode: 0xf8
/// LD HL,SP+dd
/// HL = SP +/- dd ; dd is 8-bit signed number
/// cycles: 12
std::string Disassembler::LD_HL_SPs8(const InstructionBytes &bytes)
{
    int8_t value = static_cast<int8_t>(bytes.data[1]);
    return std::format("LD HL,SP+{}", value);
}

/// LD A,(nnnn)
/// opcode: 0xfa
/// cycles: 16
std::string Disassembler::LD_A_pnnnn(const InstructionBytes &bytes)
{
    return std::format("LD A,(0x{:02X}{:02X})", bytes.data[2], bytes.data[1]);
}
