#include "../HostMemory.h"
#include <cstdint>
#include <string>
#include <variant>
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

using DisAsmFunc = std::function<std::string(const InstructionBytes &bytes)>;

struct Instruction
{
    /// Number of CPU cycles the instruction takes to execute
    uint8_t numBytes = 0;
    uint8_t cycles = 0;
    /// pointer to the function executing the instruction
    // void (Disassembler::*code)(const InstructionBytes &) = nullptr;
    // std::function<std::string(const InstructionBytes &bytes)> code;
    std::variant<std::string, DisAsmFunc> text;
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

    static std::string invalid_opcode(const InstructionBytes &bytes);
    static std::string decode_bit_instruction(const InstructionBytes &bytes);

    // Arithmetic instructions

    static std::string CP_r(const InstructionBytes &bytes);
    static std::string CP_n(const InstructionBytes &bytes);
    static std::string ADD_A_r(const InstructionBytes &bytes);
    static std::string ADD_A_n(const InstructionBytes &bytes);
    static std::string ADC_A_r(const InstructionBytes &bytes);
    static std::string ADC_A_n(const InstructionBytes &bytes);
    static std::string SUB_r(const InstructionBytes &bytes);
    static std::string SUB_n(const InstructionBytes &bytes);
    static std::string SBC_r(const InstructionBytes &bytes);
    static std::string SBC_n(const InstructionBytes &bytes);
    static std::string AND_r(const InstructionBytes &bytes);
    static std::string AND_n(const InstructionBytes &bytes);
    static std::string XOR_r(const InstructionBytes &bytes);
    static std::string XOR_n(const InstructionBytes &bytes);
    static std::string OR_r(const InstructionBytes &bytes);
    static std::string OR_n(const InstructionBytes &bytes);

    static std::string DEC_SP(const InstructionBytes &bytes);
    static std::string INC_A(const InstructionBytes &bytes);
    static std::string DEC_A(const InstructionBytes &bytes);
    static std::string DEC_HL(const InstructionBytes &bytes);
    static std::string INC_L(const InstructionBytes &bytes);
    static std::string DEC_L(const InstructionBytes &bytes);
    static std::string INC_SP(const InstructionBytes &bytes);
    static std::string INC_pHL(const InstructionBytes &bytes);
    static std::string DEC_pHL(const InstructionBytes &bytes);
    static std::string INC_BC(const InstructionBytes &bytes);
    static std::string INC_B(const InstructionBytes &bytes);
    static std::string DEC_B(const InstructionBytes &bytes);
    static std::string DEC_BC(const InstructionBytes &bytes);
    static std::string INC_C(const InstructionBytes &bytes);
    static std::string DEC_C(const InstructionBytes &bytes);
    static std::string INC_DE(const InstructionBytes &bytes);
    static std::string INC_D(const InstructionBytes &bytes);
    static std::string DEC_D(const InstructionBytes &bytes);
    static std::string INC_HL(const InstructionBytes &bytes);
    static std::string INC_H(const InstructionBytes &bytes);
    static std::string DEC_H(const InstructionBytes &bytes);
    static std::string DEC_DE(const InstructionBytes &bytes);
    static std::string INC_E(const InstructionBytes &bytes);
    static std::string DEC_E(const InstructionBytes &bytes);
    static std::string ADD_SP_s8(const InstructionBytes &bytes);

    // Load instructions

    static std::string LD_pnnnn_SP(const InstructionBytes &bytes);
    static std::string LD_HL_nnnn(const InstructionBytes &bytes);
    static std::string LD_DE_nnnn(const InstructionBytes &bytes);
    static std::string LD_BC_nnnn(const InstructionBytes &bytes);
    static std::string LD_SP_nnnn(const InstructionBytes &bytes);
    static std::string LD_r_n(const InstructionBytes &bytes);
    static std::string LD_pnnnn_A(const InstructionBytes &bytes);
    static std::string LD_HL_SPs8(const InstructionBytes &bytes);
    static std::string LD_A_pnnnn(const InstructionBytes &bytes);
    static std::string LD_A_ff00n(const InstructionBytes &bytes);
    static std::string LD_ff00n_A(const InstructionBytes &bytes);

    // Jump instructions
    static std::string JP_cc_nnnn(const InstructionBytes &bytes);
    static std::string JP_nnnn(const InstructionBytes &bytes);
    static std::string JR_n(const InstructionBytes &bytes);
    static std::string JR_c(const InstructionBytes &bytes);
    static std::string JR_nc(const InstructionBytes &bytes);
    static std::string JR_nz(const InstructionBytes &bytes);
    static std::string JR_z(const InstructionBytes &bytes);
    static std::string CALL_cc_nnnn(const InstructionBytes &bytes);
    static std::string CALL_nnnn(const InstructionBytes &bytes);

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

    /// Debug method to get the name of a condition code, encoded in jump instruction
    static inline std::string name_from_condition(uint8_t conditionCode)
    {
        switch (conditionCode)
        {
        case 0:
            return "NZ"; // Non zero
        case 1:
            return "Z"; // zero
        case 2:
            return "NC"; // non carry
        case 3:
            return "C"; // carry
        default:
            return "Unknown condition code?!";
        }
    }

    std::vector<Instruction> instructions;
};