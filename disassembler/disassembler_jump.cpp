#include <format>
#include <cstdint>

#include "disassembler.h"

// Absolute jumps

std::string Disassembler::JP_nnnn(const InstructionBytes &bytes)
{
    return std::format("JP 0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

std::string Disassembler::JP_cc_nnnn(const InstructionBytes &bytes)
{
    uint8_t conditionCode = (bytes.data[0] & 0b00111000) >> 3;

    return std::format("JP {} 0x{:02X}{:02X}", name_from_condition(conditionCode), bytes.data[2], bytes.data[1]);
}

// Relative jumps

std::string Disassembler::JR_n(const InstructionBytes &bytes)
{
    int8_t offset = static_cast<int8_t>(bytes.data[1]);
    return std::format("JR {}", offset);
}

std::string Disassembler::JR_c(const InstructionBytes &bytes)
{
    int8_t offset = static_cast<int8_t>(bytes.data[1]);
    return std::format("JR C {}", offset);
}

std::string Disassembler::JR_nc(const InstructionBytes &bytes)
{
    int8_t offset = static_cast<int8_t>(bytes.data[1]);
    return std::format("JR NC {}", offset);
}

std::string Disassembler::JR_z(const InstructionBytes &bytes)
{
    int8_t offset = static_cast<int8_t>(bytes.data[1]);
    return std::format("JR Z {}", offset);
}

std::string Disassembler::JR_nz(const InstructionBytes &bytes)
{
    int8_t offset = static_cast<int8_t>(bytes.data[1]);
    return std::format("JR NZ {}", offset);
}

// Calls

std::string Disassembler::CALL_nnnn(const InstructionBytes &bytes)
{
    return std::format("CALL 0x{:02X}{:02X}", bytes.data[2], bytes.data[1]);
}

std::string Disassembler::CALL_cc_nnnn(const InstructionBytes &bytes)
{
    uint8_t conditionCode = (bytes.data[0] & 0b00111000) >> 3;
    return std::format("CALL {} 0x{:02X}{:02X}", name_from_condition(conditionCode), bytes.data[2], bytes.data[1]);
}