#include <cstdint>
#include <vector>
#include <iostream>
#include "cpu.h"

uint8_t& HostMemory::operator[] (int index)
{
    if (index >= UINT16_MAX) {
        std::cout << "Array index out of bound, exiting";
        exit(0);
    }
    return memory[index];
}

CPU::CPU() {

    // Based on tables from:
    // https://clrhome.org/table/

    opCodeLookup = { 
        {"INC",&CPU::NOP},                  // 0x00
        {"LD BC,NN",&CPU::load_bc_nn},      // 0x01
        {"LD BC,A",&CPU::load_bc_a},        // 0x02
        {"INC BC",&CPU::inc_bc},            // 0x03
        {"INC B",&CPU::inc_b},              // 0x04
        {"DEC B",&CPU::dec_b},              // 0x05
        {"LD B,N",&CPU::load_b_n},          // 0x06
        {"RLCA",&CPU::rlca},                // 0x07
        {"EXAF",&CPU::ex_af},               // 0x08
        {"ADD HL,BC",&CPU::add_hl_bc},      // 0x09
        {"LD A,(BC)",&CPU::load_a_ptr_bc},  // 0x0a
        {"DEC BC",&CPU::dec_bc},            // 0x0b
        {"INC C",&CPU::inc_c},              // 0x0c
        {"DEC C",&CPU::dec_c},              // 0x0d
        {"LD C,N",&CPU::load_c_n},          // 0x0e
        {"RRCA",&CPU::rrca},                // 0x0f

        {"DJNZ N",&CPU::djnz_n},            // 0x10
        {"LD DE,NN",&CPU::load_de_nn},      // 0x11
        {"LD (DE),A",&CPU::load_ptr_de_a},  // 0x12
        {"INC DE",&CPU::inc_de},            // 0x13
        {"INC D",&CPU::inc_d},              // 0x14
        {"DEC D",&CPU::dec_d},              // 0x15
        {"LD D,N",&CPU::load_d_n},          // 0x16
        {"RLA",&CPU::rla},                  // 0x17
        {"JR N",&CPU::jr_n},                // 0x18
        {"ADD HL,DE",&CPU::add_hl_de},      // 0x19
        {"LD A,(DE)",&CPU::load_a_ptr_de},  // 0x1a
        {"DEC DE",&CPU::dec_de},            // 0x1b
        {"INC E",&CPU::inc_e},              // 0x1c
        {"DEC E",&CPU::dec_e},              // 0x1d
        {"LD E,N",&CPU::ld_e_n},            // 0x1e
        {"RRA",&CPU::rra},                  // 0x1f

        {"JR NZ",&CPU::jr_nz},              // 0x20
        {"LD HL,NN",&CPU::ld_hl_nn},        // 0x21
        {"LD (NN),HL",&CPU::ld_ptr_nn_hl},  // 0x22
        {"INC HL",&CPU::inc_hl},            // 0x23
        {"INC H",&CPU::inc_h},              // 0x24
        {"DEC H",&CPU::dec_h},              // 0x25
        {"LD H,N",&CPU::ld_h_n},            // 0x26
        {"DAA",&CPU::daa},                  // 0x27
        {"JR Z",&CPU::jr_z},                // 0x28
        {"ADD HL,HL",&CPU::add_hl_hl},      // 0x29
        {"LD HL,(NN)",&CPU::ld_hl_ptr_nn},  // 0x2a
        {"DEC HL",&CPU::dec_hl},            // 0x2b
        {"INC L",&CPU::inc_l},              // 0x2c
        {"DEC L",&CPU::dec_l},              // 0x2d
        {"LD L,N",&CPU::ld_l_n},            // 0x2e
        {"CPL",&CPU::cpl},                  // 0x2f


    };

}

void CPU::step() {

    //assert(mem!=nullptr);
    
    // M1: OP Code fetch
    uint8_t op = fetchInstruction();

    if( op < opCodeLookup.size()  ){
        (this->*opCodeLookup[op].code)();
        return;
        // (this->*lookup[opcode].addrmode)();
    }

    // LD r,R  - (01 rrr RRR)
    if( (op & 0b11000000) == 0b01000000 ){
        uint8_t dstRegCode = (op >> 3) & 0b111;
        uint8_t srcRegCode = op & 0b111;
        loadRR(dstRegCode,srcRegCode);
    } else 

    // LD r,n - (00rrr110 nnnnnnnn)
    if( (op & 0b11000111) == 0b00000110 ) {
        uint8_t reg = (op & 0b00111000) >> 3;
        uint8_t value = fetchInstruction();
        loadRN(reg,value);
    } else 

    if( op == 0xDD ){
        uint8_t op2 = fetchInstruction();

        // Load r,(IX + d) (11 011 101,01rrr101,dddddddd)
        if( (op2 & 0b11000111) == 0b01000101){
            uint8_t reg = (op2 & 0b00111000) >> 3;
            int8_t d = fetchInstruction();
            uint16_t addr = specialRegs.IX + d;
            loadRN(reg,mem[addr]);
        } else 

        // Load (IX+d), r (11011101,01110rrr,dddddddd )
        if( (op2 & 0b11111000) == 0b01110000 ){
            uint8_t reg = (op2 & 0b111);
            int8_t d = fetchInstruction();
            uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
            mem[specialRegs.IX + d] = *regPtr;
        } else
        
        // Load (IX + d),n (DD,36,d,n)
        if(op2 == 0x36){
            int8_t d = fetchInstruction();
            uint8_t value = fetchInstruction();
            mem[specialRegs.IX + d] = value;
        } else 

        // Load IX,nn - M:4 T:14
        if(op2 == 0x21){
            specialRegs.IX = fetch16BitAddress();
        }

        // Load IX,(nn) - M:6 T:20
        if(op2 == 0x2a){
            uint16_t addr = fetch16BitAddress();
            uint8_t lobyte = mem[addr];
            uint8_t hibyte = mem[addr+1];
            specialRegs.IX = (hibyte<<8) + lobyte;
        }

        // Load (nn),IX - (0xdd,0x22,n,n)- M:6 T:20
        if(op2 == 0x22){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = (uint8_t)specialRegs.IX;
            mem[(uint16_t)(addr+1)] = specialRegs.IX>>8;
        }

        // Load SP,IX - M:2 T:10
        if(op2 == 0xF9){
            specialRegs.SP = specialRegs.IX;
        }


        // Push IX - M:€ T:15
        if( op2 == 0xE5){
            mem[specialRegs.SP-2] = (uint8_t)specialRegs.IX;
            mem[specialRegs.SP-1] = specialRegs.IX >> 8;
            specialRegs.SP -= 2;
        }

        // POP IX - M:4 T:14
        if( op2 == 0xE1 ){
            specialRegs.IX = (mem[specialRegs.SP+1]<<8) + mem[specialRegs.SP];
            specialRegs.SP +=2;
        }

    
    } else 
    
    if ( op == 0xFD ){
        uint8_t op2 = fetchInstruction();

        // Load r,(IY + d) (11 111 101,01rrr110,dddddddd)
        if( (op2 & 11000111) == 01000110 ){
            uint8_t reg = (op2 & 0b0011100) >> 3;
            uint8_t value = fetchInstruction();
            uint16_t addr = specialRegs.IY + (int8_t)regs.D;
            loadRN(reg,mem[addr]);
        } else

        // Load (IY + d),r (11111101, 01110rrr, dddddddd)
        if( (op2 & 0b11111000) == 0b01110000){
            uint8_t reg = (op2 & 0b111);
            int8_t d = fetchInstruction();
            uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
            mem[specialRegs.IY + d] = *regPtr;
        }else
        // Load (IY + d),n (FD,36,d,n)
        if(op2 == 0x36){
            int8_t d = fetchInstruction();
            uint8_t value = fetchInstruction();
            mem[specialRegs.IY + d] = value;
        }else 
        
        // Load IX,nn - M:4 T:14
        if(op2 == 0x21){
            specialRegs.IY = fetch16BitAddress();
        }

        // Load IY,(nn) - M:6 T:20
        if(op2 == 0x2a){
            uint16_t addr = fetch16BitAddress();
            uint8_t lobyte = mem[addr];
            uint8_t hibyte = mem[addr+1];
            specialRegs.IY = (hibyte<<8) + lobyte;
        } else

        // Load (nn),IY - (0xfd,0x22,n,n)- M:6 T:20
        if(op2 == 0x22){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = (uint8_t)specialRegs.IY;
            mem[addr+1] = specialRegs.IY>>8;
        }else 

        // Load SP,IY - M:2 T:10
        if(op2 == 0xF9){
            specialRegs.SP = specialRegs.IY;
        }

        // Push IY - M:4 T:15
        if( op2 == 0xE5){
            mem[specialRegs.SP-2] = (uint8_t)specialRegs.IY;
            mem[specialRegs.SP-1] = specialRegs.IY >> 8;
            specialRegs.SP -= 2;
        } else 

        // POP IY - M:4 T:14
        if( op2 == 0xE1 ){
            specialRegs.IY = (mem[specialRegs.SP+1]<<8) + mem[specialRegs.SP];
            specialRegs.SP +=2;
        }


    }else 

    // load (HL),n (0x36,n)
    if( op == 0x36 ){
        uint8_t value = fetchInstruction();
        mem[regs.value_in_hl()] = value;
    } else

    // Load A,(BC)
    if( op == 0x0A){
        regs.A = mem[regs.value_in_bc()];
    } else

    // Load A,(DE)
    if( op == 0x1A){
        regs.A = mem[regs.value_in_de()];
    } else

    // Load A,(nn) (0x3a,n,n)
    if( op == 0x3A){
        uint16_t addr = fetch16BitAddress();
        regs.A = mem[addr];
    } else

    // Load (BC),A
    if( op == 0x02){
        mem[regs.value_in_bc()] = regs.A;
    } else 

    // Load (DE),A
    if( op == 0x12){
        mem[regs.value_in_de()] = regs.A;
    } else 

    // Load (nn),A
    if( op == 0x32){
        uint16_t addr = fetch16BitAddress();
        mem[addr] = regs.A;
    } else 

    if( op == 0xED){
        uint8_t op2 = fetchInstruction();
        // Load A,I
        if(op2 == 0x57){
            regs.A = specialRegs.I;
            regs.F &= 0b00000001;
            // Bits affected:
            // S: 1 if I-reg is negative. 0 otherwise.
            // TODO: how is I negative? isn't it unsigned?
            // Z: 1 if I is zero. 0 otherwise
            if(specialRegs.I == 0){ regs.F |= FlagBitmaskZero; };
            // H : 0
            // P/V: contents of IFF2
            if(IFF2){ regs.F |= FlagBitmaskPV; };
            // N: 0
            // C: not affected
        } else 

        // Load A,R
        if(op2 == 0x5f){
            regs.A = specialRegs.R;
            regs.F &= 0b00000001;
            // Bits affected:
            // S: 1 if I-reg is negative. 0 otherwise.
            // TODO: how is I negative? isn't it unsigned?
            // Z: 1 if I is zero. 0 otherwise
            if(specialRegs.I == 0){ regs.F |= FlagBitmaskZero; };
            // H : 0
            // P/V: contents of IFF2
            if(IFF2){ regs.F |= FlagBitmaskPV; };
            // N: 0
            // C: not affected
        } else 

        // Load I,A
        if(op2==0x47){
            specialRegs.I = regs.A;
        } else 

        // Load R,A
        if(op2==0x4F){
            specialRegs.R = regs.A;
        } else 

        // Load BC,(nn) - M:6 T:20
        if(op2==0b01001011){
            uint16_t addr = fetch16BitAddress();
            regs.C = mem[addr];
            regs.B = mem[addr+1];
        } else 
        
        // Load DE,(nn) - M:6 T:20
        if(op2==0b01011011){
            uint16_t addr = fetch16BitAddress();
            regs.E = mem[addr];
            regs.D = mem[addr+1];
        } else

        // Load HL,(nn) - M:6 T:20
        if(op2==0b01101011){
            uint16_t addr = fetch16BitAddress();
            regs.L = mem[addr];
            regs.H = mem[addr+1];
        } else 
        
        // Load SP,(nn) - M:6 T:20
        if(op2==0b01111011){
            uint16_t addr = fetch16BitAddress();
            uint8_t lowbyte = mem[addr];
            uint16_t hibyte = mem[addr+1];
            specialRegs.SP = (hibyte<<8) + lowbyte;
        } else 

        // Load (nn),BC - M:6 T:20
        if(op2==0b01000011){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = regs.C;
            mem[(uint16_t)(addr+1)] = regs.B;
        }else 

        // Load (nn),DE - M:6 T:20
        if(op2==0b01010011){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = regs.E;
            mem[(uint16_t)(addr+1)] = regs.D;
        }else 

        // Load (nn),HL - M:6 T:20
        if(op2==0b01100011){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = regs.L;
            mem[(uint16_t)(addr+1)] = regs.H;
        }

        // Load (nn),SP - M:6 T:20
        if(op2==0b01110011){
            uint16_t addr = fetch16BitAddress();
            mem[addr] = (uint8_t)specialRegs.SP;
            mem[(uint16_t)(addr+1)] = (specialRegs.SP>>8);
        }


    } else 

    // Load HL,nn (0b00100001) - M:3 T:10
    if( op == 0x21 ){
        regs.L = fetchInstruction();
        regs.H = fetchInstruction();
    } else 

    if( op == 0x22 ){
        uint16_t addr = fetch16BitAddress();
        mem[addr] = regs.L;
        mem[addr+1] = regs.H;
    } else

    // Load SP,nn (0b00110001) - M:3 T:10
    if( op == 0x31 ){
        specialRegs.SP = fetch16BitAddress();
    } else 

    // Load HL,(nn) - M:5 T:16
    if( op == 0x2A ){
        uint16_t addr = fetch16BitAddress();
        regs.L = mem[addr];
        regs.H = mem[(uint16_t)(addr+1)];
    } else

    // Load SP,HL - M:1 T:6
    if( op == 0xF9){
        specialRegs.SP = regs.value_in_hl();
    } else 

    // Push BC - M:3 T:11
    if( op == 0b11000101){
        mem[specialRegs.SP-2] = regs.C;
        mem[specialRegs.SP-1] = regs.B;
        specialRegs.SP -= 2;
    } else

    // Push DE - M:3 T:11
    if( op == 0b11010101){
        mem[specialRegs.SP-2] = regs.E;
        mem[specialRegs.SP-1] = regs.D;
        specialRegs.SP -= 2;
    } else 

    // Push HL - M:3 T:11
    if( op == 0b11100101){
        mem[specialRegs.SP-2] = regs.L;
        mem[specialRegs.SP-1] = regs.H;
        specialRegs.SP -= 2;
    }

    // Push AF - M:3 T:11
    if( op == 0b11110101){
        mem[specialRegs.SP-2] = regs.F;
        mem[specialRegs.SP-1] = regs.A;
        specialRegs.SP -= 2;
    }

    // Pop BC - M:3 T:10
    if( op == 0b11000001 ){
        regs.C = mem[specialRegs.SP];
        regs.B = mem[specialRegs.SP+1];
        specialRegs.SP +=2;
    }

    // Pop DE - M:3 T:10
    if( op == 0b11010001 ){
        regs.E = mem[specialRegs.SP];
        regs.D = mem[specialRegs.SP+1];
        specialRegs.SP +=2;
    }

    // Pop HL - M:3 T:10
    if( op == 0b11100001 ){
        regs.L = mem[specialRegs.SP];
        regs.H = mem[specialRegs.SP+1];
        specialRegs.SP +=2;
    }

    // Pop AF - M:3 T:10
    if( op == 0b11110001 ){
        regs.F = mem[specialRegs.SP];
        regs.A = mem[specialRegs.SP+1];
        specialRegs.SP +=2;
    }

};


bool CPU::has_parity( uint8_t x ){
    /*
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return (~x) & 1;
    */
    uint8_t p = 1;
    while (x)
    {
        p ^= 1;
        x &= x-1; // at each iteration, we set the least significant 1 to 0
    }
    return p;
}

// *******************************************************
// generic methods
// *******************************************************

void CPU::loadRR( uint8_t dstReg , uint8_t srcReg ) {
        if(srcReg == RegisterCode::HLPtr){
            uint16_t srcMemAddr = regs.value_in_hl(); // Get memory address from HL register pair
            uint8_t data = mem[srcMemAddr]; // Get data from HL location
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = data;
        } if(dstReg == RegisterCode::HLPtr){
            uint16_t dstMemAddr = regs.value_in_hl(); // Get memory address from HL register pair
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcReg;
            mem[dstMemAddr] = *srcRegPtr;
        } else {
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcReg;
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = *srcRegPtr;
        }
    }


void CPU::loadRN( uint8_t dstReg, uint8_t value){
    if(dstReg == RegisterCode::HLPtr){
        uint16_t dstMemAddr = regs.value_in_hl(); // Get memory address from HL register pair
        mem[dstMemAddr] = value;
    } else {
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
        *dstRegPtr = value;
    }
}

void CPU::inc_r(uint8_t &reg){
    reg++;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x80 ); // Set if B was 0x7f before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, 0); // reset
}

void CPU::dec_r(uint8_t &reg){
    reg--;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x7f ); // Set if B was 0x80 before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0b00001111 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, 1); // set
}

// Helper method to get (ix + n)
/*
void::CPU getValuePointedToByIXPlusN(){
    int8_t d = fetchInstruction();
    uint16_t addr = specialRegs.IX + d;
    value = mem->data(addr);
    return value;
}
*/

// *******************************************************
// Instruction methods
// *******************************************************

// nop
// opcode: 00
// Cycles: 4
void CPU::NOP(){
};

// ld bc,nn          
// opcode: 01 n  n
// cycles: 10
void CPU::load_bc_nn(){
    regs.C = fetchInstruction();
    regs.B = fetchInstruction();
};

// ld (bc),a
// Opcode: 02
// Cycles: 7
void CPU::load_bc_a(){
    mem[regs.value_in_bc()] = regs.A;
}

// inc bc
// Opcode: 03
// Cycles: 06
// Flags: -
void CPU::inc_bc(){
    regs.C++;
    if(regs.C==0) regs.B++;
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

// Load B,N
// opcode: 0x06
// cycles: 7
void CPU::load_b_n(){
    regs.B = fetchInstruction();
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

// EX AF AF'
// opcode: 0x08
// cycles: 4
void CPU::ex_af(){
    std::swap(regs.A,auxRegs.A);
    std::swap(regs.F,auxRegs.F);
}

// ADD HL,BC
// opcode: 0x09
// cycles: 11
// flags: H, N, C
void CPU::add_hl_bc(){
    uint32_t hl = regs.value_in_hl();
    uint32_t bc = regs.value_in_bc();
    uint32_t result = hl + bc;
    setFlag(FlagBitmaskC,result & 0x10000); // Set if carry out of bit 15
    // TODO: not sure that the half-carry is 100% correct
    setFlag(FlagBitmaskHalfCarry, (hl & (1<<11)) &&  (result ^ (1<<11)) ); // Set if carry out of bit 11
    setFlag(FlagBitmaskN,0);
    regs.H = result >> 8;
    regs.L = result;
}

// LD A,(BC)
// opcode: 0x0A
// cycles: 7
// flags: -
void CPU::load_a_ptr_bc(){
    regs.A = mem[regs.value_in_bc()];
}

// DEC BC
// opcode: 0x0b
// cycles: 6
void CPU::dec_bc(){
    uint16_t result = regs.value_in_bc();
    result--;
    regs.B = result >> 8;
    regs.C = result;
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

// LD C,N
// opcode: 0x0e
// cycles: 7
void CPU::load_c_n(){
    regs.C = fetchInstruction();
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
}

// DJNZ N
// opcode: 0x10
// cycles: 13/8
// flags: -
//
// B is decremented, and if not zero, the signed value n is added to PC.
// The jump is measured from the start of the instruction opcode.
//
void CPU::djnz_n(){
    regs.B--;
    if(regs.B != 0){
        int8_t jumpOffset = fetchInstruction();
        specialRegs.PC-=2; // measure from the start of this instruction opcode
        specialRegs.PC += jumpOffset;
    }
}

// ld de,nn          
// opcode: 11 n  n
// cycles: 10
void CPU::load_de_nn(){
    regs.E = fetchInstruction();
    regs.D = fetchInstruction();
};

// load (de),a
// opcode: 0x12
// cycles: 7
void CPU::load_ptr_de_a(){
    mem[regs.value_in_de()] = regs.A;
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

// load d,n
// opcode: 0x16
// cycles: 7
void CPU::load_d_n(){
    regs.D = fetchInstruction();
}

// opcode: 0x17
// cycles: 4
void CPU::rla(){
    // rotate left and set carry 
    // Set bit 0 to previous carry.
    uint8_t newcarry = regs.A & 0x80;
    regs.A <<= 1;
    regs.A |= (newcarry>>7);
}

// jr n
// opcode: 0x18
// cycles: 12
void CPU::jr_n(){
    int8_t jumpOffset = fetchInstruction();
    specialRegs.PC -= 2; // start calculation from beginning of this instruction
    specialRegs.PC += jumpOffset;
}
// add hl,de
// opcode: 0x19
// cycleS: 11
// flag: C N H
void CPU::add_hl_de() {
    uint32_t hl = regs.value_in_hl();
    uint32_t de = regs.value_in_de();
    uint32_t result = hl + de;
    setFlag(FlagBitmaskC,result & 0x10000); // Set if carry out of bit 15
    // TODO: not sure that the half-carry is 100% correct
    setFlag(FlagBitmaskHalfCarry, (hl & (1<<11)) &&  (result ^ (1<<11)) ); // Set if carry out of bit 11
    setFlag(FlagBitmaskN,0);
    regs.H = result >> 8;
    regs.L = result;
}

// LD A,(DE)
// opcode: 0x1a
// cycles: 7
// flags: -
void CPU::load_a_ptr_de(){
    regs.A = mem[regs.value_in_de()];
}

// DEC DE
// opcode: 0x1b
// cycles: 6
void CPU::dec_de(){
    uint16_t result = regs.value_in_de();
    result--;
    regs.D = result >> 8;
    regs.E = result;
}

// INC E
// opcode: 0x1c
// cycles 4
void CPU::inc_e(){
    inc_r(regs.E);
}

// DEC E
// opcode: 0x1d
// cycles: 4
void CPU::dec_e(){
    dec_r(regs.E);
}

// LD E,n
// opcode: 0x1e
// cycles: 7
// flags: -
void CPU::ld_e_n(){
    regs.E = fetchInstruction();
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
}

// JR NZ
// opcode: 0x20
// cycles: 12/7
void CPU::jr_nz(){
    int8_t jumpOffset = fetchInstruction();
    if(!(regs.F & FlagBitmaskZero)){ // If not zero
        specialRegs.PC -= 2; // start calculation from beginning of this instruction
        specialRegs.PC += jumpOffset;
    }
}

// LD HL,NN
// opcode: 0x21
// cycles: 10
// flags: -
void CPU::ld_hl_nn(){
    regs.L = fetchInstruction();
    regs.H = fetchInstruction();
}

// LD (NN),HL
// opcode: 0x22
// flags: -
void CPU::ld_ptr_nn_hl(){
    uint16_t addr = fetch16BitAddress();
    mem[addr] = regs.L;
    mem[addr+1] = regs.H;
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

void CPU::ld_h_n(){
    regs.H = fetchInstruction();
}


// DAA
// opcode: 0x27
// cycles: 4
// flags: S Z H PV C
void CPU::daa(){

    bool c_before_daa = (regs.F & FlagBitmaskC);
    bool h_before_daa = (regs.F & FlagBitmaskHalfCarry);
    uint8_t hexValueInUpper = regs.A >> 4;
    uint8_t hexValueInLower = regs.A & 0x0f;

    // Implemented according to the truth-table in the Z80 Technical manual p.141
    // test with:
    //
    // ld a,$15 ; represents 15 (decimal)
    // ld b,$27 ; represents 27 (decimal)
    // add a,b  ; a now contains $3C (not a valid BCD number)
    // daa      ; a now contains $42, representing 42 (decimal) = 15+27
    //
    // or test with:
    // 
    // ld a,$09 ; represents 9 (decimal)
    // inc a    ; a now contains $0a (not a valid BCD number)
    // daa      ; a now contains $10, representing 10 (decimal) = 9+1

    if(regs.F & FlagBitmaskN){ // a SUB instruction was performed previously
        if( c_before_daa == 0 ) {
            if( hexValueInUpper <= 0x09 && h_before_daa == 0 && hexValueInLower <= 0x09){ 
                regs.A += 0x00;
                setFlag(FlagBitmaskC,false);
            } else 
            if( hexValueInUpper <= 0x08 && h_before_daa == 1 && hexValueInLower >= 0x06){ 
                regs.A += 0xFA;
                setFlag(FlagBitmaskC,false);
            }
        }else{
            if( hexValueInUpper >= 0x07 && h_before_daa == 0 && hexValueInLower <= 0x09){ 
                regs.A += 0xA0;
                setFlag(FlagBitmaskC,true);
            } else
            if( hexValueInUpper >= 0x06 && h_before_daa == 1 && hexValueInLower <= 0x06){ 
                regs.A += 0x9A;
                setFlag(FlagBitmaskC,true);
            }
        }
    } else { // an ADD instruction was performed previously
        if( c_before_daa == 0 ) {
            if( hexValueInUpper <= 0x09 && h_before_daa == 0 && hexValueInLower <= 0x09){ 
                regs.A += 0x00;
                setFlag(FlagBitmaskC,false);
            } else 
            if( hexValueInUpper <= 0x08 && h_before_daa == 0 && hexValueInLower >= 0x0a){
                regs.A += 0x06;
                setFlag(FlagBitmaskC,false);
            } else 
            if( hexValueInUpper <= 0x09 && h_before_daa == 1 && hexValueInLower <= 0x03){
                regs.A += 0x06;
                setFlag(FlagBitmaskC,false);
            } else 
            if( hexValueInUpper >= 0x0a && h_before_daa == 0 && hexValueInLower <= 0x09){
                regs.A += 0x60;
                setFlag(FlagBitmaskC,true);
            } else 
            if( hexValueInUpper >= 0x09 && h_before_daa == 0 && hexValueInLower >= 0x0a){
                regs.A += 0x66;
                setFlag(FlagBitmaskC,true);
            } else 
            if ( hexValueInUpper >= 0x0a && h_before_daa == 1 && hexValueInLower <= 0x03){
                regs.A += 0x66;
                setFlag(FlagBitmaskC,true);
            }
        } else {
            if( hexValueInUpper <= 0x02 && h_before_daa == 0 && hexValueInLower <= 0x09){
                regs.A += 0x60;
                setFlag(FlagBitmaskC,true);
            } else 
            if( hexValueInUpper <= 0x02 && h_before_daa == 0 && hexValueInLower >= 0x0a){
                regs.A += 0x66;
                setFlag(FlagBitmaskC,true);
            } else 
            if( hexValueInUpper <= 0x03 && h_before_daa == 1 && hexValueInLower <= 0x03){
                regs.A += 0x66;
                setFlag(FlagBitmaskC,true);
            }
        }
    }

    setFlag( FlagBitmaskSign, regs.A & 0b10000000 );
    setFlag( FlagBitmaskZero, regs.A == 0);
    setFlag( FlagBitmaskPV, has_parity(regs.A) );
    // TODO: tech.manual says "See instruction" for H flag, but it doesn't say anything else?!
    // Parity: if number of 1s id odd p=0, if number is even p=1
}

// jr z,n
// opcode: 0x28
// cycles: 12/7
// flags: -
void CPU::jr_z(){
    int8_t offset = fetchInstruction();
    if((regs.F & FlagBitmaskZero)){ // If zero
        specialRegs.PC -= 2; // start calculation from beginning of this instruction
        specialRegs.PC += offset;
    }
}

// add hl,hl
// opcode: 0x29
// cycles: 11
// flags: C H N
void CPU::add_hl_hl(){
    uint32_t hl = regs.value_in_hl();
    uint32_t result = hl << 1;
    setFlag(FlagBitmaskC,result & 0x10000); // Set if carry out of bit 15
    // TODO: not sure that the half-carry is 100% correct
    setFlag(FlagBitmaskHalfCarry, (hl & (1<<11)) &&  (result ^ (1<<11)) ); // Set if carry out of bit 11
    setFlag(FlagBitmaskN,0);
    regs.H = result >> 8;
    regs.L = result;
}

// ld hl,(nn)
// opcode: 0x2a
// cycles: 16
// flags: -
void CPU::ld_hl_ptr_nn(){
    uint16_t addr = fetch16BitAddress();
    regs.L = mem[addr];
    regs.H = mem[addr+1];
}

// dec hl
// opcode: 0x2b
// cycles: 6
// flags: -
void CPU::dec_hl(){
    uint16_t result = regs.value_in_hl();
    result--;
    regs.D = result >> 8;
    regs.E = result;
}

// inc l
// opcode: 0x2c
// cycles: 4
void CPU::inc_l(){
    inc_r(regs.L);
}

// DEC L
// opcode: 0x2d
// cycles: 4
void CPU::dec_l(){
    dec_r(regs.L);
}

// LD L,n
// opcode: 0x2E
// cycles: 7
void CPU::ld_l_n(){
    regs.L = fetchInstruction();
}

// CPL
// opcode: 0x2f
// cycles: 4
// flags: N H
void CPU::cpl(){
    regs.A = ~regs.A;
    setFlag(FlagBitmaskHalfCarry,true);
    setFlag(FlagBitmaskN,true);
}