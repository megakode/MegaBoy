//
// Created by sbeam on 03/12/2021.
//

#pragma once

#include <format>

// cycles: 4 (8 when src or dst is an index register: ixl,ixh,iyl,iyh
void CPU::LD_r(uint8_t& dstReg, uint8_t value )
{
    dstReg = value;
}

void CPU::LD_rr(uint16_t& dstReg, uint16_t& value )
{
    dstReg = value;
}


void CPU::ld_r_r()
{
    uint8_t dstRegCode = (current_opcode >> 3) & 0b111;
    uint8_t srcRegCode = current_opcode & 0b111;

    auto dst = reg_from_regcode(dstRegCode);
    auto src = reg_from_regcode(srcRegCode);

    dst = src;

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD {},{}",reg_name_from_regcode(dstRegCode),reg_name_from_regcode(srcRegCode)));
#endif

}

void CPU::LD_r_n()
{
    uint8_t dstRegCode = (current_opcode >> 3) & 0b111;
    uint8_t& dstReg = reg_from_regcode(dstRegCode);
    uint8_t value = fetch8BitValue();
    auto regName =  reg_name_from_regcode(dstRegCode);

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD {},{:#04x}",regName,value));
    #endif
}

// LD HL,NN
// opcode: 0x21
// cycles: 10
// flags: -
void CPU::LD_HL_nn(){
    regs.L = fetch8BitValue();
    regs.H = fetch8BitValue();

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD HL,{:#06x}",regs.HL));
#endif
}

// LD SP,NN
// opcode: 0x31
// flags: -
void CPU::LD_SP_nn()
{
    specialRegs.SP = fetch16BitValue();
#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD SP,{:#06x}",specialRegs.SP));
#endif
}

// LD (NN),A
// opcode: 0x32
// flags: -
void CPU::LD_pnn_A()
{
    auto addr = fetch16BitValue();
    mem[fetch16BitValue()] = regs.A;
#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD ({:#06x}),A",addr));
#endif
}

// LD (**),BC
// LD (**),DE
// LD (**),HL
// LD (**),SP
// LD (**),IX
// LD (**),IY
// cycles: 20
// flags: -
void CPU::LD_pnn_rr(uint16_t location, uint16_t value)
{
    mem[location] = static_cast<uint8_t>(value);
    mem[location+1] = value >> 8;
}

// LD rr,(nn)
//
// LD BC,(**)
// LD DE,(**)
// LD HL,(**)
// LD SP,(**)
// LD IX,(**)
// LD IY,(**)
//
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
    AddDebugLog(std::format("LD HL,({:#06x})",addr));
#endif
}

// LD (NN),HL
// opcode: 0x22
// flags: -
void CPU::LD_pnn_HL(){
    uint16_t addr = fetch16BitValue();
    mem[addr] = regs.L;
    mem[addr+1] = regs.H;
#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD ({:#06x}),HL",addr));
#endif
}

// LD (HL),N
// opcode: 0x36
void CPU::LD_pHL_n()
{
    uint8_t value = fetch8BitValue();
    mem[regs.HL] = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD (HL),{:#04x}", value));
    #endif
}

// LD IX,nn
// opcode: 0xdd 0x21
// cycles: 14
void CPU::LD_IX_nn()
{
    specialRegs.IX = fetch16BitValue();

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IX,{:#06x}", specialRegs.IX));
    #endif
}

// LD IX,nn
// opcode: 0xfd 0x21
// cycles: 14
void CPU::ld_iy_nn()
{
    specialRegs.IY = fetch16BitValue();

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IY,{:#06x}", specialRegs.IY));
#endif
}

// LD IX,(nn)
// opcode: 0xdd 0x2a
// cycles: 20
// flags: -
void CPU::LD_IX_pnn()
{
    uint16_t addr = fetch16BitValue();
    specialRegs.IXH = mem[addr+1];
    specialRegs.IXL = mem[addr];

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IX,({:#06x})", addr));
    #endif
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

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IY,({:#06x})", addr));
    #endif
}

// LD SP,HL
// opcode: 0xf9
// flags: -
// cycles: 6
void CPU::ld_sp_hl()
{
    specialRegs.SP = regs.HL;
    #ifdef DEBUG_LOG
    AddDebugLog("LD SP,HL");
    #endif
}

// ld bc,nn
// opcode: 01 n  n
// cycles: 10
void CPU::LD_BC_nn(){
    regs.C = fetch8BitValue();
    regs.B = fetch8BitValue();

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD BC,{:#06x}", regs.BC));
    #endif
};

// ld (bc),a
// Opcode: 02
// Cycles: 7
void CPU::LD_pBC_A(){
    mem[regs.BC] = regs.A;

    #ifdef DEBUG_LOG
    AddDebugLog("LD (BC),A");
    #endif
}

// LD A,(BC)
// opcode: 0x0A
// cycles: 7
// flags: -
void CPU::LD_A_pBC(){
    regs.A = mem[regs.BC];

    #ifdef DEBUG_LOG
    AddDebugLog("LD A,(BC)");
    #endif
}

// ld de,nn
// opcode: 11 n  n
// cycles: 10
void CPU::load_de_nn(){
    regs.E = fetch8BitValue();
    regs.D = fetch8BitValue();

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD DE,{:#06x}", regs.DE));
    #endif
};

// load (de),a
// opcode: 0x12
// cycles: 7
void CPU::load_ptr_de_a(){
    mem[regs.DE] = regs.A;

    #ifdef DEBUG_LOG
    AddDebugLog("LD (DE),A");
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

#ifdef DEBUG_LOG
    AddDebugLog("LD A,I");
#endif
}

// LD A,(DE)
// opcode: 0x1a
// cycles: 7
// flags: -
void CPU::LD_A_pDE(){
    regs.A = mem[regs.DE];

#ifdef DEBUG_LOG
    AddDebugLog("LD A,(DE)");
#endif
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

    #ifdef DEBUG_LOG
    AddDebugLog("A,R");
    #endif
}

// LD A,(NN)
// opcode: 0x3a
void CPU::ld_a_ptr_nn()
{
    uint16_t addr = fetch16BitValue();
    regs.A = mem[addr];

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD A,({:#06x})",addr));
    #endif
}


// LD IXH,n
// cycles: 8
void CPU::LD_IXH_n(uint8_t value )
{
    specialRegs.IXH = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IXH,{:#04x}",value));
    #endif
}

// LD IXL,n
// cycles: 8
void CPU::LD_IXL_n(uint8_t value )
{
    specialRegs.IXL = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IXL,{:#04x}",value));
    #endif
}

// LD IYH,n
// cycles: 8
void CPU::ld_iyh_n( uint8_t value )
{
    specialRegs.IYH = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IYH({:#04x}",value));
    #endif
}

// LD IYL,n
// cycles: 8
void CPU::ld_iyl_n( uint8_t value )
{
    specialRegs.IYL = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD IYL,{:#04x}",value));
    #endif
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::LD_pIXn_n()
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto value = fetch8BitValue();
    mem[specialRegs.IX + offset] = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD (IX+{}),{:#04x}",offset,value));
    #endif
}

// LD (IY+d),n
// cycles: 19
// flags: -
void CPU::LD_pIYn_n()
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto value =  fetch8BitValue();
    mem[specialRegs.IY + offset] = value;

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD (IY+{}),{:#04x}",offset,value));
    #endif
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::LD_pIXn_r(uint8_t& reg )
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto regName = reg_name_from_regcode(reg);

    mem[specialRegs.IX+offset] = reg;

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD (IX+{}),{1}",offset,regName));
#endif
}

// LD (IY+d),n
// cycles: 19
// flags: -
void CPU::LD_pIYn_r( uint8_t& reg)
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto regName = reg_name_from_regcode(reg);

    mem[specialRegs.IY+offset] = reg;

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD (IY+{0}),{1}",offset,regName));
#endif
}

// LD r,(ix+n)
// cycles: 19
void CPU::LD_r_pIXn(uint8_t& reg )
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto regName = reg_name_from_regcode(reg);

    reg = mem[specialRegs.IX+offset];

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD {1},(IX+{0})",offset,regName));
#endif
}

// LD r,(iy+n)
// cycles: 19
void CPU::LD_r_pIYn( uint8_t& reg )
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    auto regName = reg_name_from_regcode(reg);

    reg = mem[specialRegs.IY+offset];

#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD {1},(IY+{0})",offset,regName));
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
    uint8_t regPairCode = (current_opcode & 0b00110000) >> 4;
    switch (regPairCode)
    {
        case 0:
            pop16(regs.BC);
            #ifdef DEBUG_LOG
            AddDebugLog("POP BC");
            #endif
            break;
        case 1:
            pop16(regs.DE);
            #ifdef DEBUG_LOG
            AddDebugLog("POP DE");
            #endif
            break;
        case 2:
            pop16(regs.HL);
            #ifdef DEBUG_LOG
            AddDebugLog("POP HL");
            #endif
            break;
        case 3:
            pop16(regs.AF);
            #ifdef DEBUG_LOG
            AddDebugLog("POP AF");
            #endif
            break;
    }
};


// cyckes; 11
void CPU::push_bc(){
    mem[--specialRegs.SP] = regs.B;
    mem[--specialRegs.SP] = regs.C;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH BC");
#endif
}

// cyckes; 11
void CPU::push_de(){
    mem[--specialRegs.SP] = regs.D;
    mem[--specialRegs.SP] = regs.E;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH DE");
#endif
}

// cyckes; 11
void CPU::push_hl(){
    mem[--specialRegs.SP] = regs.H;
    mem[--specialRegs.SP] = regs.L;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH HL");
#endif
}

// cyckes; 11
void CPU::push_af(){
    mem[--specialRegs.SP] = regs.A;
    mem[--specialRegs.SP] = regs.F;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH AF");
#endif
}

// cycles 15
void CPU::push_ix(){
    mem[--specialRegs.SP] = specialRegs.IXH;
    mem[--specialRegs.SP] = specialRegs.IXL;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH IX");
#endif
}

// cycles 15
void CPU::push_iy(){
    mem[--specialRegs.SP] = specialRegs.IYH;
    mem[--specialRegs.SP] = specialRegs.IYL;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH IY");
#endif
}
