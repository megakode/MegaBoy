//
// Created by sbeam on 03/12/2021.
//

#pragma once

// cycles: 4 (8 when src or dst is an index register: ixl,ixh,iyl,iyh
void CPU::LD_r_r(uint8_t& dstReg, uint8_t value )
{
    dstReg = value;
}

void CPU::ld_rr_rr( uint16_t& dstReg, uint16_t& value )
{
    dstReg = value;
}


void CPU::ld_r_r()
{
    uint8_t dstRegCode = (currentOpcode >> 3) & 0b111;
    uint8_t srcRegCode = currentOpcode & 0b111;

    auto dst = reg_from_regcode(dstRegCode);
    auto src = reg_from_regcode(srcRegCode);

    dst = src;
#ifdef DEBUG_LOG
    std::cout << "LD " << reg_name_from_regcode(dstRegCode) << "," << reg_name_from_regcode(srcRegCode) << " {" << (int)src << "}" << std::endl;
#endif
/*
    if(srcRegCode == RegisterCode::HLPtr)
    {
        // LD R,(HL)
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = mem[regs.HL]; // Get data from HL location
    }
    if(dstRegCode == RegisterCode::HLPtr)
    {
        // LD (HL),R
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        mem[regs.HL] = *srcRegPtr;
    }
    else
    { // LD R,R
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = *srcRegPtr;
    }*/
}


// LD HL,NN
// opcode: 0x21
// cycles: 10
// flags: -
void CPU::LD_HL_nn(){
    regs.L = fetch8BitValue();
    regs.H = fetch8BitValue();

}

void CPU::LD_H_n(){
    regs.H = fetch8BitValue();
}

// LD L,n
// opcode: 0x2E
// cycles: 7
void CPU::LD_L_n(){
    regs.L = fetch8BitValue();
}


// LD SP,NN
// opcode: 0x31
// flags: -
void CPU::LD_SP_nn()
{
    specialRegs.SP = fetch16BitValue();
#ifdef DEBUG_LOG
    std::cout << "LD SP," << specialRegs.SP << std::endl;
#endif
}

// LD (NN),A
// opcode: 0x32
// flags: -
void CPU::LD_pnn_A()
{
    mem[fetch16BitValue()] = regs.A;
}

// cycles: 20
// flags: -
void CPU::LD_pnn_rr(uint16_t location, uint16_t value)
{
    mem[location] = static_cast<uint8_t>(value);
    mem[location+1] = value >> 8;
}

// LD rr,(nn)
// cycles: 20
// flags: -
void CPU::LD_rr_pnn(uint16_t& regPair, uint16_t addr )
{
    uint16_t value = mem[addr] + (mem[addr+1]<<8);
    regPair = value;
}

// ld hl,(nn)
// opcode: 0x2a
// cycles: 16
// flags: -
void CPU::LD_HL_pnn(){
    uint16_t addr = fetch16BitValue();
    regs.L = mem[addr];
    regs.H = mem[addr+1];

#ifdef DEBUG_LOG
    std::cout << "LD HL,(" << addr << ") [hl=" << regs.HL << "]" << std::endl;
#endif
}

// LD (NN),HL
// opcode: 0x22
// flags: -
void CPU::LD_pnn_HL(){
    uint16_t addr = fetch16BitValue();
    mem[addr] = regs.L;
    mem[addr+1] = regs.H;
}

// load d,n
// opcode: 0x16
// cycles: 7
void CPU::LD_D_n(){
    regs.D = fetch8BitValue();
}

// LD (HL),N
// opcode: 0x36
void CPU::ld_ptr_hl_n()
{
    mem[regs.HL] = fetch8BitValue();
}

// opcode: 0x3e
// flags: -
void CPU::ld_a_n()
{
    regs.A = fetch8BitValue();

#ifdef DEBUG_LOG
    std::cout << "LD A," << (int)regs.A << std::endl;
#endif

}

// LD IX,nn
// opcode: 0xdd 0x21
// cycles: 14
void CPU::ld_ix_nn()
{
    specialRegs.IX = fetch16BitValue();
}

// LD IX,nn
// opcode: 0xfd 0x21
// cycles: 14
void CPU::ld_iy_nn()
{
    specialRegs.IY = fetch16BitValue();
}

// LD IX,(nn)
// opcode: 0xdd 0x2a
// cycles: 20
// flags: -
void CPU::ld_ix_ptr_nn()
{
    uint16_t addr = fetch16BitValue();
    specialRegs.IXH = mem[addr+1];
    specialRegs.IXL = mem[addr];
}

// LD IY,(nn)
// opcode: 0xfd 0x2a
// cycles: 20
// flags: -
void CPU::ld_iy_ptr_nn()
{
    uint16_t addr = fetch16BitValue();
    specialRegs.IYH = mem[addr+1];
    specialRegs.IYL = mem[addr];
}

// LD SP,HL
// opcode: 0xf9
// flags: -
// cycles: 6
void CPU::ld_sp_hl()
{
    specialRegs.SP = regs.HL;
#ifdef DEBUG_LOG
    std::cout << "LD SP,HL [sp=" << specialRegs.SP << ",hl=" << regs.HL << "]" << std::endl;
#endif
}

// LD E,n
// opcode: 0x1e
// cycles: 7
// flags: -
void CPU::LD_E_n(){
    regs.E = fetch8BitValue();
}

// Load B,N
// opcode: 0x06
// cycles: 7
void CPU::LD_B_n(){
    regs.B = fetch8BitValue();
}


// ld bc,nn
// opcode: 01 n  n
// cycles: 10
void CPU::LD_BC_nn(){
    regs.C = fetch8BitValue();
    regs.B = fetch8BitValue();
#ifdef DEBUG_LOG
    std::cout << "LD BC," << regs.BC << std::endl;
#endif
};

// ld (bc),a
// Opcode: 02
// Cycles: 7
void CPU::LD_pBC_A(){
    mem[regs.BC] = regs.A;
#ifdef DEBUG_LOG
    std::cout << "LD (BC),A [BC=" << regs.BC << ",A=" << (int)regs.A << "]" << std::endl;
#endif
}

// LD A,(BC)
// opcode: 0x0A
// cycles: 7
// flags: -
void CPU::LD_A_pBC(){
    regs.A = mem[regs.BC];
}


// LD C,N
// opcode: 0x0e
// cycles: 7
void CPU::LD_C_n(){
    regs.C = fetch8BitValue();
#ifdef DEBUG_LOG
    std::cout << "LD C," << (int)regs.C << std::endl;
#endif
}


// ld de,nn
// opcode: 11 n  n
// cycles: 10
void CPU::load_de_nn(){
    regs.E = fetch8BitValue();
    regs.D = fetch8BitValue();
#ifdef DEBUG_LOG
    std::cout << "LD DE," << regs.DE << std::endl;
#endif
};

// load (de),a
// opcode: 0x12
// cycles: 7
void CPU::load_ptr_de_a(){
    mem[regs.DE] = regs.A;
#ifdef DEBUG_LOG
    std::cout << "LD (DE),A" << std::endl;
#endif
}

// LD a,i - load the interrupt vector register I into A
// opcode: 0xed 0x57
// cycles: 9
void CPU::LD_A_I()
{
    regs.A = specialRegs.I;
    regs.F &= 0b00000001;
    // Bits affected:
    setFlag(FlagBitmaskSign,specialRegs.I & 0x80);
    setFlag(FlagBitmaskZero, specialRegs.I == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV,IFF2); // P/V: contents of IFF2
    setFlag(FlagBitmaskN,0);
}

// LD A,(DE)
// opcode: 0x1a
// cycles: 7
// flags: -
void CPU::LD_A_pDE(){
    regs.A = mem[regs.DE];
}

// Load A,R - Loads memory refresh register R into A
// opcode: 0xed 0x5f
// cycles: 9
void CPU::LD_A_R()
{
    regs.A = specialRegs.R;
    setFlag(FlagBitmaskSign, specialRegs.R & 0x80);
    setFlag(FlagBitmaskZero, specialRegs.R == 0);
    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskPV, IFF2); // "If an interrupt occurs during execution of this instruction, the Parity flag will contain a 0" (p.51 Z80 tech ref)
    setFlag(FlagBitmaskN,0);
}

// LD A,(NN)
// opcode: 0x3a
void CPU::ld_a_ptr_nn()
{
    regs.A = mem[fetch16BitValue()];
}


// LD IXH,n
// cycles: 8
void CPU::ld_ixh_n( uint8_t value )
{
    specialRegs.IXH = value;
}

// LD IXL,n
// cycles: 8
void CPU::ld_ixl_n( uint8_t value )
{
    specialRegs.IXL = value;
}

// LD IYH,n
// cycles: 8
void CPU::ld_iyh_n( uint8_t value )
{
    specialRegs.IYH = value;
}

// LD IYL,n
// cycles: 8
void CPU::ld_iyl_n( uint8_t value )
{
    specialRegs.IYL = value;
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::LD_pIXn_n()
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    mem[specialRegs.IX + offset] = fetch8BitValue();
}

// LD (IY+d),n
// cycles: 19
// flags: -
void CPU::LD_pIYn_n()
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    mem[specialRegs.IY + offset] = fetch8BitValue();
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::LD_pIXn_r(uint8_t& reg )
{
    auto ptr = fetch_pIXn();
    ptr = reg;
}

// LD (IY+d),n
// cycles: 19
// flags: -
void CPU::LD_pIYn_r( uint8_t& reg)
{
    auto ptr = fetch_pIYn();
    ptr = reg;
}

// LD r,(ix+n)
// cycles: 19
void CPU::LD_r_pIXn(uint8_t& dst_reg )
{
    dst_reg = fetch_pIXn();
}

// LD r,(iy+n)
// cycles: 19
void CPU::LD_r_pIYn( uint8_t& dst_reg )
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    dst_reg = mem[specialRegs.IY + offset];

#ifdef DEBUG_LOG
    std::cout << "LD r,(IY+n) [IY+n=" << specialRegs.IY + offset << "]" << std::endl;
#endif
}

// ********************************************************************************
// PUSH / POP
// ********************************************************************************

/// Pop 2 bytes of the stack into a 16 bit register
/// \param regPair The 16 bit register pair to pop the stack value into
void CPU::pop16( uint16_t& regPair )
{
    uint8_t lobyte = mem[specialRegs.SP++];
    uint8_t hibyte = mem[specialRegs.SP++];
    regPair = (hibyte << 8) + lobyte;
}

// cycles: 10
void CPU::pop_qq()
{
    uint8_t regPairCode = (currentOpcode & 0b00110000) >> 4;
    switch (regPairCode)
    {
        case 0:
            pop16(regs.BC);
#ifdef DEBUG_LOG
            std::cout << "POP BC" << std::endl;
#endif
            break;
        case 1:
            pop16(regs.DE);
#ifdef DEBUG_LOG
            std::cout << "POP DE" << std::endl;
#endif
            break;
        case 2:
            pop16(regs.HL);
#ifdef DEBUG_LOG
            std::cout << "POP HL" << std::endl;
#endif
            break;
        case 3:
            pop16(regs.AF);
#ifdef DEBUG_LOG
            std::cout << "POP AF" << std::endl;
#endif
            break;
    }
};


// cyckes; 11
void CPU::push_bc(){
    mem[--specialRegs.SP] = regs.B;
    mem[--specialRegs.SP] = regs.C;
#ifdef DEBUG_LOG
    std::cout << "PUSH BC" << std::endl;
#endif
}

// cyckes; 11
void CPU::push_de(){
    mem[--specialRegs.SP] = regs.D;
    mem[--specialRegs.SP] = regs.E;
#ifdef DEBUG_LOG
    std::cout << "PUSH DE" << std::endl;
#endif
}

// cyckes; 11
void CPU::push_hl(){
    mem[--specialRegs.SP] = regs.H;
    mem[--specialRegs.SP] = regs.L;
#ifdef DEBUG_LOG
    std::cout << "PUSH HL" << std::endl;
#endif
}

// cyckes; 11
void CPU::push_af(){
    mem[--specialRegs.SP] = regs.A;
    mem[--specialRegs.SP] = regs.F;
#ifdef DEBUG_LOG
    std::cout << "PUSH AF" << std::endl;
#endif
}

// cycles 15
void CPU::push_ix(){
    mem[--specialRegs.SP] = specialRegs.IXH;
    mem[--specialRegs.SP] = specialRegs.IXL;
#ifdef DEBUG_LOG
    std::cout << "PUSH IX" << std::endl;
#endif
}

// cycles 15
void CPU::push_iy(){
    mem[--specialRegs.SP] = specialRegs.IYH;
    mem[--specialRegs.SP] = specialRegs.IYL;
#ifdef DEBUG_LOG
    std::cout << "PUSH IY" << std::endl;
#endif
}
