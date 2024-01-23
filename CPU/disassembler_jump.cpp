
//
// Created by sbeam on 23/01/2024.
//
#include <format>
#include <cstdint>
#include "disassembler.h"

std::string Disassembler::RET_cc(const InstructionBytes &bytes){

    uint8_t conditionCode = (bytes.data[0] & 0b00111000) >> 3;

    std::string conditionName = name_from_condition(conditionCode);
    return std::format("RET {}",conditionName);
}

std::string Disassembler::JP_cc_nn(const InstructionBytes &bytes)
{
    uint8_t conditionCode = (bytes.data[0] & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue(bytes);

    return std::format("JP {},{:04X}",name_from_condition(conditionCode),location);
}

std::string Disassembler::JR_n(const InstructionBytes &bytes)
{
    int8_t offset =  static_cast<int8_t>(bytes.data[1]);
    return std::format("JR {:+}",offset);
}

std::string Disassembler::JR_cc_nn(const InstructionBytes &bytes){
    uint8_t conditionCode = (bytes.data[0] & 0b00111000) >> 3;
    int8_t offset =  static_cast<int8_t>(bytes.data[1]);

    return std::format("JR {},{:+}",name_from_condition(conditionCode),offset);
}

std::string Disassembler::JP_nnnn(const InstructionBytes &bytes){
    uint16_t addr = fetch16BitValue(bytes);
    return std::format("JP {:04X}",addr);
}