#include <cstdint>
#include <string>
#include "../HostMemory.h"
// #include "arithmetic_instructions_disassembler.h"
#pragma once

struct InstructionBytes
{
    uint8_t data[3];
};

struct DisassemblyLine
{
    uint16_t PC = 0;
    uint8_t cycles;
    InstructionBytes instructionBytes;
    std::string text;
};

class Disassembler
{

public:
    Disassembler();

    /// @brief Disassemble memory at location and add disassembled line to internal list
    /// @param address Location in host memory to disassemble. Depending on the instruction at the given location 1-4 bytes will be disassembled.
    /// @param memref Reference to the host GB memory.
    /// @return Number of bytes the instruction took up.
    DisassemblyLine DisassembleAddress(uint16_t address, const HostMemory &memref);

    struct Instruction
    {
        /// Number of CPU cycles the instruction takes to execute
        //  uint8_t cycles = 0;
        /// pointer to the function executing the instruction
        DisassemblyLine (Disassembler::*code)(const InstructionBytes &) = nullptr;
    };

    // Arithmetic instructions

    DisassemblyLine CP_r(const InstructionBytes &bytes);
    DisassemblyLine CP_n(const InstructionBytes &bytes);
    DisassemblyLine ADD_HL_HL(const InstructionBytes &bytes);
    DisassemblyLine ADD_HL_BC(const InstructionBytes &bytes);
    DisassemblyLine ADD_HL_DE(const InstructionBytes &bytes);
    DisassemblyLine ADD_HL_SP(const InstructionBytes &bytes);
    DisassemblyLine ADD_A_r(const InstructionBytes &bytes);
    DisassemblyLine ADD_A_n(const InstructionBytes &bytes);
    DisassemblyLine ADC_A_r(const InstructionBytes &bytes);
    DisassemblyLine ADC_A_n(const InstructionBytes &bytes);
    DisassemblyLine SUB_r(const InstructionBytes &bytes);
    DisassemblyLine SUB_n(const InstructionBytes &bytes);
    DisassemblyLine SBC_r(const InstructionBytes &bytes);
    DisassemblyLine SBC_n(const InstructionBytes &bytes);
    DisassemblyLine AND_r(const InstructionBytes &bytes);
    DisassemblyLine AND_n(const InstructionBytes &bytes);
    DisassemblyLine XOR_r(const InstructionBytes &bytes);
    DisassemblyLine XOR_n(const InstructionBytes &bytes);
    DisassemblyLine OR_r(const InstructionBytes &bytes);
    DisassemblyLine OR_n(const InstructionBytes &bytes);

    DisassemblyLine DEC_SP(const InstructionBytes &bytes);
    DisassemblyLine INC_A(const InstructionBytes &bytes);
    DisassemblyLine DEC_A(const InstructionBytes &bytes);
    DisassemblyLine DEC_HL(const InstructionBytes &bytes);
    DisassemblyLine INC_L(const InstructionBytes &bytes);
    DisassemblyLine DEC_L(const InstructionBytes &bytes);
    DisassemblyLine INC_SP(const InstructionBytes &bytes);
    DisassemblyLine INC_pHL(const InstructionBytes &bytes);
    DisassemblyLine DEC_pHL(const InstructionBytes &bytes);
    DisassemblyLine INC_BC(const InstructionBytes &bytes);
    DisassemblyLine INC_B(const InstructionBytes &bytes);
    DisassemblyLine DEC_B(const InstructionBytes &bytes);
    DisassemblyLine DEC_BC(const InstructionBytes &bytes);
    DisassemblyLine INC_C(const InstructionBytes &bytes);
    DisassemblyLine DEC_C(const InstructionBytes &bytes);
    DisassemblyLine INC_DE(const InstructionBytes &bytes);
    DisassemblyLine INC_D(const InstructionBytes &bytes);
    DisassemblyLine DEC_D(const InstructionBytes &bytes);
    DisassemblyLine INC_HL(const InstructionBytes &bytes);
    DisassemblyLine INC_H(const InstructionBytes &bytes);
    DisassemblyLine DEC_H(const InstructionBytes &bytes);
    DisassemblyLine DEC_DE(const InstructionBytes &bytes);
    DisassemblyLine INC_E(const InstructionBytes &bytes);
    DisassemblyLine DEC_E(const InstructionBytes &bytes);
    DisassemblyLine ADD_SP_s8(const InstructionBytes &bytes);

private:
    enum RegisterCode : uint8_t
    {
        B = 0,
        C,
        D,
        E,
        H,
        L,
        HLPtr,
        A
    };

    static inline std::string reg_name_from_regcode(uint8_t regcode)
    {
        switch (regcode)
        {
        case RegisterCode::A:
            return "A";
            break;
        case RegisterCode::B:
            return "B";
            break;
        case RegisterCode::C:
            return "C";
            break;
        case RegisterCode::D:
            return "D";
            break;
        case RegisterCode::E:
            return "E";
            break;
        case RegisterCode::H:
            return "H";
            break;
        case RegisterCode::L:
            return "L";
            break;
        case RegisterCode::HLPtr:
            return "(HL)";
            break;
        default:
            return "Regcode not found!";
            break;
        }
    }

    std::vector<Instruction> instructions;
};