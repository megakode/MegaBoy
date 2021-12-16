
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <functional>

#include <string>
#include <vector>
#include <iostream>
#include <format>

#include "../HostMemory.h"

#pragma once

enum FlagBitmask : uint8_t {
    FlagBitmaskZero = 0b10000000,
    FlagBitmaskN = 0b01000010,
    FlagBitmaskHalfCarry = 0b00100000,
    FlagBitmaskC = 0b00010000
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

struct DebugLogEntry {
    uint16_t PC = 0;
    uint8_t opcodes[4] = {0,0,0,0};
    std::string text;
};

class CPU {

    public:
    
    explicit CPU(HostMemory& mem);

    CPU() = delete;

    struct Instruction {
        /// Number of CPU cycles the instruction takes to execute
        uint8_t cycles = 0;
        /// pointer to the function executing the instruction
        void (CPU::*code)() = nullptr;
    };

    std::vector<Instruction> instructions;
    std::vector<std::function<void()>> extended_instructions;

    std::vector<DebugLogEntry> debug_log_entries;

    HostMemory& mem;

    struct GeneralRegisters {

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

        union {
            uint16_t AF;
            struct { uint8_t F,A; };
        };

        uint8_t I;      // Interrupt Page Address Register
        uint8_t R;      // Memory Refresh Register

        union
        {
            uint16_t SP;    // Stack Pointer
            struct { uint8_t SPL; uint8_t SPH; };
        };
        uint16_t PC;    // Program Counter
    };

    GeneralRegisters regs;

    bool interrupt_master_enabled = true;

    /// The address of the opcode currently being decoded.
    /// Contrary to the program counter (PC) which increases throughout decoding of a multi-byte instruction,
    /// this address stays the same until a new CPU step is executed.
    uint16_t current_pc;
    /// The first opcode of the instruction currently being executed
    uint8_t current_opcode;
    /// Current number of cycles spent by the currently executing opcode. Each instruction adds cycles to this variable during execution,
    /// and the step method waits this number of cycles after a step, and resets it to 0 afterwards.
    uint16_t additional_cycles_spent;

    // *************************************************************************************
    // Fetching instruction bytes
    // *************************************************************************************

    /// Fetch, decode and execute an instruction
    /// - returns: the number of CPU cycles spent. The implementor should wait this amount of cycles before calling `Step` again.
    uint8_t step();

    /// Fetch next instruction byte from memory and increase Program Counter by +1 (PC)
    inline uint8_t fetch8BitValue(){
        return mem[regs.PC++];
    };

    /// Fetch two instruction bytes from memory as a 16 bit address and increase Program Counter by +2 (PC)
    inline uint16_t fetch16BitValue(){
        uint8_t lowbyte = fetch8BitValue();
        uint16_t hibyte = fetch8BitValue();
        return (hibyte<<8) + lowbyte;
    };

    // *********************************************************************************
    // Helper functions
    // *********************************************************************************

    /// Returns a reference to a register based on a 3bit register code, which is encoded in all opcodes which deals with registers.
    /// \param regCode Register code
    /// \return Reference to register
    inline uint8_t& reg_from_regcode( uint8_t regCode , bool force_ix_ptr = false, bool force_iy_ptr = false )
    {
        regCode &= 0b00000111; // remove excess bits just in case

        switch (regCode) {
            case RegisterCode::HLPtr:
                return mem[regs.HL];
            case RegisterCode::A:
                return regs.A;
            case RegisterCode::B:
                return regs.B;
            case RegisterCode::C:
                return regs.C;
            case RegisterCode::D:
                return regs.D;
            case RegisterCode::E:
                return regs.E;
            case RegisterCode::L:
                return regs.L;
            case RegisterCode::H:
                return regs.H;
            default:
                return regs.A; // just to silence the compiler warning
        }
    }

    static inline std::string reg_name_from_regcode( uint8_t regcode )
    {
        switch (regcode) {
            case RegisterCode::A: return "A"; break;
            case RegisterCode::B: return "B"; break;
            case RegisterCode::C: return "C"; break;
            case RegisterCode::D: return "D"; break;
            case RegisterCode::E: return "E"; break;
            case RegisterCode::H: return "H"; break;
            case RegisterCode::L: return "L"; break;
            case RegisterCode::HLPtr: return "(HL)"; break;
            default: return "Regcode not found!"; break;
        }
    }

    /// Checks whether a condition is true, using a 3 bit condition code encoded in all jp and call instruction opcodes.
    /// Each condition code reflects a flag and a state. E.g. Carry, non-carry, zero, non-zero, etc.
    /// \param conditionCode The 3bit condition code from 0-7
    /// \return Condition state.
    [[nodiscard]] inline bool is_condition_true( uint8_t conditionCode ) const{
        switch (conditionCode)
        {
            case 0: return !(regs.F & FlagBitmaskZero); // Non zero
            case 1: return (regs.F & FlagBitmaskZero); // zero
            case 2: return !(regs.F & FlagBitmaskC); // non carry
            case 3: return (regs.F & FlagBitmaskC); // carry
            default: return false;
        }
    }

    /// Debug method to get the name of a condition code, encoded in jump instruction
    static inline std::string name_from_condition( uint8_t conditionCode )
    {
        switch (conditionCode)
        {
            case 0: return "NZ"; // Non zero
            case 1: return "Z"; // zero
            case 2: return "NC"; // non carry
            case 3: return "C"; // carry
            default: return "Unknown condition code?!";
        }
    }

    static inline bool has_parity( uint8_t x )
    {
        uint8_t p = 1;
        while (x)
        {
            p ^= 1;
            x &= x-1; // at each iteration, we set the least significant 1 to 0
        }
        return p;
    }

    inline void setFlag( FlagBitmask flag, bool value ){
        if(value){
            regs.F |= flag;
        } else {
            regs.F &= ~flag;
        }
    }

    inline bool getFlag( FlagBitmask flag ) const{
        return regs.F & flag;
    }

    void add( uint8_t srcValue, bool carry = false);
    void add16( uint16_t &regPair, uint16_t value_to_add, bool carry = false );
    void sub( uint8_t srcValue, bool carry = false, bool onlySetFlagsForComparison = false );
    void sub16( uint16_t& regPair, uint16_t value_to_sub , bool carry = false);

    void or_a_with_value(uint8_t value);
    void xor_a_with_value(uint8_t value);

    uint8_t do_bit_instruction( uint8_t op2, uint8_t& reg );

    void AddDebugLog(const std::string &text);

    void set_AND_operation_flags();
    void set_INC_operation_flags(uint8_t result);
    void set_DEC_operation_flags(uint8_t result);

public:

    // generic instructions
    void INC_r(uint8_t &reg );
    void DEC_r(uint8_t &reg );

    void enable_interrupts();
    void disable_interrupts();

    // instructions
    void NOP(); // 0x00

    void LD_pnn_rr(uint16_t location, uint16_t value);
    void LD_SP_nn();
    void LD_pnn_A();
    void LD_pHL_n();
    void LD_BC_nn();
    void LD_pBC_A();
    void LD_A_pBC();
    void LD_r_r();
    void LD_DE_nn();
    void LD_pDE_A();

    void rlca();

    void add_hl_bc();


    void rrca();

    void rla();
    void jr_n();
    void ADD_HL_DE();
    void LD_A_pDE();


    void rra();

    void jr_nz(); // 0x20
    void LD_HL_nn();

    void INC_A();
    void DEC_A();
    void INC_B();
    void DEC_B();
    void inc_c();
    void dec_c();
    void inc_d();
    void dec_d();
    void INC_E();
    void DEC_E();
    void INC_H();
    void DEC_H();
    void INC_L();
    void DEC_L();
    void INC_BC();
    void dec_bc();
    void INC_DE();
    void DEC_DE();
    void INC_HL();
    void DEC_HL();
    void INC_pHL();
    void DEC_pHL();
    void INC_SP();
    void DEC_SP();

    void daa();

    void cpl();

    void jr_z();
    void jr_nc(); // 0x30
    void jr_c();

    void scf();

    void ccf();

    void halt();

    void ADD_HL_HL();
    void add_hl_sp();
    void add_a_r();
    void add_a_n();
    void adc_a_r();
    void sub_r();
    void sbc_r();


    void and_r();

    void xor_r();
    void or_r();
    void CP_r();

    void RET_cc();

    void pop_qq();

    void jp_cc_nn();
    void jp_nn();

    void call_cc_nn();

    void push_bc();
    void push_de();
    void push_hl();
    void push_af();

    void rst();
    void RET();

    void decode_bit_instruction();
    void call();
    void adc_a_n();

    void sub_n();

    void sbc_n();

    void and_n();
    void jp_ptr_hl();

    void xor_n();
    void or_n();
    void ld_sp_hl();
    void CP_n();

    void NEG();

    void RETI();

    void RLD();

    void invalid_opcode();

    void and_a_with_value(uint8_t value);


    void pop16(uint16_t &regPair);

    void reset();

    void LD_r_n();
    void LD_pnn_SP();
    void LD_R_A();

    // Gameboy instructions

    void STOP();
    void LDI_pHL_A();
    void LDI_A_pHL();
    void LDD_pHL_A();
    void LDD_A_pHL();
    void LD_ff00n_A();
    void LD_ff00C_A();
    void LD_A_ff00n();
    void LD_A_ff00C();
    void ADD_SP_s8();
    void LD_HL_SPs8();
    void LD_A_pnn();
    void SWAP_r(uint8_t regCode);

    void push_pc();
};