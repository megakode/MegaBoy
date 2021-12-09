//
// Created by Peter Bone on 27/11/2021.
//

#pragma once

/// Perform a bit instruction from the "bit opcode group"
/// \param op2 The opcode which contains the information about the operation
/// \param reg The register to operate on

void CPU::do_bit_instruction( uint8_t op2, uint8_t& reg )
{
    // RLC r
    // opcode: 0x00 - 0x07
    // cycles: 8
    if( op2 >= 0 && op2 <= 0x07 )
    {
        uint8_t carry = reg & 0x80;
        reg = reg << 1;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 1 : 0);
#ifdef DEBUG_LOG
        AddDebugLog("RLC r");
#endif

    } else

        // RRC r
        // opcode: 0x08 - 0x0f
        // cycles:
        // RRC r : 8
        // RRC (HL) : 15
        // RRC (IX+d): 23
        // RRC (IY+d): 23
    if( (op2 & 0b11111000) == 0b00001000)
    {
        uint8_t carry = reg & 0x01;
        reg = reg >> 1;

        setFlag(FlagBitmaskHalfCarry,false);
        setFlag(FlagBitmaskN,false);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 0x80 : 0);
#ifdef DEBUG_LOG
        AddDebugLog("RRC r");
#endif
    } else

        // RL r
        // opcode: 0x10 - 0x17
        // cycles:
        // RL r: 8
        // RL (hl): 15
        // RL (ix+d): 23
        // RL (iy+d): 23
        // flags: S Z H PV N C
    if( (op2 & 0b11111000) == 0b00010000)
    {
        uint8_t carry = reg & 0x80;

        reg = reg << 1;
        reg |= (regs.F & FlagBitmaskC) ? 1 : 0;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
#ifdef DEBUG_LOG
        AddDebugLog("RL r");
#endif
    } else

        // RR r
        // opcode: 0x18 - 0x1f
        // cycles:
        // RR r: 8
        // RR (hl): 15
        // RR (ix+d): 23
        // RR (iy+d): 23
        // flags: S Z H PV N C
    if( (op2 & 0b11111000) == 0b00011000)
    {
        uint8_t carry = reg & 0x01;

        reg = reg >> 1;
        reg |= (regs.F & FlagBitmaskC) ? 0x80 : 0;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
#ifdef DEBUG_LOG
        AddDebugLog("RR r");
#endif
    } else

        // SLA r
        // opcode: 0x20 - 0x27
        // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x20 && op2 <= 0x27 )
    {
        uint8_t carry = reg & 0x80;

        reg = reg << 1;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
#ifdef DEBUG_LOG
        AddDebugLog("SLA r");
#endif
    } else

        // SRA r
        // opcode: 0x28 - 0x2f
        // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x28 && op2 <= 0x2f )
    {
        uint8_t regCode = op2 & 0b00000111;
        uint8_t &reg = reg_from_regcode(regCode);
        uint8_t carry = reg & 0x01;
        int8_t signedValue = reg;
        // TODO: page 210. Make sure this is an arithmetic shift that preserves bit 7
        signedValue = signedValue >> 1;
        reg = signedValue;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
#ifdef DEBUG_LOG
        auto regName = reg_name_from_regcode(regCode);
        AddDebugLog(std::format("SRA {}",regName));
#endif
    } else

    // opcode: 0x30 - 0x37
    if( op2 >= 0x30 && op2 <= 0x37 ){

        uint8_t regCode = op2 & 0b00000111;
        SWAP_r(regCode);

    } else

        // SRL r
        // Shift right
    if( op2 >= 0x38 && op2 <= 0x3f)
    {
        uint8_t regCode = op2 & 0b00000111;
        uint8_t &reg = reg_from_regcode(regCode);
        uint8_t carry = reg & 0x01;

        reg = reg >> 1;

        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
#ifdef DEBUG_LOG
        auto regName = reg_name_from_regcode(regCode);
        AddDebugLog(std::format("SRL {}",regName));
#endif
    } else


        // BIT b,r
        // Test bit b in register r and sets Z flag accordingly
        // opcode: 0x40 - 0x7f
        // flag: Z H N
    if( op2 >= 0x40 && op2 <= 0x7f )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t result = reg & (1 << bitNumber);
        setFlag(FlagBitmaskZero, result == 0);
        setFlag(FlagBitmaskHalfCarry,1);
        setFlag(FlagBitmaskN,0);
#ifdef DEBUG_LOG
        AddDebugLog("BIT b,r");
#endif
    } else

        // RES b,r
        // Reset bit b in register r
        // opcode: 0x80 - 0xbf (01 bbb rrr)
        // flags: -
        // cycles: 8
    if( op2 >= 0x80 && op2 <= 0xbf )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t bitMask = ~(1 << bitNumber);
        reg &= bitMask;
#ifdef DEBUG_LOG
        AddDebugLog("RES b,r");
#endif
    } else

        // SET b,r
        // Set bit b in register r
        // opcode: 0xc0 - 0xff (01 bbb rrr)
        // flags: -
        // cycles: 8
    if( op2 >= 0xc0 && op2 <= 0xff )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t bitMask = (1 << bitNumber);
        reg |= bitMask;
#ifdef DEBUG_LOG
        AddDebugLog("SET b,r");
#endif
    }

}

// RLCA
// opcode: 0x07
// cycles: 4
void CPU::rlca(){
    bool carry = regs.A & 0b10000000;
    setFlag(FlagBitmaskC,carry);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskN,0);

    regs.A <<= 1;
    if(carry)regs.A |= 1;

#ifdef DEBUG_LOG
    AddDebugLog("RLCA");
#endif
}


// RRCA - Rotate right with carry A
// opcode: 0x0f
// cycles: 4
// flags: C N H
void CPU::rrca(){
    uint8_t carry = regs.A & 1;
    setFlag(FlagBitmaskC,carry);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskHalfCarry,0);
    regs.A >>= 1;
    regs.A |= (carry<<7);
#ifdef DEBUG_LOG
    AddDebugLog("rrca");
#endif
}

// opcode: 0x17
// cycles: 4
void CPU::rla(){
    // rotate left and set carry
    // Set bit 0 to previous carry.
    uint8_t newcarry = regs.A & 0x80;
    regs.A <<= 1;
    regs.A |= (regs.F&FlagBitmaskC);
    setFlag(FlagBitmaskHalfCarry,false);
    setFlag(FlagBitmaskN,false);
    setFlag(FlagBitmaskC,newcarry);
#ifdef DEBUG_LOG
    AddDebugLog("RLA");
#endif
}

// RRA
//
// contents of A is rotated one bit right
// The contents of bit 0 are copied to the carry flag,
// and the previous contents of the carry flag are copied to bit 7
//
// opcode: 0x1f
// cycles: 4
// flags: C N H
void CPU::rra(){
    uint8_t carry = regs.A & 1;
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskHalfCarry,0);
    regs.A >>= 1;
    regs.A |= (regs.F<<7); // Set bit 7 to previous carry flag
    setFlag(FlagBitmaskC, carry); // Set new carry flag
#ifdef DEBUG_LOG
    AddDebugLog("RRA");
#endif
}

// RLD - do some swapping of nibbles between A and (HL) (see details on p.217 z80 tech ref.)
// opcode: 0xed 0x6f
// cycles: 18
// flags: s z h pv n
void CPU::RLD()
{
    uint16_t hl = regs.HL;
    uint8_t data = mem[hl];
    uint8_t old_data_hinib = data & 0xf0;
    data = data << 4;
    data |= (regs.A & 0x0f);
    regs.A &= 0xf0;
    regs.A |= (old_data_hinib>>4);
    mem[hl] = data;
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,false);
    setFlag(FlagBitmaskN,false);
#ifdef DEBUG_LOG
    AddDebugLog("RLD");
#endif
}

/// opcode: 0xCB 0x30-0x37
/// SWAP r
/// swap low/hi-nibble
void CPU::SWAP_r(uint8_t regCode)
{
    uint8_t& reg = reg_from_regcode(regCode);
    uint8_t temp = (reg & 0xf) << 4;
    reg >>= 4;
    reg &= temp;
    regs.F = 0;
    setFlag(FlagBitmaskZero,reg == 0);

#ifdef DEBUG_LOG
    auto regName = reg_name_from_regcode(regCode);
    AddDebugLog(std::format("SWAP {}",regName));
#endif
}
