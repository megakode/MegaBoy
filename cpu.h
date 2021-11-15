#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#pragma once

class HostMemory {

    public:

        uint8_t& operator[] (int index);

    private:

        uint8_t memory[UINT16_MAX];

};

enum FlagBitmask : uint8_t {
    FlagBitmaskSign = 0b10000000,
    FlagBitmaskZero = 0b01000000,
    FlagBitmaskHalfCarry = 0b00010000,
    FlagBitmaskPV = 0b00000100,
    FlagBitmaskN = 0b00000010,
    FlagBitmaskC = 0b00000001
};

enum RegisterCode : uint8_t {
    B = 0,
    C,
    D,
    E,
    H,
    L,
    HLPtr,
    A
};

class CPU {

    public:
    
    CPU();

    inline void setFlag( FlagBitmask flag, bool value ){
        if(value){
            regs.F |= flag;
        } else {
            regs.F &= ~flag;
        }
    }

    inline bool getFlag( FlagBitmask flag ){
        return regs.F & flag;
    }

    bool has_parity( uint8_t x );

    // generic instructions
    void inc_r( uint8_t &reg );
    void dec_r( uint8_t &reg );

    // instructions
    void NOP(); // 0x00
    void load_bc_nn();
    void load_bc_a();
    void inc_bc();
    void inc_b();
    void dec_b();
    void load_b_n();
    void rlca();
    void ex_af();
    void add_hl_bc();
    void load_a_ptr_bc();
    void dec_bc();
    void inc_c();
    void dec_c();
    void load_c_n();
    void rrca();

    void djnz_n(); // 0x10
    void load_de_nn();
    void load_ptr_de_a();
    void inc_de();
    void inc_d();
    void dec_d();
    void load_d_n();
    void rla();
    void jr_n();
    void add_hl_de();
    void load_a_ptr_de();
    void dec_de();
    void inc_e();
    void dec_e();
    void ld_e_n();
    void rra();

    void jr_nz(); // 0x20
    void ld_hl_nn();
    void ld_ptr_nn_hl();
    void inc_hl();
    void inc_h();
    void dec_h();
    void ld_h_n();
    void daa();
    void jr_z();
    void add_hl_hl();
    void ld_hl_ptr_nn();
    void dec_hl();
    void inc_l();
    void dec_l();
    void ld_l_n();
    void cpl();

    void jr_nc(); // 0x30
    void ld_sp_nn();
    void ld_ptr_nn_a();
    void inc_sp();
    void inc_ptr_hl();
    void dec_ptr_hl();
    void ld_ptr_hl_n();
    void scf();
    void jr_c();
    void add_hl_sp();
    void ld_a_ptr_nn();
    void dec_sp();
    void inc_a();
    void dec_a();
    void ld_a_n();
    void ccf();

    struct Instruction {
        std::string name;
        void (CPU::*code)(void) = nullptr;
    };

    std::vector<Instruction> opCodeLookup;

    struct Pins {
        uint16_t addr;  // Address bus
        uint8_t data;   // Data bus
    } pins;

    public:

    HostMemory mem;

    // Interrupt enable Flip Flop #1 and #2 (see page 323 in Zilog Z80 Technical manual)
    bool IFF = 0;
    bool IFF2 = 0;

    struct GeneralRegisters {

        // Flags:
        //
        // [S | Z | . | H | . | P/V | N | C ]
        //        
        // S - Sign flag
        // Z - Zero flag
        // . - unused
        // H - Half-carry
        // . - unused
        // P/V - Parity/overflow
        // N Add/Subtract flag
        // C - Carry 

        uint8_t B = 0,C = 0;
        uint8_t D = 0,E = 0;
        uint8_t H = 0,L = 0;
        uint8_t padding; // So register indexes in opcodes matches indexing into this struct
        uint8_t A = 0; // Accumulator
        uint8_t F = 0; 
        
        // Returns the Z80 address contained in the HL register pair
        uint16_t value_in_hl() {
            return static_cast<uint16_t>((H<<8) + L);
        };

        // Returns the Z80 address contained in the DE register pair
        uint16_t value_in_de() {
            return static_cast<uint16_t>((D<<8) + E);
        };

        // Returns the Z80 address contained in the BC register pair
        uint16_t value_in_bc() {
            return static_cast<uint16_t>((B<<8) + C);
        };

    };

    struct SpecialRegisters {
        uint8_t I = 0;      // Interrupt Page Address Register
        uint8_t R = 0;      // Memory Refresh Register
        uint16_t IX = 0;    // Index Register X
        uint16_t IY = 0;    // Index Register Y
        uint16_t SP = 0;    // Stack Pointer
        uint16_t PC = 0;    // Program Counter
    } specialRegs;

    GeneralRegisters regs,auxRegs;
    uint8_t regArray[9];

    // Fetch next instruction byte from memory and increase Program Counter by +1 (PC)
    inline uint8_t fetch8BitValue(){
        return mem[specialRegs.PC++];
    };

    // Fetch two instruction bytes from memory as a 16 bit address and increase Program Counter by +2 (PC)
    inline uint16_t fetch16BitValue(){
        uint8_t lowbyte = fetch8BitValue();
        uint16_t hibyte = fetch8BitValue();
        return (hibyte<<8) + lowbyte;
    };

    void loadRR( uint8_t dstReg , uint8_t srcReg );
    void loadRN( uint8_t dstReg, uint8_t value);

    void step();

};