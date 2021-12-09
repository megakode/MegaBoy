#include <cstdint>
#include <vector>
#include <iostream>

#include "cpu.h"

#include "bit_instructions.h"
#include "load_instructions.h"
#include "jump_instructions.h"
#include "arithmetic_instructions.h"
#include "general_instructions.h"
#include "gameboy_instructions.h"

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

    instructions = {
        {"NOP",&CPU::NOP},                  // 0x00
        {"LD BC,NN", &CPU::LD_BC_nn},      // 0x01
        {"LD (BC),A", &CPU::LD_pBC_A},        // 0x02
        {"INC BC",&CPU::inc_bc},            // 0x03
        {"INC B", &CPU::INC_B},              // 0x04
        {"DEC B", &CPU::DEC_B},              // 0x05
        {"LD B,N", &CPU::LD_r_n},          // 0x06
        {"RLCA",&CPU::rlca},                // 0x07
        {"EXAF",&CPU::LD_pnn_SP},               // 0x08
        {"ADD HL,BC",&CPU::add_hl_bc},      // 0x09
        {"LD A,(BC)", &CPU::LD_A_pBC},  // 0x0a
        {"DEC BC",&CPU::dec_bc},            // 0x0b
        {"INC C",&CPU::inc_c},              // 0x0c
        {"DEC C",&CPU::dec_c},              // 0x0d
        {"LD C,N",&CPU::LD_r_n},            // 0x0e
        {"RRCA",&CPU::rrca},                // 0x0f

        {"DJNZ N",&CPU::STOP},            // 0x10
        {"LD DE,NN", &CPU::LD_DE_nn},      // 0x11
        {"LD (DE),A", &CPU::LD_pDE_A},  // 0x12
        {"INC DE", &CPU::INC_DE},            // 0x13
        {"INC D",&CPU::inc_d},              // 0x14
        {"DEC D",&CPU::dec_d},              // 0x15
        {"LD D,N", &CPU::LD_r_n},          // 0x16
        {"RLA",&CPU::rla},                  // 0x17
        {"JR N",&CPU::jr_n},                // 0x18
        {"ADD HL,DE", &CPU::ADD_HL_DE},      // 0x19
        {"LD A,(DE)", &CPU::LD_A_pDE},  // 0x1a
        {"DEC DE", &CPU::DEC_DE},            // 0x1b
        {"INC E", &CPU::INC_E},              // 0x1c
        {"DEC E", &CPU::DEC_E},              // 0x1d
        {"LD E,N", &CPU::LD_r_n},            // 0x1e
        {"RRA",&CPU::rra},                  // 0x1f

        {"JR NZ",&CPU::jr_nz},              // 0x20
        {"LD HL,NN", &CPU::LD_HL_nn},       // 0x21
        {"LD (NN),HL", &CPU::LDI_pHL_A},    // 0x22
        {"INC HL", &CPU::INC_HL},            // 0x23
        {"INC H", &CPU::INC_H},              // 0x24
        {"DEC H", &CPU::DEC_H},              // 0x25
        {"LD H,N", &CPU::LD_r_n},           // 0x26
        {"DAA",&CPU::daa},                  // 0x27
        {"JR Z",&CPU::jr_z},                // 0x28
        {"ADD HL,HL", &CPU::ADD_HL_HL},     // 0x29
        {"LD HL,(NN)", &CPU::LDI_A_pHL},    // 0x2a
        {"DEC HL", &CPU::DEC_HL},           // 0x2b
        {"INC L", &CPU::INC_L},             // 0x2c
        {"DEC L", &CPU::DEC_L},             // 0x2d
        {"LD L,N", &CPU::LD_r_n},           // 0x2e
        {"CPL",&CPU::cpl},                  // 0x2f

        {"JR NC",&CPU::jr_nc},              // 0x30
        {"LD SP,NN", &CPU::LD_SP_nn},       // 0x31
        {"LD (NN),A", &CPU::LDD_pHL_A},      // 0x32
        {"INC SP", &CPU::INC_SP},           // 0x33
        {"INC (HL)", &CPU::INC_pHL},        // 0x34
        {"DEC (HL)", &CPU::DEC_pHL},        // 0x35
        {"LD (HL),N", &CPU::LD_pHL_n},      // 0x36
        {"SCF",&CPU::scf},                  // 0x37
        {"JR C,N",&CPU::jr_c},              // 0x38
        {"ADD HL,SP",&CPU::add_hl_sp},      // 0x39
        {"LD A,(NN)",&CPU::LDD_A_pHL},    // 0x3a
        {"DEC SP", &CPU::DEC_SP},           // 0x3b
        {"INC A", &CPU::INC_A},             // 0x3c
        {"DEC A", &CPU::DEC_A},             // 0x3d
        {"LD A,N",&CPU::LD_r_n},            // 0x3e
        {"CCF",&CPU::ccf},                  // 0x3f

        {"LD R,R", &CPU::LD_r_r},            // 0x40
        {"LD R,R", &CPU::LD_r_r},            // 0x41
        {"LD R,R", &CPU::LD_r_r},            // 0x42
        {"LD R,R", &CPU::LD_r_r},            // 0x43
        {"LD R,R", &CPU::LD_r_r},            // 0x44
        {"LD R,R", &CPU::LD_r_r},            // 0x45
        {"LD R,R", &CPU::LD_r_r},            // 0x46
        {"LD R,R", &CPU::LD_r_r},            // 0x47
        {"LD R,R", &CPU::LD_r_r},            // 0x48
        {"LD R,R", &CPU::LD_r_r},            // 0x49
        {"LD R,R", &CPU::LD_r_r},            // 0x4a
        {"LD R,R", &CPU::LD_r_r},            // 0x4b
        {"LD R,R", &CPU::LD_r_r},            // 0x4c
        {"LD R,R", &CPU::LD_r_r},            // 0x4d
        {"LD R,R", &CPU::LD_r_r},            // 0x4e
        {"LD R,R", &CPU::LD_r_r},            // 0x4f

        {"LD R,R", &CPU::LD_r_r},            // 0x50
        {"LD R,R", &CPU::LD_r_r},            // 0x51
        {"LD R,R", &CPU::LD_r_r},            // 0x52
        {"LD R,R", &CPU::LD_r_r},            // 0x53
        {"LD R,R", &CPU::LD_r_r},            // 0x54
        {"LD R,R", &CPU::LD_r_r},            // 0x55
        {"LD R,R", &CPU::LD_r_r},            // 0x56
        {"LD R,R", &CPU::LD_r_r},            // 0x57
        {"LD R,R", &CPU::LD_r_r},            // 0x58
        {"LD R,R", &CPU::LD_r_r},            // 0x59
        {"LD R,R", &CPU::LD_r_r},            // 0x5a
        {"LD R,R", &CPU::LD_r_r},            // 0x5b
        {"LD R,R", &CPU::LD_r_r},            // 0x5c
        {"LD R,R", &CPU::LD_r_r},            // 0x5d
        {"LD R,R", &CPU::LD_r_r},            // 0x5e
        {"LD R,R", &CPU::LD_r_r},            // 0x5f

        {"LD R,R", &CPU::LD_r_r},            // 0x60
        {"LD R,R", &CPU::LD_r_r},            // 0x61
        {"LD R,R", &CPU::LD_r_r},            // 0x62
        {"LD R,R", &CPU::LD_r_r},            // 0x63
        {"LD R,R", &CPU::LD_r_r},            // 0x64
        {"LD R,R", &CPU::LD_r_r},            // 0x65
        {"LD R,R", &CPU::LD_r_r},            // 0x66
        {"LD R,R", &CPU::LD_r_r},            // 0x67
        {"LD R,R", &CPU::LD_r_r},            // 0x68
        {"LD R,R", &CPU::LD_r_r},            // 0x69
        {"LD R,R", &CPU::LD_r_r},            // 0x6a
        {"LD R,R", &CPU::LD_r_r},            // 0x6b
        {"LD R,R", &CPU::LD_r_r},            // 0x6c
        {"LD R,R", &CPU::LD_r_r},            // 0x6d
        {"LD R,R", &CPU::LD_r_r},            // 0x6e
        {"LD R,R", &CPU::LD_r_r},            // 0x6f

        {"LD R,R", &CPU::LD_r_r},            // 0x70
        {"LD R,R", &CPU::LD_r_r},            // 0x71
        {"LD R,R", &CPU::LD_r_r},            // 0x72
        {"LD R,R", &CPU::LD_r_r},            // 0x73
        {"LD R,R", &CPU::LD_r_r},            // 0x74
        {"LD R,R", &CPU::LD_r_r},            // 0x75
        {"HALT",&CPU::halt},                // 0x76
        {"LD R,R", &CPU::LD_r_r},            // 0x77
        {"LD R,R", &CPU::LD_r_r},            // 0x78
        {"LD R,R", &CPU::LD_r_r},            // 0x79
        {"LD R,R", &CPU::LD_r_r},            // 0x7a
        {"LD R,R", &CPU::LD_r_r},            // 0x7b
        {"LD R,R", &CPU::LD_r_r},            // 0x7c
        {"LD R,R", &CPU::LD_r_r},            // 0x7d
        {"LD R,R", &CPU::LD_r_r},            // 0x7e
        {"LD R,R", &CPU::LD_r_r},            // 0x7f

        {"ADD A,r",&CPU::add_a_r},          // 0x80
        {"ADD A,r",&CPU::add_a_r},          // 0x81
        {"ADD A,r",&CPU::add_a_r},          // 0x82
        {"ADD A,r",&CPU::add_a_r},          // 0x83
        {"ADD A,r",&CPU::add_a_r},          // 0x84
        {"ADD A,r",&CPU::add_a_r},          // 0x85
        {"ADD A,r",&CPU::add_a_r},          // 0x86
        {"ADD A,r",&CPU::add_a_r},          // 0x87

        {"ADC A,r",&CPU::adc_a_r},          // 0x88
        {"ADC A,r",&CPU::adc_a_r},          // 0x89
        {"ADC A,r",&CPU::adc_a_r},          // 0x8a
        {"ADC A,r",&CPU::adc_a_r},          // 0x8b
        {"ADC A,r",&CPU::adc_a_r},          // 0x8c
        {"ADC A,r",&CPU::adc_a_r},          // 0x8d
        {"ADC A,r",&CPU::adc_a_r},          // 0x8e
        {"ADC A,r",&CPU::adc_a_r},          // 0x8f

        {"SUB r",&CPU::sub_r},              // 0x90
        {"SUB r",&CPU::sub_r},              // 0x91
        {"SUB r",&CPU::sub_r},              // 0x92
        {"SUB r",&CPU::sub_r},              // 0x93
        {"SUB r",&CPU::sub_r},              // 0x94
        {"SUB r",&CPU::sub_r},              // 0x95
        {"SUB r",&CPU::sub_r},              // 0x96
        {"SUB r",&CPU::sub_r},              // 0x97

        {"SBC r",&CPU::sbc_r},              // 0x98
        {"SBC r",&CPU::sbc_r},              // 0x99
        {"SBC r",&CPU::sbc_r},              // 0x9a
        {"SBC r",&CPU::sbc_r},              // 0x9b
        {"SBC r",&CPU::sbc_r},              // 0x9c
        {"SBC r",&CPU::sbc_r},              // 0x9d
        {"SBC r",&CPU::sbc_r},              // 0x9e
        {"SBC r",&CPU::sbc_r},              // 0x9f

        { "AND r",&CPU::and_r},             // 0xa0
        { "AND r",&CPU::and_r},             // 0xa1
        { "AND r",&CPU::and_r},             // 0xa2
        { "AND r",&CPU::and_r},             // 0xa3
        { "AND r",&CPU::and_r},             // 0xa4
        { "AND r",&CPU::and_r},             // 0xa5
        { "AND r",&CPU::and_r},             // 0xa6
        { "AND r",&CPU::and_r},             // 0xa7

        { "XOR r",&CPU::xor_r},             // 0xa8
        { "XOR r",&CPU::xor_r},             // 0xa9
        { "XOR r",&CPU::xor_r},             // 0xaa
        { "XOR r",&CPU::xor_r},             // 0xab
        { "XOR r",&CPU::xor_r},             // 0xac
        { "XOR r",&CPU::xor_r},             // 0xad
        { "XOR r",&CPU::xor_r},             // 0xae
        { "XOR r",&CPU::xor_r},             // 0xaf

        { "OR r",&CPU::or_r},             // 0xb0
        { "OR r",&CPU::or_r},             // 0xb1
        { "OR r",&CPU::or_r},             // 0xb2
        { "OR r",&CPU::or_r},             // 0xb3
        { "OR r",&CPU::or_r},             // 0xb4
        { "OR r",&CPU::or_r},             // 0xb5
        { "OR r",&CPU::or_r},             // 0xb6
        { "OR r",&CPU::or_r},             // 0xb7

        { "CP r",&CPU::cp_r},             // 0xb8
        { "CP r",&CPU::cp_r},             // 0xb9
        { "CP r",&CPU::cp_r},             // 0xba
        { "CP r",&CPU::cp_r},             // 0xbb
        { "CP r",&CPU::cp_r},             // 0xbc
        { "CP r",&CPU::cp_r},             // 0xbd
        { "CP r",&CPU::cp_r},             // 0xbe
        { "CP r",&CPU::cp_r},             // 0xbf

        {"RET NZ", &CPU::RET_cc},            // 0xc0
        {"POP BC",&CPU::pop_qq},            // 0xc1
        {"JP NZ NN",&CPU::jp_cc_nn},        // 0xc2
        {"JP NN",&CPU::jp_nn},              // 0xc3
        {"CALL NZ,NN",&CPU::call_cc_nn},    // 0xc4
        {"PUSH BC",&CPU::push_bc},          // 0xc5
        {"ADD A,n",&CPU::add_a_n},          // 0xc6
        {"RST 00h",&CPU::rst},              // 0xc7
        {"RET Z", &CPU::RET_cc},             // 0xc8
        {"RET", &CPU::RET},                  // 0xc9
        {"JP Z,**",&CPU::jp_cc_nn},         // 0xca
        {"BIT opcode group", &CPU::decode_bit_instruction}, // 0xcb
        {"CALL Z,nn",&CPU::call_cc_nn},     // 0xcc
        {"CALL nn",&CPU::call},             // 0xcd
        {"ADC A,N",&CPU::adc_a_n},          // 0xce
        {"RST 08h",&CPU::rst},              // 0xcf

        {"RET NC", &CPU::RET_cc},           // 0xd0
        {"POP DE",&CPU::pop_qq},            // 0xd1
        {"JP NC,NN",&CPU::jp_cc_nn},        // 0xd2
        {"OUT (N),A",&CPU::invalid_opcode}, // 0xd3
        {"CALL NC,NN",&CPU::call_cc_nn},    // 0xd4
        {"PUSH DE",&CPU::push_de},          // 0xd5
        {"SUB N",&CPU::sub_n},              // 0xd6
        {"RST 10h",&CPU::rst},              // 0xd7
        {"RET C", &CPU::RET_cc},            // 0xd8
        {"EXX",&CPU::RETI},                 // 0xd9
        {"JP C,NN",&CPU::jp_cc_nn},         // 0xda
        {"IN A,(n)",&CPU::invalid_opcode},  // 0xdb
        {"CALL C,NN",&CPU::call_cc_nn},     // 0xdc
        {"", &CPU::invalid_opcode},         // 0xdd
        {"SBC N",&CPU::sbc_n},              // 0xde
        {"RST 18h",&CPU::rst},              // 0xdf

        {"RET PO", &CPU::LD_ff00n_A},            // 0xe0
        {"POP HL",&CPU::pop_qq},            // 0xe1
        {"JP PO,NN",&CPU::LD_ff00C_A},     // 0xe2
        {"EX (SP),HL", &CPU::invalid_opcode},  // 0xe3
        {"CALL PO,NN",&CPU::invalid_opcode},    // 0xe4
        {"PUSH HL",&CPU::push_hl},          // 0xe5
        {"AND N",&CPU::and_n},              // 0xe6
        {"RST 20h",&CPU::rst},              // 0xe7
        {"RET PE", &CPU::ADD_SP_s8},            // 0xe8
        {"JP (HL)",&CPU::jp_ptr_hl},        // 0xe9
        {"JP PE,NN",&CPU::LD_pnn_A},        // 0xea
        {"EX DH,HL",&CPU::invalid_opcode},        // 0xeb
        {"CALL PE,NN",&CPU::invalid_opcode},// 0xec
        {"", &CPU::invalid_opcode},         // 0xed Extended instructions
        {"XOR N",&CPU::xor_n},              // 0xee
        {"RST 28h",&CPU::rst},              // 0xef

        {"RET P", &CPU::LD_A_ff00n},             // 0xf0
        {"POP AF",&CPU::pop_qq},            // 0xf1
        {"JP P,NN",&CPU::LD_A_ff00C},         // 0xf2
        {"DI",&CPU::disable_interrupts},    // 0xf3
        {"CALL P,NN",&CPU::invalid_opcode},     // 0xf4
        {"PUSH AF",&CPU::push_af},          // 0xf5
        {"OR N",&CPU::or_n},                // 0xf6
        {"RST 30h",&CPU::rst},              // 0xf7
        {"RET M", &CPU::LD_HL_SPs8},        // 0xf8
        {"LD SP,HL",&CPU::ld_sp_hl},        // 0xf9
        {"JP M,NN",&CPU::LD_A_pnn},         // 0xfa
        {"EI",&CPU::enable_interrupts},     // 0xfb
        {"CALL M,NN",&CPU::invalid_opcode}, // 0xfc
        {"", &CPU::invalid_opcode},         // 0xfd
        {"CP N",&CPU::cp_n},                // 0xfe
        {"RST 38h",&CPU::rst}               // 0xff

    };

}

// opcodes: 0xCB ..
void CPU::decode_bit_instruction()
{
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t& reg = reg_from_regcode(op2 & 0b00000111);
    do_bit_instruction(op2,reg);
}

void CPU::reset()
{
    // Initial values
    regs.PC = 0x0000;
    regs.SP = 0xFFFF;
    regs.I = 0;
    regs.R = 0;
    regs.AF = 0xFFFF;

    // Not guarenteed
    regs.BC = 0;
    regs.DE = 0;
    regs.HL = 0;

}

void CPU::AddDebugLog(const std::string& text)
{
    // TODO: also add opcodes
    std::cout << current_pc << ": " << text << std::endl;
    debug_log_entries.push_back({current_pc, {current_opcode,0,0,0}, text});
}



void CPU::step()
{
    // M1: OP Code fetch
    current_pc = regs.PC;
    current_opcode = fetch8BitValue();


    if(current_opcode < instructions.size()  )
    {
#ifdef DEBUG_LOG
        std::cout << std::nouppercase << std::showbase << std::hex << regs.PC-1 << "[" << static_cast<int>(current_opcode) << "] ";
#endif
        (this->*instructions[current_opcode].code)();

        // TODO: wait `currentCycles` number of cycles
        currentCycles = 0;

    }

};

/// Add a number of cycles that the current operation has spent.
/// At the end of each `step`, the final number of cycles is used as a delay to wait before running the next `step`.
void CPU::AddCurrentCycles(uint8_t cycles ){
    // +15 cycles at læse fra (ix+n)
    // +5 hvis ixl/ixh er involveret
}

// *******************************************************
// Flag helpers
// *******************************************************

void CPU::set_AND_operation_flags()
{

    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry, 1);

    setFlag(FlagBitmaskN, 0);
    setFlag(FlagBitmaskC, 0);
}

void CPU::set_INC_operation_flags( uint8_t result )
{
    setFlag( FlagBitmaskZero, result == 0 );   // Set if result is zero
    setFlag( FlagBitmaskHalfCarry, (result & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, false); // reset
}

void CPU::set_DEC_operation_flags( uint8_t result )
{
    setFlag( FlagBitmaskZero, result == 0 );   // Set if result is zero
    setFlag( FlagBitmaskHalfCarry, (result & 0b00001111) == 0b00001111 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, true); // set
}

// *******************************************************
// Instruction methods
// *******************************************************

void CPU::invalid_opcode()
{
#ifdef DEBUG_LOG
    std::cout << "INVALID OPCODE: " << current_opcode << std::endl;
#endif
}
