
//
// Created by sbeam on 03/12/2021.
//

#pragma once
#include <format>

// *********************************************************************************
// Compare
// *********************************************************************************

DisassemblyLine Disassembler::CP_r(const InstructionBytes& bytes)
{
    uint8_t srcRegCode = bytes.data[0] & 0b111;
    auto regName = reg_name_from_regcode(srcRegCode);
    return { 
        .cycles = (srcRegCode == RegisterCode::HLPtr ? 8 : 4),  
        .text = std::format("CP {}",regName) 
        };
}

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 8
DisassemblyLine Disassembler::CP_n(const InstructionBytes& bytes)
{
    uint8_t value = bytes.data[1];
    return { 
        .cycles = 8, 
        .text = std::format("CP {:#x}", value) 
        };
}


// *********************************************************************************
// ADD
// *********************************************************************************

// add hl,hl
// opcode: 0x29
// cycles: 11
// flags: C H N
// DisassemblyLine Disassembler::ADD_HL_HL( const InstructionBytes& bytes )
// {
//     return { };
// }


// ADD HL,BC
// opcode: 0x09
// cycles: 11
// flags: H, N, C
// void Disassembler::ADD_HL_BC()
// {
//     add16(regs.HL,regs.BC);
// }

// add hl,de
// opcode: 0x19
// cycleS: 11
// flag: C N H
// void Disassembler::ADD_HL_DE() 
// {
//     add16(regs.HL,regs.DE);
// }


// ADD HL,SP
// opcode: 0x39
// cycles: 11
// flags: - 0 H C
// void Disassembler::ADD_HL_SP()
// {
//     add16(regs.HL,regs.SP);
// }


// ADD a,r
// cycles: 4
// flags: s z h pv n c
// void Disassembler::ADD_A_r(){
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);

// #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("ADD A,%s",regName.c_str());
// #endif
// }

// // cycles: 7
// void Disassembler::ADD_A_n(){
//     uint8_t srcRegValue = fetch8BitValue();
//     add(srcRegValue,false);
// #ifdef DEBUG_LOG
//     AddDebugLog("ADD A,%x",srcRegValue);
// #endif
// }

// void Disassembler::ADC_A_r(){
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);

//     add(srcRegValue,true);
// #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("ADC A,%s",regName.c_str());
// #endif
// }

// // ADC A,N
// // opcode: 0xce
// // cycles: 7
// void Disassembler::ADC_A_n(){
//     auto value = fetch8BitValue();
//     add(value,true);
// #ifdef DEBUG_LOG
//     AddDebugLog("ADC A,%x",value);
// #endif
// }


// // *********************************************************************************
// // SUB
// // *********************************************************************************

// void Disassembler::SUB_r(){
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);
//     sub(srcRegValue,false, false);
// }

// // SUB N
// // opcode: 0xd6
// // cycles: 7
// void Disassembler::SUB_n(){
//     uint8_t srcRegValue = fetch8BitValue();
//     sub(srcRegValue,false, false);
// #ifdef DEBUG_LOG
//     AddDebugLog("SUB A,%x",srcRegValue);
// #endif
// }

// void Disassembler::SBC_r(){
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);
//     sub(srcRegValue,true, false);
// #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("SBC A,%s",regName.c_str());
// #endif
// }

// // SBC N
// // opcode: 0xde
// // cycles: 7
// void Disassembler::SBC_n(){
//     uint8_t srcRegValue = fetch8BitValue();
//     sub(srcRegValue,true, false);
// #ifdef DEBUG_LOG
//     AddDebugLog("SBC A,%x",srcRegValue);
// #endif
// }


// // *********************************************************************************
// // And
// // *********************************************************************************

// void Disassembler::and_a_with_value( uint8_t value )
// {
//     regs.A &= value;
//     set_AND_operation_flags();
// }

// // cycles: 4
// void Disassembler::AND_r()
// {
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);
//     and_a_with_value(srcRegValue);

// #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("AND %s",regName.c_str());
// #endif
// }

// void Disassembler::AND_n()
// {
//     auto value = fetch8BitValue();
//     and_a_with_value(value);

// #ifdef DEBUG_LOG
//     AddDebugLog("AND %02x",value);
// #endif
// }

// // *********************************************************************************
// // XOR
// // *********************************************************************************

// // cycles:
// // xor r 4
// // xor n 7
// // xor (hl) 7
// // xor (IX+d) 19
// // xor (IY+d) 19
// void Disassembler::XOR_r()
// {
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);
//     xor_a_with_value(srcRegValue);

//     #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("XOR %s",regName.c_str());
//     #endif
// }

// // opcode: 0xee
// // cycles: 7
// void Disassembler::XOR_n()
// {
//     auto value = fetch8BitValue();
//     xor_a_with_value(value);

// #ifdef DEBUG_LOG
//     AddDebugLog("XOR %02x",value);
// #endif
// }

// void Disassembler::xor_a_with_value( uint8_t value )
// {
//     regs.A ^= value;

//     setFlag(FlagBitmaskZero, regs.A == 0);
//     setFlag(FlagBitmaskHalfCarry,false);
//     setFlag(FlagBitmaskN,false);
//     setFlag(FlagBitmaskC,false);
// }

// // *********************************************************************************
// // Or
// // *********************************************************************************

// // cycles:
// // or r 4
// // or n 7
// // or (hl) 7
// void Disassembler::OR_r()
// {
//     uint8_t srcRegCode = current_opcode & 0b111;
//     uint8_t srcRegValue = read_from_register(srcRegCode);
//     or_a_with_value(srcRegValue);

// #ifdef DEBUG_LOG
//     auto regName = reg_name_from_regcode(srcRegCode);
//     AddDebugLog("OR %s",regName.c_str());
// #endif
// }

// // OR N
// // opcode: 0xf6
// // cycles: 7
// void Disassembler::OR_n()
// {
//     or_a_with_value(fetch8BitValue());
// }

// void Disassembler::or_a_with_value(uint8_t value)
// {
//     regs.A |= value;

//     setFlag(FlagBitmaskZero, regs.A == 0);
//     setFlag(FlagBitmaskHalfCarry,0);
//     setFlag(FlagBitmaskN,0);
//     setFlag(FlagBitmaskC,0);
// }


// // *********************************************************************************
// // Inc / Dec
// // *********************************************************************************


// // DEC SP
// // opcode: 0x3b
// void Disassembler::DEC_SP()
// {
//     regs.SP--;
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC SP");
// #endif
// }

// // opcode: 0x3c
// // flags: -
// void Disassembler::INC_A()
// {
//     INC_r(regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC A");
// #endif
// }

// // opcode: 0x3d
// void Disassembler::DEC_A()
// {
//     DEC_r(regs.A);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC A");
// #endif
// }


// // dec hl
// // opcode: 0x2b
// // cycles: 6
// // flags: -
// void Disassembler::DEC_HL(){
//     regs.HL--;
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC HL");
// #endif
// }

// // inc l
// // opcode: 0x2c
// // cycles: 4
// void Disassembler::INC_L(){
//     INC_r(regs.L);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC L");
// #endif
// }

// // DEC L
// // opcode: 0x2d
// // cycles: 4
// void Disassembler::DEC_L(){
//     DEC_r(regs.L);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC L");
// #endif
// }


// // INC SP
// // opcode: 0x33
// // flags: -
// void Disassembler::INC_SP()
// {
//     regs.SP++;
// #ifdef DEBUG_LOG
//     AddDebugLog("INC SP");
// #endif
// }

// // INC (HL)
// // opcode: 0x34
// // Z 0 H -
// void Disassembler::INC_pHL()
// {
//     uint8_t result = mem.Read(regs.HL);
//     mem.Write(regs.HL,++result);
//     set_INC_operation_flags(result);
// }

// // DEC (HL)
// // opcode: 0x35
// void Disassembler::DEC_pHL()
// {
//     uint8_t result = mem.Read(regs.HL);
//     mem.Write(regs.HL,--result);
//     set_DEC_operation_flags(result);
// }

// // INC BC
// // Opcode: 03
// // Cycles: 06
// // Flags: -
// void Disassembler::INC_BC(){
//     regs.BC++;
// #ifdef DEBUG_LOG
//     AddDebugLog("INC BC");
// #endif
// }

// // inc b
// // opcode: 04
// // cycles: 4
// // flags: S Z HC PV N
// void Disassembler::INC_B(){
//     INC_r(regs.B);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC B");
// #endif
// }

// // dec b
// // opcode: 0x05
// // cycles: 4
// void Disassembler::DEC_B(){
//     DEC_r(regs.B);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC B");
// #endif
// }


// // DEC BC
// // opcode: 0x0b
// // cycles: 6
// void Disassembler::DEC_BC(){
//     regs.BC--;
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC BC");
// #endif
// }

// // INC C
// // opcode: 0x0c
// // cycles: 4
// void Disassembler::INC_C(){
//     INC_r(regs.C);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC C");
// #endif
// }

// // DEC C
// // opcode: 0x0d
// // cycles: 4
// void Disassembler::DEC_C(){
//     DEC_r(regs.C);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC C");
// #endif
// }

// // inc de
// // Opcode: 0x13
// // Cycles: 06
// // Flags: -
// void Disassembler::INC_DE(){
//     regs.DE++;
// #ifdef DEBUG_LOG
//     AddDebugLog("INC DE");
// #endif
// }

// // inc d
// // opcode: 0x14
// void Disassembler::INC_D(){
//     INC_r(regs.D);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC D");
// #endif
// }

// // dec d
// // opcode: 0x15
// void Disassembler::DEC_D(){
//     DEC_r(regs.D);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC D");
// #endif
// }

// // INC HL
// // Opcode: 0x23
// // Cycles: 06
// // Flags: -
// void Disassembler::INC_HL(){
//     regs.HL++;
// #ifdef DEBUG_LOG
//     AddDebugLog("INC HL");
// #endif
// }

// // INC H
// // opcodE: 0x24
// void  Disassembler::INC_H(){
//     INC_r(regs.H);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC H");
// #endif
// }

// // DEC H
// // opcode: 0x25
// void  Disassembler::DEC_H(){
//     DEC_r(regs.H);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC H");
// #endif
// }

// // DEC DE
// // opcode: 0x1b
// // cycles: 6
// void Disassembler::DEC_DE(){
//     regs.DE--;
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC DE");
// #endif
// }

// // INC E
// // opcode: 0x1c
// // cycles 4
// void Disassembler::INC_E(){
//     INC_r(regs.E);
// #ifdef DEBUG_LOG
//     AddDebugLog("INC E");
// #endif
// }

// // DEC E
// // opcode: 0x1d
// // cycles: 4
// void Disassembler::DEC_E(){
//     DEC_r(regs.E);
// #ifdef DEBUG_LOG
//     AddDebugLog("DEC E");
// #endif
// }

// /// ADD SP,dd
// /// opcode: 0xe8
// /// cycles: 16
// /// flags: 00hc
// void Disassembler::ADD_SP_s8()
// {
//     auto value = static_cast<int8_t>(fetch8BitValue());
//     uint16_t result = regs.SP + value;
//     regs.F = 0;
//     setFlag(FlagBitmaskC,(regs.SP & 0x00FF) + (value & 0xff) > 0x00FF);
//     setFlag(FlagBitmaskHalfCarry, (regs.SP & 0x000f) + (value & 0x000f) > 0x000f);
//     regs.SP = result & 0xffff;

// }