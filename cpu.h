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

    void ld_r_r();

    void halt();

    void add_a_r();
    void add_a_n();
    void adc_a_r();
    void sub_r();
    void sbc_r();


    void and_r();

    void xor_r();
    void or_r();
    void cp_r();

    void ret_cc();

    void pop_qq();

    void jp_cc_nn();
    void jp_nn();

    void call_cc_nn();

    void push_bc();
    void push_de();
    void push_hl();
    void push_af();

    void rst();
    void ret();

    void bit_instruction_group();
    void ix_instruction_group();

    void call();
    void adc_a_n();
    void out_n_a();
    void sub_n();
    void exx();
    void in_a_n();
    void sbc_n();
    void ex_ptr_sp_hl();
    void and_n();
    void jp_ptr_hl();
    void ex_de_hl();
    void xor_n();
    void enable_interrupts();
    void disable_interrupts();
    void or_n();
    void ld_sp_hl();
    void cp_n();



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

    uint8_t currentOpcode;

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

    // returns the value contained in the register represented by the regCode
    inline uint8_t value_from_regcode( uint8_t regCode ){

        uint8_t regValue;

        if(regCode == RegisterCode::HLPtr){
            regValue = mem[regs.value_in_hl()];
        } else {
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + regCode;
            regValue = *srcRegPtr;
        }

        return regValue;
    }

    inline uint8_t& reg_from_regcode( uint8_t regCode )
    {
        regCode &= 0b00000111; // remove excess bits just in case

        if(regCode == RegisterCode::HLPtr){
            return mem[regs.value_in_hl()];
        } else {
            return ((reinterpret_cast<uint8_t*>(&regs))[regCode]);
        }
    }

    inline bool is_condition_true( uint8_t conditionCode ){
        switch (conditionCode)
        {
            case 0: return !(regs.F & FlagBitmaskZero); // Non zero
            case 1: return (regs.F & FlagBitmaskZero); // zero
            case 2: return !(regs.F & FlagBitmaskC); // non carry
            case 3: return (regs.F & FlagBitmaskC); // carry
            case 4: return !(regs.F & FlagBitmaskPV); // parity odd
            case 5: return (regs.F & FlagBitmaskPV); // parity even
            case 6: return !(regs.F & FlagBitmaskSign); // sign positive
            case 7: return (regs.F & FlagBitmaskSign); // sign negative
            default: return false;
        }
    }



    void step();



private:

    void add( uint8_t srcValue, bool carry );
    void sub(uint8_t srcValue, bool carry, bool onlySetFlagsForComparison);
    void or_a_with_value(uint8_t value);
    void xor_a_with_value(uint8_t value);

    void extended_instruction_group();
    void iy_instruction_group();


};