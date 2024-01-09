//
// Created by sbeam on 03/12/2021.
//

#include "disassembler.h"
#include <format>

// ***********************************************************************************
// Load Instructions
// ***********************************************************************************

// LD (nn),SP
// Cycles: 16
std::string Disassembler::LD_pnn_SP(const InstructionBytes &bytes)
{
    return std::format("LD ({}),SP", bytes.data[1]);
}

// // LD r,r
// // Cycles 4 / 8
// std::string Disassembler::LD_r_r(const InstructionBytes &bytes)
// {
//     uint8_t dstRegCode = (bytes.data[0] >> 3) & 0b111;
//     uint8_t srcRegCode = bytes.data[0] & 0b111;

//     uint8_t cycles = 4;

//     if (dstRegCode == RegisterCode::HLPtr || srcRegCode == RegisterCode::HLPtr)
//     {
//         cycles = 8;
//     }

//     return {.cycles = cycles, .text = std::format("LD {},{}", reg_name_from_regcode(dstRegCode), reg_name_from_regcode(srcRegCode))};

//     // if (dstRegCode == 0 && srcRegCode == 0)
//     // {
//     //     // LD B,B - mooneye test has completed
//     //     if (regs.B == 3)
//     //     {
//     //         std::cout << "Test OK!" << std::flush;
//     //     }
//     //     else
//     //     {
//     //         std::cout << "Test FAILED" << std::flush;
//     //     }
//     // }
// }

// LD r,n
// cycles: 8 / 12
// std::string Disassembler::LD_r_n(const InstructionBytes &bytes)
// {
//     uint8_t dstRegCode = (bytes.data[0] >> 3) & 0b111;
//     uint8_t value = bytes.data[1];
//     uint8_t cycles = 8;

//     if (dstRegCode == RegisterCode::HLPtr)
//     {
//         cycles = 12;
//     }

//     return {.cycles = cycles, .text = std::format("LD {},{:#x}", reg_name_from_regcode(dstRegCode), value)};
// }

// LD HL,NN
// opcode: 0x21
// cycles: 10
// flags: -
// std::string Disassembler::LD_HL_nn(const InstructionBytes &bytes)
// {
//     regs.L = fetch8BitValue();
//     regs.H = fetch8BitValue();

// #ifdef DEBUG_LOG
//     AddDebugLog("LD HL,0x%04x", regs.HL);
// #endif
// }

// // LD SP,NN
// // opcode: 0x31
// // flags: -
// std::string Disassembler::LD_SP_nn()
// {
//     regs.SP = fetch16BitValue();
// #ifdef DEBUG_LOG
//     AddDebugLog("LD SP,0x%04x", regs.SP);
// #endif
// }

// // LD (NN),A
// // opcode: 0x32
// // flags: -
// std::string Disassembler::LD_pnn_A()
// {
//     auto addr = fetch16BitValue();
//     mem.Write(addr, regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD (0x%04x),A", addr);
// #endif
// }

// // LD (HL),N
// // opcode: 0x36
// std::string Disassembler::LD_pHL_n()
// {
//     uint8_t value = fetch8BitValue();
//     mem.Write(regs.HL, value);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD (HL),0x%02x", value);
// #endif
// }

// // LD SP,HL
// // opcode: 0xf9
// // flags: -
// // cycles: 6
// std::string Disassembler::ld_sp_hl()
// {
//     regs.SP = regs.HL;
// #ifdef DEBUG_LOG
//     AddDebugLog("LD SP,HL");
// #endif
// }

// // ld bc,nn
// // opcode: 01 n  n
// // cycles: 10
std::string Disassembler::LD_BC_nn(const InstructionBytes &bytes)
{
    return std::format("LD BC,0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

// // ld (bc),a
// // Opcode: 02
// // Cycles: 7
// std::string Disassembler::LD_pBC_A()
// {
//     mem.Write(regs.BC, regs.A);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD (BC),A");
// #endif
// }

// // LD A,(BC)
// // opcode: 0x0A
// // cycles: 7
// // flags: -
// std::string Disassembler::LD_A_pBC()
// {
//     regs.A = mem.Read(regs.BC);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD A,(BC)");
// #endif
// }

// // ld de,nn
// // opcode: 11 n  n
// // cycles: 10
// std::string Disassembler::LD_DE_nn()
// {
//     regs.DE = fetch16BitValue();

// #ifdef DEBUG_LOG
//     AddDebugLog("LD DE,0x%04x", regs.DE);
// #endif
// }

// // load (de),a
// // opcode: 0x12
// // cycles: 7
// std::string Disassembler::LD_pDE_A()
// {
//     mem.Write(regs.DE, regs.A);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD (DE),A");
// #endif
// }

// // LD A,(DE)
// // opcode: 0x1a
// // cycles: 7
// // flags: -
// std::string Disassembler::LD_A_pDE()
// {
//     regs.A = mem.Read(regs.DE);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD A,(DE)");
// #endif
// }

// /// LDI  (HL),A
// /// opcode: 22
// /// cycles: 8
// std::string Disassembler::LDI_pHL_A()
// {
//     mem.Write(regs.HL++, regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("LDI (HL),A");
// #endif
// }

// /// LDI  A,(HL)
// /// opcode: 2a
// /// cycles: 8
// std::string Disassembler::LDI_A_pHL()
// {
//     regs.A = mem.Read(regs.HL++);
// #ifdef DEBUG_LOG
//     AddDebugLog("LDI A,(HL)");
// #endif
// }

// /// LDD (HL),A
// /// opcode: 0x32
// /// cycles: 8
// std::string Disassembler::LDD_pHL_A()
// {
//     mem.Write(regs.HL--, regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("LDD (HL),A");
// #endif
// }

// /// LDD  A,(HL)
// /// opcode: 0x3a
// /// cycles: 8
// std::string Disassembler::LDD_A_pHL()
// {
//     regs.A = mem.Read(regs.HL--);
// #ifdef DEBUG_LOG
//     AddDebugLog("LDD A,(HL)");
// #endif
// }

// /// LD (0xff00 + n),A
// /// opcode: e0 nn
// /// cycles: 8
// /// flags: -
// std::string Disassembler::LD_ff00n_A()
// {
//     uint8_t lowbyte = fetch8BitValue();
//     uint16_t addr = 0xff00 + lowbyte;
//     mem.Write(addr, regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD (0xff00 + 0x%02x),A", lowbyte);
// #endif
// }

// /// LD (FF00+C),A
// /// opcode: 0xe2
// std::string Disassembler::LD_ff00C_A()
// {
//     uint16_t addr = 0xff00 + regs.C;
//     mem.Write(addr, regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD (0xff00 + C),A");
// #endif
// }

// /// LD A,(0xff00 + n)
// /// opcode: f0 nn
// /// cycles: 8
// /// flags: -
// std::string Disassembler::LD_A_ff00n()
// {
//     uint8_t lowbyte = fetch8BitValue();
//     uint16_t addr = 0xff00 + lowbyte;
//     regs.A = mem.Read(addr);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD A,(0xff00 + 0x%02x)", lowbyte);
// #endif
// }

// /// LD A,(FF00+C)
// /// opcode: 0xe2
// std::string Disassembler::LD_A_ff00C()
// {
//     uint16_t addr = 0xff00 + regs.C;
//     regs.A = mem.Read(addr);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD A,(0xff00 + C)");
// #endif
// }

// // Opcode: 0xf8
// /// LD HL,SP+dd
// /// HL = SP +/- dd ; dd is 8-bit signed number
// /// cycles: 12
// /// test rom: ok
// std::string Disassembler::LD_HL_SPs8()
// {
//     auto value = static_cast<int8_t>(fetch8BitValue());

//     regs.HL = regs.SP + value;
//     regs.F = 0;
//     setFlag(FlagBitmaskC, (regs.SP & 0xFF) + (value & 0xff) > 0xFF);
//     setFlag(FlagBitmaskHalfCarry, (regs.SP & 0xf) + (value & 0xf) > 0xf);

// #ifdef DEBUG_LOG
//     AddDebugLog("LD HL,SP+%+i", value);
// #endif
// }

// /// LD A,(nn)
// /// opcode: 0xfa
// /// cycles: 16
// std::string Disassembler::LD_A_pnn()
// {
//     auto addr = fetch16BitValue();
//     regs.A = mem.Read(addr);
// #ifdef DEBUG_LOG
//     AddDebugLog("LD A,(0x%04x)", addr);
// #endif
// }

// // ********************************************************************************
// // PUSH / POP
// // ********************************************************************************

// /// Pop 2 bytes of the stack into a 16 bit register
// /// \param regPair The 16 bit register pair to pop the stack value into
// std::string Disassembler::pop16(uint16_t &regPair)
// {
//     uint8_t lobyte = mem.Read(regs.SP++);
//     uint8_t hibyte = mem.Read(regs.SP++);
//     regPair = (hibyte << 8) + lobyte;
// }

// // cycles: 10
// std::string Disassembler::pop_qq()
// {
//     uint8_t regPairCode = (current_opcode & 0b00110000) >> 4;
//     switch (regPairCode)
//     {
//     case 0:
//         pop16(regs.BC);
// #ifdef DEBUG_LOG
//         AddDebugLog("POP BC");
// #endif
//         break;
//     case 1:
//         pop16(regs.DE);
// #ifdef DEBUG_LOG
//         AddDebugLog("POP DE");
// #endif
//         break;
//     case 2:
//         pop16(regs.HL);
// #ifdef DEBUG_LOG
//         AddDebugLog("POP HL");
// #endif
//         break;
//     case 3:
//         pop16(regs.AF);
//         regs.F &= 0xf0; // Always mask out lower nibble of flags, as they are forced zero on hardware
// #ifdef DEBUG_LOG
//         AddDebugLog("POP AF");
// #endif
//         break;
//     }
// }

// /// PUSH BC
// /// cycles: 11
// std::string Disassembler::push_bc()
// {
//     mem.Write(--regs.SP, regs.B);
//     mem.Write(--regs.SP, regs.C);
// #ifdef DEBUG_LOG
//     AddDebugLog("PUSH BC");
// #endif
// }

// /// PUSH DE
// /// cycles: 11
// std::string Disassembler::push_de()
// {
//     mem.Write(--regs.SP, regs.D);
//     mem.Write(--regs.SP, regs.E);
// #ifdef DEBUG_LOG
//     AddDebugLog("PUSH DE");
// #endif
// }

// /// PUSH HL
// /// cycles: 11
// std::string Disassembler::push_hl()
// {
//     mem.Write(--regs.SP, regs.H);
//     mem.Write(--regs.SP, regs.L);
// #ifdef DEBUG_LOG
//     AddDebugLog("PUSH HL");
// #endif
// }

// /// PUSH AF
// /// cycles: 11
// std::string Disassembler::push_af()
// {
//     mem.Write(--regs.SP, regs.A);
//     mem.Write(--regs.SP, regs.F & 0xf0); // mask out the lower 4 bits to force them to always be zero
// #ifdef DEBUG_LOG
//     AddDebugLog("PUSH AF");
// #endif
// }

// std::string Disassembler::push_pc()
// {
//     mem.Write(--regs.SP, regs.PC >> 8);   // hi-byte
//     mem.Write(--regs.SP, regs.PC & 0xff); // lo-byte
// }
