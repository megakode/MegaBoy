#include "../HostMemory.h"
#include <cstdint>
#include <string>
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
    uint8_t numberOfBytes;
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
        uint8_t numBytes = 0;
        uint8_t cycles = 0;
        /// pointer to the function executing the instruction
        // void (Disassembler::*code)(const InstructionBytes &) = nullptr;
        std::function<std::string(const InstructionBytes &bytes)> code;
    };

    // Arithmetic instructions

    std::string CP_r(const InstructionBytes &bytes);
    std::string CP_n(const InstructionBytes &bytes);
    std::string ADD_HL_HL(const InstructionBytes &bytes);
    std::string ADD_HL_BC(const InstructionBytes &bytes);
    std::string ADD_HL_DE(const InstructionBytes &bytes);
    std::string ADD_HL_SP(const InstructionBytes &bytes);
    std::string ADD_A_r(const InstructionBytes &bytes);
    std::string ADD_A_n(const InstructionBytes &bytes);
    std::string ADC_A_r(const InstructionBytes &bytes);
    std::string ADC_A_n(const InstructionBytes &bytes);
    std::string SUB_r(const InstructionBytes &bytes);
    std::string SUB_n(const InstructionBytes &bytes);
    std::string SBC_r(const InstructionBytes &bytes);
    std::string SBC_n(const InstructionBytes &bytes);
    std::string AND_r(const InstructionBytes &bytes);
    std::string AND_n(const InstructionBytes &bytes);
    std::string XOR_r(const InstructionBytes &bytes);
    std::string XOR_n(const InstructionBytes &bytes);
    std::string OR_r(const InstructionBytes &bytes);
    std::string OR_n(const InstructionBytes &bytes);

    std::string DEC_SP(const InstructionBytes &bytes);
    std::string INC_A(const InstructionBytes &bytes);
    std::string DEC_A(const InstructionBytes &bytes);
    std::string DEC_HL(const InstructionBytes &bytes);
    std::string INC_L(const InstructionBytes &bytes);
    std::string DEC_L(const InstructionBytes &bytes);
    std::string INC_SP(const InstructionBytes &bytes);
    std::string INC_pHL(const InstructionBytes &bytes);
    std::string DEC_pHL(const InstructionBytes &bytes);
    std::string INC_BC(const InstructionBytes &bytes);
    std::string INC_B(const InstructionBytes &bytes);
    std::string DEC_B(const InstructionBytes &bytes);
    std::string DEC_BC(const InstructionBytes &bytes);
    std::string INC_C(const InstructionBytes &bytes);
    std::string DEC_C(const InstructionBytes &bytes);
    std::string INC_DE(const InstructionBytes &bytes);
    std::string INC_D(const InstructionBytes &bytes);
    std::string DEC_D(const InstructionBytes &bytes);
    std::string INC_HL(const InstructionBytes &bytes);
    std::string INC_H(const InstructionBytes &bytes);
    std::string DEC_H(const InstructionBytes &bytes);
    std::string DEC_DE(const InstructionBytes &bytes);
    std::string INC_E(const InstructionBytes &bytes);
    std::string DEC_E(const InstructionBytes &bytes);
    std::string ADD_SP_s8(const InstructionBytes &bytes);

    // Load instructions

    std::string LD_pnnnn_SP(const InstructionBytes &bytes);
    std::string LD_HL_nnnn(const InstructionBytes &bytes);
    std::string LD_DE_nnnn(const InstructionBytes &bytes);
    std::string LD_BC_nnnn(const InstructionBytes &bytes);
    std::string LD_SP_nnnn(const InstructionBytes &bytes);
    std::string LD_r_n(const InstructionBytes &bytes);
    // std::string LD_r_r(const InstructionBytes &bytes);

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

    /// Fetch two instruction bytes from memory as a 16 bit address
    static inline uint16_t fetch16BitValue(const InstructionBytes &bytes)
    {
        uint8_t lowbyte = bytes.data[1];
        uint16_t hibyte = bytes.data[2];
        return (hibyte << 8) + lowbyte;
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