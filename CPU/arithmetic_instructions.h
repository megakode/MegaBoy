//
// Created by sbeam on 03/12/2021.
//

#pragma once

void CPU::add( uint8_t srcValue, bool carry ){

    // Page 107 in z80 technical manual.

    uint16_t result = regs.A + srcValue;
    if ( carry && (regs.F & FlagBitmaskC) ) {
        result++;
    }

    setFlag(FlagBitmaskC,result > 0xff);
    setFlag(FlagBitmaskZero, (result&0xff) == 0);
    setFlag(FlagBitmaskHalfCarry, (regs.A & 0x0f) > (result & 0x0f));
    setFlag(FlagBitmaskN,0);

    regs.A = static_cast<uint8_t>(result);
}

//  add 16 bit register pair
void CPU::add16( uint16_t& regPair, uint16_t value_to_add , bool carry )
{
    uint32_t result = regPair + value_to_add;
    if ( regs.F & FlagBitmaskC ) {
        result++;
    }

    setFlag(FlagBitmaskC,result > 0xffff);
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskHalfCarry, (regPair & 0x0fffu) > (result & 0x0fffu));
    setFlag(FlagBitmaskN,0);

    regPair = result;
}

/**
 * Subtracts a value from register A
 * @param srcValue the value to subtract
 * @param carry whether to subtract the carry flag
 * @param onlySetFlagsForComparison Only set the flags for usage by the `cp` compare instruction and don't perform the actual subtraction
 */
void CPU::sub( uint8_t srcValue, bool carry, bool onlySetFlagsForComparison ){

    uint16_t result = (0xff00 | regs.A) - srcValue;
    if ( carry && (regs.F & FlagBitmaskC) ) {
        result--;
    }
    setFlag(FlagBitmaskZero, srcValue == regs.A);
    setFlag(FlagBitmaskN,true);
    setFlag(FlagBitmaskC, ((result&0xff00) < 0xff00) );
    setFlag(FlagBitmaskHalfCarry, (0x0f & regs.A) < (srcValue & 0x0f) );

    if(!onlySetFlagsForComparison){
        regs.A = static_cast<uint8_t>(result);
    }
}


void CPU::INC_r(uint8_t &reg)
{
    reg++;
    set_INC_operation_flags(reg);
}

void CPU::DEC_r(uint8_t &reg)
{
    reg--;
    set_DEC_operation_flags(reg);
}



// *********************************************************************************
// Compare
// *********************************************************************************

void CPU::CP_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    sub(srcRegValue,false, true);

    #ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("CP %s",regName);
    #endif
}

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 7
void CPU::CP_n()
{
    uint8_t value = fetch8BitValue();
    sub(value,false, true);

    #ifdef DEBUG_LOG
    AddDebugLog("CP %02x", value);
    #endif
}


// *********************************************************************************
// ADD
// *********************************************************************************

// add hl,hl
// opcode: 0x29
// cycles: 11
// flags: C H N
void CPU::ADD_HL_HL(){
    add16(regs.HL,regs.HL);
}


// ADD HL,BC
// opcode: 0x09
// cycles: 11
// flags: H, N, C
void CPU::add_hl_bc(){
    add16(regs.HL,regs.BC);
}

// add hl,de
// opcode: 0x19
// cycleS: 11
// flag: C N H
void CPU::ADD_HL_DE() {
    add16(regs.HL,regs.DE);
}


// ADD HL,SP
// opcode: 0x39
// cycles: 11
// flags: C N H
void CPU::add_hl_sp()
{
    add16(regs.HL,regs.SP);
}


// ADD a,r
// cycles: 4
// flags: s z h pv n c
void CPU::add_a_r(){
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);

    add(srcRegValue,false);
}

// cycles: 7
void CPU::add_a_n(){
    uint8_t srcRegValue = fetch8BitValue();
    add(srcRegValue,false);
}

void CPU::adc_a_r(){
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);

    add(srcRegValue,true);
}

// ADC A,N
// opcode: 0xce
// cycles: 7
void CPU::adc_a_n(){
    add(fetch8BitValue(),true);
}


// *********************************************************************************
// SUB
// *********************************************************************************

void CPU::sub16( uint16_t& regPair, uint16_t value_to_sub , bool carry )
{
    uint32_t result = (0xffff0000 | regs.HL) - value_to_sub;
    if ( regs.F & FlagBitmaskC ) {
        result--;
    }
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskN,true);
    setFlag(FlagBitmaskC, ((result&0xffff0000) < 0xffff0000) );
    setFlag(FlagBitmaskHalfCarry, (0xfffff000 & regs.HL) < (result & 0xfffff000) );

    regs.HL = result;
}


void CPU::sub_r(){
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    sub(srcRegValue,false, false);
}

// SUB N
// opcode: 0xd6
// cycles: 7
void CPU::sub_n(){
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue,false, false);
}

void CPU::sbc_r(){
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    sub(srcRegValue,true, false);
}

// SBC N
// opcode: 0xde
// cycles: 7
void CPU::sbc_n(){
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue,true, false);
}


// *********************************************************************************
// And
// *********************************************************************************

void CPU::and_a_with_value( uint8_t value )
{
    regs.A &= value;
    set_AND_operation_flags();
}

// cycles: 4
void CPU::and_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    and_a_with_value(srcRegValue);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("AND %s",regName);
#endif
}

void CPU::and_n()
{
    auto value = fetch8BitValue();
    and_a_with_value(value);

#ifdef DEBUG_LOG
    AddDebugLog("AND %02x",value);
#endif
}

// *********************************************************************************
// XOR
// *********************************************************************************

// cycles:
// xor r 4
// xor n 7
// xor (hl) 7
// xor (IX+d) 19
// xor (IY+d) 19
void CPU::xor_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    xor_a_with_value(srcRegValue);

    #ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("XOR %s",regName);
    #endif
}

// opcode: 0xee
// cycles: 7
void CPU::xor_n()
{
    auto value = fetch8BitValue();
    xor_a_with_value(value);

#ifdef DEBUG_LOG
    AddDebugLog("XOR %02x",value);
#endif
}

void CPU::xor_a_with_value( uint8_t value )
{
    regs.A ^= value;

    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
}

// *********************************************************************************
// Or
// *********************************************************************************

// cycles:
// or r 4
// or n 7
// or (hl) 7
// or (IX+d) 19
// or (IY+d) 19
void CPU::or_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    or_a_with_value(srcRegValue);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(srcRegCode);
    AddDebugLog("OR %s",regName.c_str());
#endif
}

// OR N
// opcode: 0xf6
// cycles: 7
void CPU::or_n()
{
    or_a_with_value(fetch8BitValue());
}

void CPU::or_a_with_value(uint8_t value)
{
    regs.A |= value;

    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
}


// *********************************************************************************
// Inc / Dec
// *********************************************************************************


// DEC SP
// opcode: 0x3b
void CPU::DEC_SP()
{
    regs.SP--;
}

// opcode: 0x3c
// flags: -
void CPU::INC_A()
{
    INC_r(regs.A);
}

// opcode: 0x3d
void CPU::DEC_A()
{
    DEC_r(regs.A);
}


// dec hl
// opcode: 0x2b
// cycles: 6
// flags: -
void CPU::DEC_HL(){
    regs.HL--;
}

// inc l
// opcode: 0x2c
// cycles: 4
void CPU::INC_L(){
    INC_r(regs.L);
}

// DEC L
// opcode: 0x2d
// cycles: 4
void CPU::DEC_L(){
    DEC_r(regs.L);
}


// INC SP
// opcode: 0x33
// flags: -
void CPU::INC_SP()
{
    regs.SP++;

}

// INC (HL)
// opcode: 0x34
// N PV H S Z
void CPU::INC_pHL()
{
    uint8_t result = mem.Read(regs.HL);
    mem.Write(regs.HL,++result);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00010000 );
    setFlag(FlagBitmaskN,0);
}

// DEC (HL)
// opcode: 0x35
void CPU::DEC_pHL()
{
    uint8_t result = mem.Read(regs.HL);
    mem.Write(regs.HL,--result);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00001111 );
    setFlag(FlagBitmaskN,1);
}

// INC BC
// Opcode: 03
// Cycles: 06
// Flags: -
void CPU::INC_BC(){
    regs.BC++;
#ifdef DEBUG_LOG
    AddDebugLog("INC BC");
#endif
}

// inc b
// opcode: 04
// cycles: 4
// flags: S Z HC PV N
void CPU::INC_B(){
    INC_r(regs.B);
#ifdef DEBUG_LOG
    AddDebugLog("INC B");
#endif
}

// dec b
// opcode: 0x05
// cycles: 4
void CPU::DEC_B(){
    DEC_r(regs.B);
#ifdef DEBUG_LOG
    AddDebugLog("DEC B");
#endif
}


// DEC BC
// opcode: 0x0b
// cycles: 6
void CPU::dec_bc(){
    regs.BC--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC BC");
#endif
}

// INC C
// opcode: 0x0c
// cycles: 4
void CPU::inc_c(){
    INC_r(regs.C);
#ifdef DEBUG_LOG
    AddDebugLog("INC C");
#endif
}

// DEC C
// opcode: 0x0d
// cycles: 4
void CPU::dec_c(){
    DEC_r(regs.C);
#ifdef DEBUG_LOG
    AddDebugLog("DEC C");
#endif
}

// inc de
// Opcode: 0x13
// Cycles: 06
// Flags: -
void CPU::INC_DE(){
    regs.DE++;
#ifdef DEBUG_LOG
    AddDebugLog("INC DE");
#endif
}

// inc d
// opcode: 0x14
void CPU::inc_d(){
    INC_r(regs.D);
#ifdef DEBUG_LOG
    AddDebugLog("INC D");
#endif
}

// dec d
// opcode: 0x15
void CPU::dec_d(){
    DEC_r(regs.D);
#ifdef DEBUG_LOG
    AddDebugLog("DEC D");
#endif
}

// INC HL
// Opcode: 0x23
// Cycles: 06
// Flags: -
void CPU::INC_HL(){
    regs.HL++;
#ifdef DEBUG_LOG
    AddDebugLog("INC HL");
#endif
}

// INC H
// opcodE: 0x24
void  CPU::INC_H(){
    INC_r(regs.H);
#ifdef DEBUG_LOG
    AddDebugLog("INC H");
#endif
}

// DEC H
// opcode: 0x25
void  CPU::DEC_H(){
    DEC_r(regs.H);
#ifdef DEBUG_LOG
    AddDebugLog("DEC H");
#endif
}

// DEC DE
// opcode: 0x1b
// cycles: 6
void CPU::DEC_DE(){
    regs.DE--;
#ifdef DEBUG_LOG
    AddDebugLog("DEC DE");
#endif
}

// INC E
// opcode: 0x1c
// cycles 4
void CPU::INC_E(){
    INC_r(regs.E);
#ifdef DEBUG_LOG
    AddDebugLog("INC E");
#endif
}

// DEC E
// opcode: 0x1d
// cycles: 4
void CPU::DEC_E(){
    DEC_r(regs.E);
#ifdef DEBUG_LOG
    AddDebugLog("DEC E");
#endif
}

/// ADD SP,dd
/// opcode: 0xe8
/// cycles: 16
/// flags: 00hc
void CPU::ADD_SP_s8()
{
    auto value = static_cast<int8_t>(fetch8BitValue());
    uint32_t result = regs.SP + value;
    regs.F = 0;
    setFlag(FlagBitmaskC, result & 0xff0000);
    setFlag(FlagBitmaskHalfCarry, (regs.SP & 0x0fffu) > (result & 0x0fffu));
}