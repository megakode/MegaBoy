//
// Created by Peter Bone on 27/11/2021.
//

#pragma once


/// Perform a bit instruction from the "bit opcode group"
/// \param op2 The opcode which contains the information about the operation
/// \param reg The register to operate on
/// \param copyResultReg Additionally copy the result to this register (used by the undocumented IX/IY bit opcodes). set to the same as reg to disable copying the result.

void CPU::do_bit_instruction( uint8_t op2, uint8_t& reg , uint8_t& copyResultToReg )
{
    // RLC r
    // opcode: 0x00 - 0x07
    // cycles: 8
    if( op2 >= 0 && op2 <= 0x07 )
    {
        uint8_t carry = reg & 0x80;
        reg = reg << 1;

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 1 : 0);
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

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,false);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,false);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 0x80 : 0);
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

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
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

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

        // SLA r
        // opcode: 0x20 - 0x27
        // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x20 && op2 <= 0x27 )
    {
        uint8_t carry = reg & 0x80;

        reg = reg << 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

        // SRA r
        // opcode: 0x28 - 0x2f
        // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x28 && op2 <= 0x2f )
    {
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x01;
        int8_t signedValue = reg;
        // TODO: page 210. Make sure this is an arithmetic shift that preserves bit 7
        signedValue = signedValue >> 1;
        reg = signedValue;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

        // SLL r
        // opcode: 0x30 - 0x37
    if( op2 >= 0x30 && op2 <= 0x37 ){
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x80;

        reg = reg << 1;
        reg |= 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

        // SRL r
        // Shift right
    if( op2 >= 0x38 && op2 <= 0x3f)
    {
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x01;

        reg = reg >> 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
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
    }

    // The undocumented IX and IY bit instructions also loads a copy of the result from (IX+d) or (IY+d)
    // into the register encoded in the opcode
    if( &reg != &copyResultToReg) {
        copyResultToReg = reg;
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
}