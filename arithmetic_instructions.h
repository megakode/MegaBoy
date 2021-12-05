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
    setFlag(FlagBitmaskSign, result & 0x80);

    // TODO: test this!
    // if like signs in numbers being added, and result sign is different, set overflow:
    /*
           ADDITION SIGN BITS
    num1sign num2sign ADDsign
   ---------------------------
        0       0       0
 *OVER* 0       0       1 (adding two positives should be positive)
        0       1       0
        0       1       1
        1       0       0
        1       0       1
 *OVER* 1       1       0 (adding two negatives should be negative)
        1       1       1
        */
    setFlag(FlagBitmaskPV, !((regs.A ^ srcValue) & 0x80) && ((result ^ regs.A) & 0x80) );

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
    setFlag(FlagBitmaskSign, result & 0x8000);
    setFlag(FlagBitmaskPV, !((regPair ^ value_to_add) & 0x8000) && ((result ^ regPair) & 0x8000) );

    regPair = result;
}

/**
 * Subtracts a value from register A
 * @param srcValue the value to subtract
 * @param carry whether to subtract the carry flag
 * @param onlySetFlagsForComparison Only set the flags for usage by the `cp` compare instruction and don't perform the actual subtraction
 */
void CPU::sub( uint8_t srcValue, bool carry, bool onlySetFlagsForComparison ){
    // TODO: finish this
    uint16_t result = (0xff00 | regs.A) - srcValue;
    if ( carry && (regs.F & FlagBitmaskC) ) {
        result--;
    }
    setFlag(FlagBitmaskZero, srcValue == regs.A);
    setFlag(FlagBitmaskN,true);
    setFlag(FlagBitmaskC, ((result&0xff00) < 0xff00) );
    setFlag(FlagBitmaskHalfCarry, (0xfff0 & regs.A) < (result & 0xfff0) );
    setFlag(FlagBitmaskSign, result & 0x80 );
    /*

     http://teaching.idallen.com/dat2343/11w/notes/040_overflow.txt

         SUBTRACTION SIGN BITS
       num1sign num2sign SUBsign
      ---------------------------
           0       0       0
           0       0       1
           0       1       0
    *OVER* 0       1       1 (subtract negative is same as adding a positive)
    *OVER* 1       0       0 (subtract positive is same as adding a negative)
           1       0       1
           1       1       0
           1       1       1
     */
    bool isOperandsSignBitDifferent = (regs.A ^ srcValue) & 0x80;
    bool didChangeSignInResult = (regs.A ^ result) & 0x80;
    setFlag( FlagBitmaskPV, isOperandsSignBitDifferent && didChangeSignInResult );

    if(!onlySetFlagsForComparison){
        regs.A = static_cast<uint8_t>(result);
    }
}


void CPU::inc_r(uint8_t &reg)
{
    reg++;
    set_INC_operation_flags(reg);
}

void CPU::dec_r(uint8_t &reg)
{
    reg--;
    set_DEC_operation_flags(reg);
}


// INC IX
// cycles: 10
void CPU::INC_IX()
{
    specialRegs.IX++;
}

// DEC IX
// opcode: 0xdd 0x2b
// cycles: 10
void CPU::DEC_IX()
{
    specialRegs.IX--;
}

// INC IY
// cycles: 10
void CPU::INC_IY()
{
    specialRegs.IY++;
}

// DEC IY
// opcode: 0xfd 0x2b
// cycles: 10
void CPU::DEC_IY()
{
    specialRegs.IY--;
}

// INC (ix+n)
// cycles: 23
void CPU::INC_pIXn()
{
    uint8_t& location = fetch_pIXn();
    location++;
    set_INC_operation_flags(location);

#ifdef DEBUG_LOG
    std::cout << "INC (IX+n) [IX+n=" << location << "]" << std::endl;
#endif
}

// DEC (ix+n)
// cycles: 23
void CPU::DEC_pIXn()
{
    uint8_t& location = fetch_pIXn();
    location--;
    set_DEC_operation_flags(location);

#ifdef DEBUG_LOG
    std::cout << "DEC (IX+n) [IX+n=" << location << "]" << std::endl;
#endif
}

// INC (iy+n)
// cycles: 23
void CPU::INC_pIYn()
{
    uint8_t& location = fetch_pIYn();
    location++;
    set_INC_operation_flags(location);

#ifdef DEBUG_LOG
    std::cout << "INC (IY+n) [IY+n=" << location << "]" << std::endl;
#endif
}

// DEC (iy+n)
// cycles: 23
void CPU::DEC_pIYn()
{
    uint8_t& location = fetch_pIYn();
    location--;
    set_DEC_operation_flags(location);

#ifdef DEBUG_LOG
    std::cout << "DEC (IY+n) [IY+n=" << location << "]" << std::endl;
#endif
}

// *********************************************************************************
// Compare
// *********************************************************************************

void CPU::cp_a_with_value( uint8_t value )
{
    sub(value,false, true);
}

void CPU::cp_r()
{
    uint8_t srcRegCode = current_opcode & 0b111;
    uint8_t srcRegValue = reg_from_regcode(srcRegCode);
    sub(srcRegValue,false, true);
}

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 7
void CPU::cp_n()
{
    uint8_t value = fetch8BitValue();
#ifdef DEBUG_LOG
    std::cout << "CP " << (int)value << std::endl;
#endif
    sub(value,false, true);
}


// *********************************************************************************
// ADD
// *********************************************************************************


void CPU::adc_hl_nn(uint16_t value)
{
    add16(regs.HL,value,true);
}

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
    add16(regs.HL,specialRegs.SP);
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

// SBC HL,nn
// opcode: 0xed 01ss0010
void CPU::sbc_hl_nn( uint16_t value ){
    sub16(regs.HL,value,true);
}

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
    setFlag(FlagBitmaskSign, result & 0x8000 );

    bool isOperandsSignBitDifferent = (regs.HL ^ value_to_sub) & 0x80;
    bool didChangeSignInResult = (regs.HL ^ result) & 0x80;
    setFlag( FlagBitmaskPV, isOperandsSignBitDifferent && didChangeSignInResult );

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
    std::cout << "AND " << regName << " {A="<< (int)regs.A <<","<<regName<<"=" << (int)srcRegValue << "}" << std::endl;
#endif
}

void CPU::and_n()
{
    and_a_with_value(fetch8BitValue());
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
}

// opcode: 0xee
// cycles: 7
void CPU::xor_n()
{
    xor_a_with_value(fetch8BitValue());
}

void CPU::xor_a_with_value( uint8_t value )
{
    regs.A ^= value;

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
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

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
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
    specialRegs.SP--;
}

// opcode: 0x3c
// flags: -
void CPU::INC_A()
{
    inc_r(regs.A);
}

// opcode: 0x3d
void CPU::DEC_A()
{
    dec_r(regs.A);
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
    inc_r(regs.L);
}

// DEC L
// opcode: 0x2d
// cycles: 4
void CPU::DEC_L(){
    dec_r(regs.L);
}


// INC SP
// opcode: 0x33
// flags: -
void CPU::INC_SP()
{
    specialRegs.SP++;

}

// INC (HL)
// opcode: 0x34
// N PV H S Z
void CPU::INC_pHL()
{
    uint8_t result = mem[regs.HL]++;
    setFlag(FlagBitmaskSign, result & 0x80);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00010000 );
    setFlag(FlagBitmaskPV, result ==  0x80);
    setFlag(FlagBitmaskN,0);
}

// DEC (HL)
// opcode: 0x35
void CPU::DEC_pHL()
{
    uint8_t result = mem[regs.HL]--;
    setFlag(FlagBitmaskSign, result & 0x80);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00001111 );
    setFlag(FlagBitmaskPV, result ==  0x7f);
    setFlag(FlagBitmaskN,1);
}

// inc bc
// Opcode: 03
// Cycles: 06
// Flags: -
void CPU::inc_bc(){
    regs.C++;
    if(regs.C==0) regs.B++;
#ifdef DEBUG_LOG
    std::cout << "INC BC" << std::endl;
#endif
}

// inc b
// opcode: 04
// cycles: 4
// flags: S Z HC PV N
void CPU::inc_b(){
    inc_r(regs.B);
}

// dec b
// opcode: 0x05
// cycles: 4
void CPU::dec_b(){
    dec_r(regs.B);
}


// DEC BC
// opcode: 0x0b
// cycles: 6
void CPU::dec_bc(){
    regs.BC--;
}

// INC C
// opcode: 0x0c
// cycles: 4
void CPU::inc_c(){
    inc_r(regs.C);
}

// DEC C
// opcode: 0x0d
// cycles: 4
void CPU::dec_c(){
    dec_r(regs.C);
}

// inc de
// Opcode: 0x13
// Cycles: 06
// Flags: -
void CPU::inc_de(){
    regs.E++;
    if(regs.E==0) regs.D++;
}

// inc d
// opcode: 0x14
void CPU::inc_d(){
    inc_r(regs.D);
}

// dec d
// opcode: 0x15
void CPU::dec_d(){
    dec_r(regs.D);
}

// INC HL
// Opcode: 0x23
// Cycles: 06
// Flags: -
void CPU::inc_hl(){
    regs.L++;
    if(regs.L==0) regs.H++;
}

// INC H
// opcodE: 0x24
void  CPU::inc_h(){
    inc_r(regs.H);
}

// DEC H
// opcode: 0x25
void  CPU::dec_h(){
    dec_r(regs.H);
}

// DEC DE
// opcode: 0x1b
// cycles: 6
void CPU::DEC_DE(){
    regs.DE--;
}

// INC E
// opcode: 0x1c
// cycles 4
void CPU::INC_E(){
    inc_r(regs.E);
}

// DEC E
// opcode: 0x1d
// cycles: 4
void CPU::DEC_E(){
    dec_r(regs.E);
}