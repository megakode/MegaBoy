//
// Created by sbeam on 03/12/2021.
//

#pragma once

#include <format>

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

// ***********************************************************************************
// Load Instructions
// ***********************************************************************************

void CPU::LD_pnn_SP() {
    LD_pnn_rr(fetch16BitValue(), regs.SP);
}


void CPU::LD_r_r()
{
    uint8_t dstRegCode = (current_opcode >> 3) & 0b111;
    uint8_t srcRegCode = current_opcode & 0b111;

    uint8_t& dst = reg_from_regcode(dstRegCode);
    uint8_t& src = reg_from_regcode(srcRegCode);

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
    dstReg = value;

    #ifdef DEBUG_LOG
    auto regName =  reg_name_from_regcode(dstRegCode);
    AddDebugLog(std::format("LD {},{:#04x}",regName,value));
    #endif
}

void CPU::LD_R_A() {
    regs.R = regs.A;
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
    regs.SP = fetch16BitValue();
#ifdef DEBUG_LOG
    AddDebugLog(std::format("LD SP,{:#06x}",regs.SP));
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

// LD SP,HL
// opcode: 0xf9
// flags: -
// cycles: 6
void CPU::ld_sp_hl()
{
    regs.SP = regs.HL;
    #ifdef DEBUG_LOG
    AddDebugLog("LD SP,HL");
    #endif
}

// ld bc,nn
// opcode: 01 n  n
// cycles: 10
void CPU::LD_BC_nn()
{
    regs.C = fetch8BitValue();
    regs.B = fetch8BitValue();

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD BC,{:#06x}", regs.BC));
    #endif
};

// ld (bc),a
// Opcode: 02
// Cycles: 7
void CPU::LD_pBC_A()
{
    mem[regs.BC] = regs.A;

    #ifdef DEBUG_LOG
    AddDebugLog("LD (BC),A");
    #endif
}

// LD A,(BC)
// opcode: 0x0A
// cycles: 7
// flags: -
void CPU::LD_A_pBC()
{
    regs.A = mem[regs.BC];

    #ifdef DEBUG_LOG
    AddDebugLog("LD A,(BC)");
    #endif
}

// ld de,nn
// opcode: 11 n  n
// cycles: 10
void CPU::LD_DE_nn()
{
    regs.DE = fetch16BitValue();

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("LD DE,{:#06x}", regs.DE));
    #endif
};

// load (de),a
// opcode: 0x12
// cycles: 7
void CPU::LD_pDE_A()
{
    mem[regs.DE] = regs.A;

    #ifdef DEBUG_LOG
    AddDebugLog("LD (DE),A");
    #endif
}

// LD A,(DE)
// opcode: 0x1a
// cycles: 7
// flags: -
void CPU::LD_A_pDE()
{
    regs.A = mem[regs.DE];

#ifdef DEBUG_LOG
    AddDebugLog("LD A,(DE)");
#endif
}

/// LDI  (HL),A
/// opcode: 22
/// cycles: 8
void CPU::LDI_pHL_A()
{
    mem[regs.HL++] = regs.A;
}

/// LDI  A,(HL)
/// opcode: 2a
/// cycles: 8
void CPU::LDI_A_pHL()
{
    regs.A = mem[regs.HL++];
}

/// LDD (HL),A
/// opcode: 0x32
/// cycles: 8
void CPU::LDD_pHL_A()
{
    mem[regs.HL--] = regs.A;
}

/// LDD  A,(HL)
/// opcode: 0x3a
/// cycles: 8
void CPU::LDD_A_pHL()
{
    regs.A = mem[regs.HL--];
}

/// LD (0xff00 + n),A
/// opcode: e0 nn
/// cycles: 8
/// flags: -
void CPU::LD_ff00n_A()
{
    uint8_t lowbyte = fetch8BitValue();
    uint16_t addr = 0xff00 + lowbyte;
    mem[addr] = regs.A;
}

/// LD (FF00+C),A
/// opcode: 0xe2
void CPU::LD_ff00C_A()
{
    uint16_t addr = 0xff00 + regs.C;
    mem[addr] = regs.A;
}

/// LD A,(0xff00 + n)
/// opcode: f0 nn
/// cycles: 8
/// flags: -
void CPU::LD_A_ff00n()
{
    uint8_t lowbyte = fetch8BitValue();
    uint16_t addr = 0xff00 + lowbyte;
    regs.A = mem[addr];
}

/// LD A,(FF00+C)
/// opcode: 0xe2
void CPU::LD_A_ff00C()
{
    uint16_t addr = 0xff00 + regs.C;
    regs.A = mem[addr];
}


/// LD HL,SP+dd
/// HL = SP +/- dd ; dd is 8-bit signed number
/// cycles: 12
void CPU::LD_HL_SPs8()
{
    auto value = static_cast<int8_t>(fetch8BitValue());
    regs.HL = regs.SP + value;
}

/// LD A,(nn)
/// opcode: 0xfa
/// cycles: 16
void CPU::LD_A_pnn()
{
    regs.A = mem[fetch16BitValue()];
}



// ********************************************************************************
// PUSH / POP
// ********************************************************************************

/// Pop 2 bytes of the stack into a 16 bit register
/// \param regPair The 16 bit register pair to pop the stack value into
void CPU::pop16( uint16_t& regPair )
{
    uint8_t lobyte = mem[regs.SP++];
    uint8_t hibyte = mem[regs.SP++];
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

/// PUSH BC
/// cycles: 11
void CPU::push_bc(){
    mem[--regs.SP] = regs.B;
    mem[--regs.SP] = regs.C;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH BC");
#endif
}

/// PUSH DE
/// cycles: 11
void CPU::push_de(){
    mem[--regs.SP] = regs.D;
    mem[--regs.SP] = regs.E;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH DE");
#endif
}

/// PUSH HL
/// cycles: 11
void CPU::push_hl(){
    mem[--regs.SP] = regs.H;
    mem[--regs.SP] = regs.L;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH HL");
#endif
}

/// PUSH AF
/// cycles: 11
void CPU::push_af(){
    mem[--regs.SP] = regs.A;
    mem[--regs.SP] = regs.F;
#ifdef DEBUG_LOG
    AddDebugLog("PUSH AF");
#endif
}

void CPU::push_pc(){
    mem[--regs.SP] = regs.PC >> 8; // hi-byte
    mem[--regs.SP] = regs.PC & 0xff; // lo-byte
}
