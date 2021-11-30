
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <functional>

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


/// Interrupt mode - see Z80 technical reference manual p.153
enum class InterruptMode {
    IM_0,
    IM_1,
    IM_2
};

class CPU {

    public:
    
    CPU();

    struct Instruction {
        std::string name;
         void (CPU::*code)(void) = nullptr;
        //std::function<void()> code;
    };

    std::vector<Instruction> instructions;
    std::vector<std::function<void()>> extended_instructions;
    std::vector<std::function<void()>> bit_instructions;
    std::vector<std::function<void()>> ix_instructions;
    std::vector<std::function<void()>> iy_instructions;

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
        union {
            uint16_t BC;
            struct { uint8_t C,B; };
        };

        union {
            uint16_t DE;
            struct { uint8_t E,D; };
        };

        union {
            uint16_t HL;
            struct { uint8_t L,H; };
        };

        uint8_t padding; // So register indexes in opcodes matches indexing into this struct

        union {
            uint16_t AF;
            struct { uint8_t F,A; };
        };
    };

    struct SpecialRegisters {
        uint8_t I;      // Interrupt Page Address Register
        uint8_t R;      // Memory Refresh Register
        union
        {
            uint16_t IX;    // Index Register X
            struct {
                uint8_t IXL; uint8_t IXH;
            };
        };
        union
        {
            uint16_t IY;    // Index Register Y
            struct { uint8_t IYL; uint8_t IYH; };
        };
        union
        {
            uint16_t SP;    // Stack Pointer
            struct { uint8_t SPL; uint8_t SPH; };
        };
        uint16_t PC;    // Program Counter
    };

    SpecialRegisters specialRegs;
    GeneralRegisters regs;
    GeneralRegisters auxRegs;

    InterruptMode interrupt_mode = InterruptMode::IM_0;

    uint8_t currentOpcode;

    /// Fetch next instruction byte from memory and increase Program Counter by +1 (PC)
    inline uint8_t fetch8BitValue(){
        return mem[specialRegs.PC++];
    };

    /// Fetch two instruction bytes from memory as a 16 bit address and increase Program Counter by +2 (PC)
    inline uint16_t fetch16BitValue(){
        uint8_t lowbyte = fetch8BitValue();
        uint16_t hibyte = fetch8BitValue();
        return (hibyte<<8) + lowbyte;
    };

    /// Fetch a byte and return a (IX+n) pointer using that.
    inline uint8_t& fetch_pIXn(){
        auto offset = static_cast<int8_t>(fetch8BitValue());
        return mem[specialRegs.IX+offset];
    }

    /// Fetch a byte and return a (IY+n) pointer using that.
    inline uint8_t& fetch_pIYn(){
        auto offset = static_cast<int8_t>(fetch8BitValue());
        return mem[specialRegs.IY+offset];
    }


    // returns the value contained in the register represented by the regCode
    inline uint8_t value_from_regcode( uint8_t regCode ){

        uint8_t regValue;

        if(regCode == RegisterCode::HLPtr){
            regValue = mem[regs.HL];
        } else {
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + regCode;
            regValue = *srcRegPtr;
        }

        return regValue;
    }

    /// Returns a reference to a register based on a 3bit register code, which is encoded in all opcodes which deals with registers.
    /// \param regCode Register code
    /// \return Reference to register
    inline uint8_t& reg_from_regcode( uint8_t regCode , bool force_ix_ptr = false, bool force_iy_ptr = false )
    {
        regCode &= 0b00000111; // remove excess bits just in case

        if(regCode == RegisterCode::HLPtr){
            return mem[regs.HL];
        } else {
            return ((reinterpret_cast<uint8_t*>(&regs))[regCode]);
        }
    }

    /// Checks whether a condition is true, using a 3 bit condition code encoded in all jp and call instruction opcodes.
    /// Each condition code reflects a flag and a state. E.g. Carry, non-carry, zero, non-zero, etc.
    /// \param conditionCode The 3bit condition code from 0-7
    /// \return Condition state.
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

    void set_interrupt_mode( InterruptMode mode ) {
        interrupt_mode = mode;
    }

    void step();

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

    void add( uint8_t srcValue, bool carry = false);
    void add16( uint16_t &regPair, uint16_t value_to_add, bool carry = false );
    void sub( uint8_t srcValue, bool carry = false, bool onlySetFlagsForComparison = false );
    void sub16( uint16_t& regPair, uint16_t value_to_sub , bool carry = false);

    void or_a_with_value(uint8_t value);
    void xor_a_with_value(uint8_t value);
    void in( uint8_t& dstReg , uint8_t portNumber, bool only_set_flags = false);

    void decode_extended_instruction();
    void decode_iy_instruction();
    void do_bit_instruction(uint8_t op2, uint8_t &reg, uint8_t &copyResultToReg);

    void decode_ix_bit_instruction();

    void decode_iy_bit_instruction();

    uint8_t input_from_port(uint8_t port);
    void output_to_port(uint8_t port, uint8_t value);


public:

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

    void decode_bit_instruction();
    void decode_ix_instruction();

    void call();
    void adc_a_n();
    void out_n_a();
    void sub_n();
    void exx();
    void in_a_n();
    void sbc_n();
    void EX_pSP_HL();
    void and_n();
    void jp_ptr_hl();
    void ex_de_hl();
    void xor_n();
    void enable_interrupts();
    void disable_interrupts();
    void or_n();
    void ld_sp_hl();
    void cp_n();
    void out(uint8_t dstPort, uint8_t value);

    void sbc_hl_nn(uint16_t value);

    void ld_ptr_nn_n(uint16_t location, uint8_t value);

    void neg();

    void retn();


    void adc_hl_nn(uint16_t value);

    void reti();

    void ld_ptr_nn_nn(uint16_t location, uint16_t value);

    void rrd();

    void rld();

    void invalid_opcode();

    void ldi( bool decrease = false, bool repeat = false );
    void ldir();
    void ldd();
    void lddr();


    void cpi( bool decrease = false, bool repeat = false );
    void cpir();
    void cpd();
    void cpdr();

    void ini( bool decrease = false, bool repeat = false );
    void inir();
    void ind();
    void indr();

    void outi(bool decrease = false, bool repeat = false );
    void otir();
    void outd();
    void otdr();


    void ld_rr_ptr_nn(uint16_t &regPair, uint16_t location);

    void sub16(uint16_t &regPair, uint16_t value_to_sub);

    void ld_a_r();
    void ld_a_i();

    void ld_ix_nn();

    void ld_iy_nn();

    void ld_ix_ptr_nn();

    void ld_iy_ptr_nn();

    void ld_ptr_nn_ix();

    void inc_ix();

    void dec_ix();

    void inc_iy();

    void dec_iy();

    void ld_ixh_n(uint8_t value);
    void ld_ixl_n(uint8_t value);

    void ld_iyh_n(uint8_t value);
    void ld_iyl_n(uint8_t value);

    void INC_pIXn();

    void dec_ptr_ix_n();

    void ld_ptr_ix_n_n();

    void LD_pIXn_r(uint8_t &reg);

    void ld_r_r(uint8_t &dstReg, uint8_t value);

    void LD_r_pIXn(uint8_t &dst_reg);

    void LD_r_pIYn(uint8_t &dst_reg);

    void set_AND_operation_flags();

    void and_a_with_value(uint8_t value);

    void cp_a_with_value(uint8_t value);

    void pop16(uint16_t &regPair);

    void push_ix();

    void push_iy();

    void jp_IY();

    void jp_IX();

    void ld_rr_rr(uint16_t &dstReg, uint16_t &value);
};