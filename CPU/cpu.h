
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <functional>

#include <string>
#include <vector>
#include <iostream>

#include "../HostMemory.h"

#pragma once

enum FlagBitmask : uint8_t
{
    FlagBitmaskZero = 0b10000000,
    FlagBitmaskN = 0b01000000,
    FlagBitmaskHalfCarry = 0b00100000,
    FlagBitmaskC = 0b00010000
};

enum RegisterCode : uint8_t
{
    B = 0,
    C,
    D,
    E,
    H,
    L,
    HLPtr,
    A
};

struct DebugLogEntry
{
    uint16_t PC = 0;
    uint8_t opcodes[3] = {0, 0, 0};
};

class CPU
{

public:
    explicit CPU(HostMemory &mem);

    CPU() = delete;

    struct Instruction
    {
        /// Number of CPU cycles the instruction takes to execute
        uint8_t cycles = 0;
        /// pointer to the function executing the instruction
        void (CPU::*code)() = nullptr;
    };

    std::vector<Instruction> instructions;

    std::vector<DebugLogEntry> debug_log_entries;

    HostMemory &mem;

    struct GeneralRegisters
    {

        union
        {
            uint16_t BC;
            struct
            {
                uint8_t C, B;
            };
        };

        union
        {
            uint16_t DE;
            struct
            {
                uint8_t E, D;
            };
        };

        union
        {
            uint16_t HL;
            struct
            {
                uint8_t L, H;
            };
        };

        union
        {
            uint16_t AF;
            struct
            {
                uint8_t F, A;
            };
        };

        // uint8_t I;      // Interrupt Page Address Register
        uint8_t R; // Memory Refresh Register

        union
        {
            uint16_t SP; // Stack Pointer
            struct
            {
                uint8_t SPL;
                uint8_t SPH;
            };
        };
        uint16_t PC; // Program Counter
    };

    GeneralRegisters regs;

    bool interrupt_master_enabled = true;
    bool ime_just_enabled = false;
    bool do_halt_bug = false;
    bool just_halted = false;

    /// The address of the opcode currently being decoded.
    /// Contrary to the program counter (PC) which increases throughout decoding of a multi-byte instruction,
    /// this address stays the same until a new CPU step is executed.
    uint16_t current_pc;
    /// The first opcode of the instruction currently being executed
    uint8_t current_opcode;
    /// Current number of cycles spent by the currently executing opcode. Each instruction adds cycles to this variable during execution,
    /// and the step method waits this number of cycles after a step, and resets it to 0 afterwards.
    uint16_t additional_cycles_spent;

    bool is_halted = false;

    // *************************************************************************************
    // Fetching instruction bytes
    // *************************************************************************************

    /// Fetch, decode and execute an instruction
    /// - returns: the number of CPU cycles spent. The implementor should wait this amount of cycles before calling `Step` again.
    uint8_t step();

    /// Fetch next instruction byte from memory and increase Program Counter by +1 (PC)
    inline uint8_t fetch8BitValue()
    {
        return mem.Read(regs.PC++);
    };

    /// Fetch two instruction bytes from memory as a 16 bit address and increase Program Counter by +2 (PC)
    inline uint16_t fetch16BitValue()
    {
        uint8_t lowbyte = fetch8BitValue();
        uint16_t hibyte = fetch8BitValue();
        return (hibyte << 8) + lowbyte;
    };

    // *********************************************************************************
    // Helper functions
    // *********************************************************************************

    /// Write to a register based on a register code.
    inline void write_to_register(uint8_t regCode, uint8_t value)
    {
        regCode &= 0b00000111; // remove excess bits just in case

        switch (regCode)
        {
        case RegisterCode::HLPtr:
            mem.Write(regs.HL, value);
            break;
        case RegisterCode::A:
            regs.A = value;
            break;
        case RegisterCode::B:
            regs.B = value;
            break;
        case RegisterCode::C:
            regs.C = value;
            break;
        case RegisterCode::D:
            regs.D = value;
            break;
        case RegisterCode::E:
            regs.E = value;
            break;
        case RegisterCode::L:
            regs.L = value;
            break;
        case RegisterCode::H:
            regs.H = value;
            break;
        default:
            regs.A = value; // just to silence the compiler warning
        }
    }

    /// Returns the value a register based on a 3bit register code, which is encoded in all opcodes which deals with registers.
    /// \param regCode Register code
    /// \return Reference to register
    inline uint8_t read_from_register(uint8_t regCode) const
    {
        regCode &= 0b00000111; // remove excess bits just in case

        switch (regCode)
        {
        case RegisterCode::HLPtr:
            return mem.Read(regs.HL);
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

    static inline std::string reg_name_from_regcode(uint8_t regcode)
    {
        switch (regcode)
        {
        case RegisterCode::A:
            return "A";
            break;
        case RegisterCode::B:
            return "B";
            break;
        case RegisterCode::C:
            return "C";
            break;
        case RegisterCode::D:
            return "D";
            break;
        case RegisterCode::E:
            return "E";
            break;
        case RegisterCode::H:
            return "H";
            break;
        case RegisterCode::L:
            return "L";
            break;
        case RegisterCode::HLPtr:
            return "(HL)";
            break;
        default:
            return "Regcode not found!";
            break;
        }
    }

    /// Checks whether a condition is true, using a 3 bit condition code encoded in all jp and call instruction opcodes.
    /// Each condition code reflects a flag and a state. E.g. Carry, non-carry, zero, non-zero, etc.
    /// \param conditionCode The 3bit condition code from 0-7
    /// \return Condition state.
    [[nodiscard]] inline bool is_condition_true(uint8_t conditionCode) const
    {
        switch (conditionCode)
        {
        case 0:
            return !(regs.F & FlagBitmaskZero); // Non zero
        case 1:
            return (regs.F & FlagBitmaskZero); // zero
        case 2:
            return !(regs.F & FlagBitmaskC); // non carry
        case 3:
            return (regs.F & FlagBitmaskC); // carry
        default:
            return false;
        }
    }

    /// Debug method to get the name of a condition code, encoded in jump instruction
    static inline std::string name_from_condition(uint8_t conditionCode)
    {
        switch (conditionCode)
        {
        case 0:
            return "NZ"; // Non zero
        case 1:
            return "Z"; // zero
        case 2:
            return "NC"; // non carry
        case 3:
            return "C"; // carry
        default:
            return "Unknown condition code?!";
        }
    }

    static inline bool has_parity(uint8_t x)
    {
        uint8_t p = 1;
        while (x)
        {
            p ^= 1;
            x &= x - 1; // at each iteration, we set the least significant 1 to 0
        }
        return p;
    }

    inline void setFlag(FlagBitmask flag, bool value)
    {
        if (value)
        {
            regs.F |= flag;
        }
        else
        {
            regs.F &= ~flag;
        }
    }

    inline bool getFlag(FlagBitmask flag) const
    {
        return regs.F & flag;
    }

    void add(uint8_t srcValue, bool carry = false);
    void add16(uint16_t &regPair, uint16_t value_to_add);
    void sub(uint8_t srcValue, bool carry = false, bool onlySetFlagsForComparison = false);

    void and_a_with_value(uint8_t value);
    void or_a_with_value(uint8_t value);
    void xor_a_with_value(uint8_t value);

    void INC_r(uint8_t &reg);
    void DEC_r(uint8_t &reg);

    uint8_t do_bit_instruction(uint8_t op2, uint8_t &reg);

    void AddDebugLog();

    void set_AND_operation_flags();
    void set_INC_operation_flags(uint8_t result);
    void set_DEC_operation_flags(uint8_t result);

public:
    void enable_interrupts();
    void disable_interrupts();

    // instructions
    void NOP(); // 0x00

    void LD_SP_nn();
    void LD_pnn_A();
    void LD_pHL_n();
    void LD_BC_nn();
    void LD_pBC_A();
    void LD_A_pBC();
    void LD_r_r();
    void LD_DE_nn();
    void LD_pDE_A();
    void ld_sp_hl();
    void LD_HL_nn();
    void LD_A_pDE();
    void LD_r_n();
    void LD_pnn_SP();
    // void LD_R_A();
    void LDI_pHL_A();
    void LDI_A_pHL();
    void LDD_pHL_A();
    void LDD_A_pHL();
    void LD_ff00n_A();
    void LD_ff00C_A();
    void LD_A_ff00n();
    void LD_A_ff00C();
    void LD_HL_SPs8();
    void LD_A_pnnnn();

    void INC_A();
    void DEC_A();
    void INC_B();
    void DEC_B();
    void INC_C();
    void DEC_C();
    void INC_D();
    void DEC_D();
    void INC_E();
    void DEC_E();
    void INC_H();
    void DEC_H();
    void INC_L();
    void DEC_L();
    void INC_BC();
    void DEC_BC();
    void INC_DE();
    void DEC_DE();
    void INC_HL();
    void DEC_HL();
    void INC_pHL();
    void DEC_pHL();
    void INC_SP();
    void DEC_SP();

    // Bit instructions

    void decode_bit_instruction();
    void RLD();
    void SWAP_r(uint8_t regCode);
    void RLCA();
    void RRCA();
    void RLA();
    void RRA();

    // Jump instructions

    void JR_z();
    void JR_nc(); // 0x30
    void JR_c();
    void JR_n();
    void JR_nz(); // 0x20
    void JP_cc_nn();
    void JP_nn();
    void JP_pHL();
    void CALL();
    void CALL_cc_nn();
    void RET_cc();
    void RETI();
    void RST();
    void RET();

    // Arithmetic instructions

    void ADD_HL_DE();
    void ADD_HL_BC();
    void ADD_HL_HL();
    void ADD_HL_SP();
    void ADD_A_r();
    void ADD_A_n();
    void ADC_A_r();
    void ADD_SP_s8();
    void ADC_A_n();
    void SUB_n();
    void SBC_n();
    void SUB_r();
    void SBC_r();

    void AND_n();
    void AND_r();
    void XOR_r();
    void XOR_n();
    void OR_n();
    void OR_r();
    void CP_r();
    void CP_n();

    void push_af();
    void push_bc();
    void push_de();
    void push_hl();
    void push_pc();

    void pop_qq();
    void pop16(uint16_t &regPair);

    // General instructions

    void NEG();
    void scf();
    void ccf();
    void cpl();
    void daa();
    void halt();
    void reset();
    void STOP();

    void invalid_opcode();

    uint8_t do_bit_instruction(uint8_t op2);
};