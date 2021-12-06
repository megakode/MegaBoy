#include <cstdint>
#include <vector>
#include <iostream>

#include "cpu.h"

#include "bit_instructions.h"
#include "load_instructions.h"
#include "jump_instructions.h"
#include "in_out_instructions.h"
#include "arithmetic_instructions.h"
#include "general_instructions.h"
#include "ex_block_search_instruction.h"

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
        {"LD BC,A", &CPU::LD_pBC_A},        // 0x02
        {"INC BC",&CPU::inc_bc},            // 0x03
        {"INC B",&CPU::inc_b},              // 0x04
        {"DEC B",&CPU::dec_b},              // 0x05
        {"LD B,N", &CPU::LD_r_n},          // 0x06
        {"RLCA",&CPU::rlca},                // 0x07
        {"EXAF",&CPU::ex_af},               // 0x08
        {"ADD HL,BC",&CPU::add_hl_bc},      // 0x09
        {"LD A,(BC)", &CPU::LD_A_pBC},  // 0x0a
        {"DEC BC",&CPU::dec_bc},            // 0x0b
        {"INC C",&CPU::inc_c},              // 0x0c
        {"DEC C",&CPU::dec_c},              // 0x0d
        {"LD C,N",&CPU::LD_r_n},            // 0x0e
        {"RRCA",&CPU::rrca},                // 0x0f

        {"DJNZ N",&CPU::djnz_n},            // 0x10
        {"LD DE,NN",&CPU::load_de_nn},      // 0x11
        {"LD (DE),A",&CPU::load_ptr_de_a},  // 0x12
        {"INC DE",&CPU::inc_de},            // 0x13
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
        {"LD (NN),HL", &CPU::LD_pnn_HL},    // 0x22
        {"INC HL",&CPU::inc_hl},            // 0x23
        {"INC H",&CPU::inc_h},              // 0x24
        {"DEC H",&CPU::dec_h},              // 0x25
        {"LD H,N", &CPU::LD_r_n},           // 0x26
        {"DAA",&CPU::daa},                  // 0x27
        {"JR Z",&CPU::jr_z},                // 0x28
        {"ADD HL,HL", &CPU::ADD_HL_HL},     // 0x29
        {"LD HL,(NN)", &CPU::LD_HL_pnn},    // 0x2a
        {"DEC HL", &CPU::DEC_HL},           // 0x2b
        {"INC L", &CPU::INC_L},             // 0x2c
        {"DEC L", &CPU::DEC_L},             // 0x2d
        {"LD L,N", &CPU::LD_r_n},           // 0x2e
        {"CPL",&CPU::cpl},                  // 0x2f

        {"JR NC",&CPU::jr_nc},              // 0x30
        {"LD SP,NN", &CPU::LD_SP_nn},       // 0x31
        {"LD (NN),A", &CPU::LD_pnn_A},      // 0x32
        {"INC SP", &CPU::INC_SP},           // 0x33
        {"INC (HL)", &CPU::INC_pHL},        // 0x34
        {"DEC (HL)", &CPU::DEC_pHL},        // 0x35
        {"LD (HL),N", &CPU::LD_pHL_n},      // 0x36
        {"SCF",&CPU::scf},                  // 0x37
        {"JR C,N",&CPU::jr_c},              // 0x38
        {"ADD HL,SP",&CPU::add_hl_sp},      // 0x39
        {"LD A,(NN)",&CPU::ld_a_ptr_nn},    // 0x3a
        {"DEC SP", &CPU::DEC_SP},           // 0x3b
        {"INC A", &CPU::INC_A},             // 0x3c
        {"DEC A", &CPU::DEC_A},             // 0x3d
        {"LD A,N",&CPU::LD_r_n},            // 0x3e
        {"CCF",&CPU::ccf},                  // 0x3f

        {"LD R,R",&CPU::ld_r_r},            // 0x40
        {"LD R,R",&CPU::ld_r_r},            // 0x41
        {"LD R,R",&CPU::ld_r_r},            // 0x42
        {"LD R,R",&CPU::ld_r_r},            // 0x43
        {"LD R,R",&CPU::ld_r_r},            // 0x44
        {"LD R,R",&CPU::ld_r_r},            // 0x45
        {"LD R,R",&CPU::ld_r_r},            // 0x46
        {"LD R,R",&CPU::ld_r_r},            // 0x47
        {"LD R,R",&CPU::ld_r_r},            // 0x48
        {"LD R,R",&CPU::ld_r_r},            // 0x49
        {"LD R,R",&CPU::ld_r_r},            // 0x4a
        {"LD R,R",&CPU::ld_r_r},            // 0x4b
        {"LD R,R",&CPU::ld_r_r},            // 0x4c
        {"LD R,R",&CPU::ld_r_r},            // 0x4d
        {"LD R,R",&CPU::ld_r_r},            // 0x4e
        {"LD R,R",&CPU::ld_r_r},            // 0x4f

        {"LD R,R",&CPU::ld_r_r},            // 0x50
        {"LD R,R",&CPU::ld_r_r},            // 0x51
        {"LD R,R",&CPU::ld_r_r},            // 0x52
        {"LD R,R",&CPU::ld_r_r},            // 0x53
        {"LD R,R",&CPU::ld_r_r},            // 0x54
        {"LD R,R",&CPU::ld_r_r},            // 0x55
        {"LD R,R",&CPU::ld_r_r},            // 0x56
        {"LD R,R",&CPU::ld_r_r},            // 0x57
        {"LD R,R",&CPU::ld_r_r},            // 0x58
        {"LD R,R",&CPU::ld_r_r},            // 0x59
        {"LD R,R",&CPU::ld_r_r},            // 0x5a
        {"LD R,R",&CPU::ld_r_r},            // 0x5b
        {"LD R,R",&CPU::ld_r_r},            // 0x5c
        {"LD R,R",&CPU::ld_r_r},            // 0x5d
        {"LD R,R",&CPU::ld_r_r},            // 0x5e
        {"LD R,R",&CPU::ld_r_r},            // 0x5f

        {"LD R,R",&CPU::ld_r_r},            // 0x60
        {"LD R,R",&CPU::ld_r_r},            // 0x61
        {"LD R,R",&CPU::ld_r_r},            // 0x62
        {"LD R,R",&CPU::ld_r_r},            // 0x63
        {"LD R,R",&CPU::ld_r_r},            // 0x64
        {"LD R,R",&CPU::ld_r_r},            // 0x65
        {"LD R,R",&CPU::ld_r_r},            // 0x66
        {"LD R,R",&CPU::ld_r_r},            // 0x67
        {"LD R,R",&CPU::ld_r_r},            // 0x68
        {"LD R,R",&CPU::ld_r_r},            // 0x69
        {"LD R,R",&CPU::ld_r_r},            // 0x6a
        {"LD R,R",&CPU::ld_r_r},            // 0x6b
        {"LD R,R",&CPU::ld_r_r},            // 0x6c
        {"LD R,R",&CPU::ld_r_r},            // 0x6d
        {"LD R,R",&CPU::ld_r_r},            // 0x6e
        {"LD R,R",&CPU::ld_r_r},            // 0x6f

        {"LD R,R",&CPU::ld_r_r},            // 0x70
        {"LD R,R",&CPU::ld_r_r},            // 0x71
        {"LD R,R",&CPU::ld_r_r},            // 0x72
        {"LD R,R",&CPU::ld_r_r},            // 0x73
        {"LD R,R",&CPU::ld_r_r},            // 0x74
        {"LD R,R",&CPU::ld_r_r},            // 0x75
        {"HALT",&CPU::halt},                // 0x76
        {"LD R,R",&CPU::ld_r_r},            // 0x77
        {"LD R,R",&CPU::ld_r_r},            // 0x78
        {"LD R,R",&CPU::ld_r_r},            // 0x79
        {"LD R,R",&CPU::ld_r_r},            // 0x7a
        {"LD R,R",&CPU::ld_r_r},            // 0x7b
        {"LD R,R",&CPU::ld_r_r},            // 0x7c
        {"LD R,R",&CPU::ld_r_r},            // 0x7d
        {"LD R,R",&CPU::ld_r_r},            // 0x7e
        {"LD R,R",&CPU::ld_r_r},            // 0x7f

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

        {"RET NZ",&CPU::ret_cc},            // 0xc0
        {"POP BC",&CPU::pop_qq},            // 0xc1
        {"JP NZ NN",&CPU::jp_cc_nn},        // 0xc2
        {"JP NN",&CPU::jp_nn},              // 0xc3
        {"CALL NZ,NN",&CPU::call_cc_nn},    // 0xc4
        {"PUSH BC",&CPU::push_bc},          // 0xc5
        {"ADD A,n",&CPU::add_a_n},          // 0xc6
        {"RST 00h",&CPU::rst},              // 0xc7
        {"RET Z",&CPU::ret_cc},             // 0xc8
        {"RET",&CPU::ret},                  // 0xc9
        {"JP Z,**",&CPU::jp_cc_nn},         // 0xca
        {"BIT opcode group", &CPU::decode_bit_instruction}, // 0xcb
        {"CALL Z,nn",&CPU::call_cc_nn},     // 0xcc
        {"CALL nn",&CPU::call},             // 0xcd
        {"ADC A,N",&CPU::adc_a_n},          // 0xce
        {"RST 08h",&CPU::rst},              // 0xcf

        {"RET NC",&CPU::ret_cc},            // 0xd0
        {"POP DE",&CPU::pop_qq},            // 0xd1
        {"JP NC,NN",&CPU::jp_cc_nn},        // 0xd2
        {"OUT (N),A",&CPU::out_n_a},        // 0xd3
        {"CALL NC,NN",&CPU::call_cc_nn},    // 0xd4
        {"PUSH DE",&CPU::push_de},          // 0xd5
        {"SUB N",&CPU::sub_n},              // 0xd6
        {"RST 10h",&CPU::rst},              // 0xd7
        {"RET C",&CPU::ret_cc},             // 0xd8
        {"EXX",&CPU::exx},                  // 0xd9
        {"JP C,NN",&CPU::jp_cc_nn},         // 0xda
        {"IN A,(n)",&CPU::in_a_n},          // 0xdb
        {"CALL C,NN",&CPU::call_cc_nn},     // 0xdc
        {"IX opcode group", &CPU::decode_ix_instruction}, // 0xdd
        {"SBC N",&CPU::sbc_n},              // 0xde
        {"RST 18h",&CPU::rst},              // 0xdf

        {"RET PO",&CPU::ret_cc},            // 0xe0
        {"POP HL",&CPU::pop_qq},            // 0xe1
        {"JP PO,NN",&CPU::jp_cc_nn},        // 0xe2
        {"EX (SP),HL", &CPU::EX_pSP_HL},  // 0xe3
        {"CALL PO,NN",&CPU::call_cc_nn},    // 0xe4
        {"PUSH HL",&CPU::push_hl},          // 0xe5
        {"AND N",&CPU::and_n},              // 0xe6
        {"RST 20h",&CPU::rst},              // 0xe7
        {"RET PE",&CPU::ret_cc},            // 0xe8
        {"JP (HL)",&CPU::jp_ptr_hl},        // 0xe9
        {"JP PE,NN",&CPU::jp_cc_nn},        // 0xea
        {"EX DH,HL",&CPU::ex_de_hl},        // 0xeb
        {"CALL PE,NN",&CPU::call_cc_nn},    // 0xec
        {"Extended instruction", &CPU::decode_extended_instruction}, // 0xed
        {"XOR N",&CPU::xor_n},              // 0xee
        {"RST 28h",&CPU::rst},              // 0xef

        {"RET P",&CPU::ret_cc},             // 0xf0
        {"POP AF",&CPU::pop_qq},            // 0xf1
        {"JP P,NN",&CPU::jp_cc_nn},         // 0xf2
        {"DI",&CPU::disable_interrupts},    // 0xf3
        {"CALL P,NN",&CPU::call_cc_nn},     // 0xf4
        {"PUSH AF",&CPU::push_af},          // 0xf5
        {"OR N",&CPU::or_n},                // 0xf6
        {"RST 30h",&CPU::rst},              // 0xf7
        {"RET M",&CPU::ret_cc},             // 0xf8
        {"LD SP,HL",&CPU::ld_sp_hl},        // 0xf9
        {"JP M,NN",&CPU::jp_cc_nn},         // 0xfa
        {"EI",&CPU::enable_interrupts},     // 0xfb
        {"CALL M,NN",&CPU::call_cc_nn},     // 0xfc
        {"IY instruction group", &CPU::decode_iy_instruction}, // 0xfd
        {"CP N",&CPU::cp_n},                // 0xfe
        {"RST 38h",&CPU::rst}               // 0xff

    };

    extended_instructions = {

            // These opcodes do not exist, but to have the correct index position of the other opcodes in the lookup table, add empty ones here
            // 0x00 - 0x0f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            // 0x10 - 0x1f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            // 0x20 - 0x2f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            // 0x30 - 0x3f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},

            [&](){ in(regs.B,regs.C); },                        // 0x40: IN B,(C)
            [&](){ out_n_a(); },                                // 0x41: OUT (C),B
            [&](){ sbc_hl_nn(regs.BC); },                       // 0x42: SBC HL,BC
            [&](){ LD_pnn_rr(fetch16BitValue(), regs.BC); },  // 0x43: LD (**),BC
            [&](){ neg(); },                                    // 0x44: NEG
            [&](){ retn(); },                                   // 0x45: RETN
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },  // 0x46: im 0
            [&](){ specialRegs.I = regs.A; },                   // 0x47: LD I,A
            [&](){ in(regs.C,regs.C); },                        // 0x48; IN C,(C)
            [&](){ out(regs.C,regs.C); },                       // 0x49: OUT (c),C
            [&](){ adc_hl_nn(regs.BC); },                       // 0x4A: ADC HL,BC
            [&](){ LD_rr_pnn(regs.BC, fetch16BitValue()); },  // 0x4B: LD BC,(NN)
            [&](){ neg(); },                                    // 0x4C: NEG
            [&](){ reti(); },                                   // 0x4d: RETI
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },  // 0x4e: Set undefined IM0/1 mode
            [&](){ specialRegs.R = regs.A; },                   // 0x4f: LD R,A

            [&](){ in(regs.D,regs.C); },                        // 0x50: in d,(c)
            [&](){ out(regs.C,regs.D); },                       // 0x51: out (c),d
            [&](){ sbc_hl_nn(regs.DE); },                       // 0x52: sbc hl,de
            [&](){ LD_pnn_rr(fetch16BitValue(), regs.DE); },  // 0x53: ld (**),de
            [&](){ neg(); },                                    // 0x54: neg
            [&](){ retn(); },                                   // 0x55: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_1); },  // 0x56: im 1
            [&](){ LD_A_I(); },                                 // 0x57: ld a,i
            [&](){ in(regs.E,regs.C); },                        // 0x58: in e,(c)
            [&](){ out(regs.C,regs.E); },                       // 0x59: out (c),e
            [&](){ adc_hl_nn(regs.DE); },                       // 0x5a: adc hl,de
            [&](){ LD_rr_pnn(regs.DE, fetch16BitValue()); },  // 0x5b: ld de,(**)
            [&](){ neg(); },                                    // 0x5c: neg
            [&](){ retn(); },                                   // 0x5d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); },  // 0x5e: im 2
            [&](){ LD_A_R(); },                                 // 0x5f: ld a,r

            [&](){ in(regs.H,regs.C); },                        // 0x60: IN H,(C)
            [&](){ out(regs.C,regs.H); },                       // 0x61: OUT (C),H
            [&](){ sbc_hl_nn(regs.HL); },                       // 0x62: SBC HL,HL
            [&](){ LD_pnn_rr(fetch16BitValue(), regs.HL); },  // 0x63: ld (**),hl
            [&](){ neg(); },                                    // 0x64: neg
            [&](){ retn(); },                                   // 0x65: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },  // 0x66: im 0
            [&](){ rrd(); },                                    // 0x67: rrd
            [&](){ in(regs.L,regs.C); },                        // 0x68: in l,(c)
            [&](){ out(regs.C,regs.L); },                       // 0x69: out (c),l
            [&](){ adc_hl_nn(regs.HL); },                       // 0x6a: ADC HL,HL
            [&](){ LD_rr_pnn(regs.HL, fetch16BitValue()); },  // 0x6b: ld hl,(**)
            [&](){ neg(); },                                    // 0x6c: neg
            [&](){ retn(); },                                   // 0x6d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },  // 0x6e: im 0/1
            [&](){ rld(); },                                    // 0x6f: rld

            [&](){ in(regs.C,regs.C,true); },                        // 0x70: in (c)
            [&](){ out(regs.C,0); },                                 // 0x71: out
            [&](){ sbc_hl_nn(specialRegs.SP); },                     // 0x72: sbc hl,sp
            [&](){ LD_pnn_rr(fetch16BitValue(), specialRegs.SP); },// 0x73: ld (**),sp
            [&](){ neg(); },                                         // 0x74: neg
            [&](){ retn(); },                                        // 0x75:
            [&](){ set_interrupt_mode(InterruptMode::IM_1); },       // 0x76: im 1
            [&](){ invalid_opcode(); },                              // 0x77
            [&](){ in(regs.A,regs.C); },                             // 0x78 in a,(c)
            [&](){ out(regs.C,regs.A); },                            // 0x79 out (c),a
            [&](){ adc_hl_nn(specialRegs.SP); },                     // 0x7a adc hl,sp
            [&](){ LD_rr_pnn(specialRegs.SP, fetch16BitValue()); },// 0x7b ld sp,(**)
            [&](){ neg(); },                                         // 0x7c neg
            [&](){ retn(); },                                        // 0x7d retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); },       // 0x7e im2
            [&](){ },                                                // 0x7f

            // 0x80 - 0x8f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            // 0x90 - 0x9f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},

            [&](){ ldi(); }, // 0xa0: LDI
            [&](){ cpi(); }, // 0xa1: CPI
            [&](){ ini(); }, // 0xa2: INI
            [&](){ outi(); },// 0xa3: OUTI
            [&](){ },        // 0xa4
            [&](){ },        // 0xa5
            [&](){ },        // 0xa6
            [&](){ },        // 0xa7
            [&](){ ldd(); }, // 0xa8
            [&](){ cpd(); }, // 0xa9
            [&](){ ind(); }, // 0xaa
            [&](){ outd(); },// 0xab
            [&](){ },        // 0xac
            [&](){ },        // 0xad
            [&](){ },        // 0xae
            [&](){ },        // 0xaf

            [&](){ ldir(); }, // 0xb0
            [&](){ cpir(); }, // 0xb1
            [&](){ inir(); }, // 0xb2
            [&](){ otir(); }, // 0xb3
            [&](){ }, // 0xb4
            [&](){ }, // 0xb5
            [&](){ }, // 0xb6
            [&](){ }, // 0xb7
            [&](){ lddr(); }, // 0xb8
            [&](){ cpdr(); }, // 0xb9
            [&](){ indr(); }, // 0xba
            [&](){ otdr(); }, // 0xbb
            [&](){ }, // 0xbc
            [&](){ }, // 0xbd
            [&](){ }, // 0xbe
            [&](){ }, // 0xbf
    };

    ix_instructions = {

        [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, // 0x00 - 0x08
        [&](){ add16(specialRegs.IX,regs.BC); }, // 0x09: ADD IX,BC
        [&](){},[&](){},[&](){},[&](){},[&](){},[&](){},

        [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
        [&](){ add16(specialRegs.IX,regs.DE); }, // 0x19: ADD IX,DE
        [&](){},[&](){},[&](){},[&](){},[&](){},[&](){}, // 0x1a - 0x1f

        [&](){},                                                    // 0x20
        [&](){ LD_IX_nn(); },                                       // 0x21 LD IX,**
        [&](){ LD_pnn_rr(fetch16BitValue(), specialRegs.IX); },     // 0x22 LD (**),IX
        [&](){ INC_IX(); },                                         // 0x23 INC IX
        [&](){ INC_r(specialRegs.IXH); },                           // 0x24 INC IXH
        [&](){ DEC_r(specialRegs.IXL); },                           // 0x25 INC IXL
        [&](){ LD_IXH_n(fetch8BitValue()); },                       // 0x26 LD IXH,n
        [&](){},                                                    // 0x27
        [&](){},                                                    // 0x28
        [&](){ add16(specialRegs.IX,specialRegs.IX); },             // 0x29 ADD IX,IX
        [&](){ LD_IX_pnn(); },                                      // 0x2a LD IX,(nn)
        [&](){ DEC_IX(); },                                         // 0x2b DEC IX
        [&](){ INC_r(specialRegs.IXL); },                           // 0x2c INC IXL
        [&](){ DEC_r(specialRegs.IXL); },                           // 0x2d DEC IXL
        [&](){ LD_IXL_n(fetch8BitValue()); },                       // 0x2e LD IXL,n
        [&](){},                                                    // 0x2f

        [&](){}, // 0x30
        [&](){}, // 0x31
        [&](){}, // 0x32
        [&](){}, // 0x33
        [&](){ INC_pIXn(); },       // 0x34
        [&](){ DEC_pIXn(); },   // 0x35
        [&](){ LD_pIXn_n(); },  // 0x36
        [&](){  }, // 0x37
        [&](){  }, // 0x38
        [&](){ add16(specialRegs.IX,specialRegs.SP,false); }, // 0x39
        [&](){  }, // 0x3A
        [&](){  }, // 0x3B
        [&](){  }, // 0x3C
        [&](){  }, // 0x3d
        [&](){  }, // 0x3e
        [&](){  }, // 0x3f

        [&](){  }, // 0x40
        [&](){  }, // 0x41
        [&](){  }, // 0x42
        [&](){  }, // 0x43
        [&](){ LD_r(regs.B, specialRegs.IXH); }, // 0x44 ld B,IXh
        [&](){ LD_r(regs.B, specialRegs.IXL); }, // 0x45
        [&](){ LD_r_pIXn(regs.B); }, // 0x46
        [&](){  }, // 0x47
        [&](){  }, // 0x48
        [&](){  }, // 0x49
        [&](){  }, // 0x4a
        [&](){  }, // 0x4b
        [&](){ LD_r(regs.C, specialRegs.IXH); }, // 0x4c
        [&](){ LD_r(regs.C, specialRegs.IXL); }, // 0x4d
        [&](){ LD_r_pIXn(regs.C); }, // 0x4e
        [&](){  }, // 0x4f

        [&](){  }, // 0x50
        [&](){  }, // 0x51
        [&](){  }, // 0x52
        [&](){  }, // 0x53
        [&](){ LD_r(regs.D, specialRegs.IXH); }, // 0x54
        [&](){ LD_r(regs.D, specialRegs.IXL); }, // 0x55
        [&](){ LD_r_pIXn(regs.D); }, // 0x56 LD D,(ix+n)
        [&](){  }, // 0x57
        [&](){  }, // 0x58
        [&](){  }, // 0x59
        [&](){  }, // 0x5a
        [&](){  }, // 0x5b
        [&](){ LD_r(regs.E, specialRegs.IXH); }, // 0x5c
        [&](){ LD_r(regs.E, specialRegs.IXL); }, // 0x5d
        [&](){ LD_r_pIXn(regs.E); }, // 0x5e LD E,(ix+n)
        [&](){  }, // 0x5f

        [&](){ LD_r(specialRegs.IXH, regs.B); }, // 0x60
        [&](){ LD_r(specialRegs.IXH, regs.C); }, // 0x61
        [&](){ LD_r(specialRegs.IXH, regs.D); }, // 0x62
        [&](){ LD_r(specialRegs.IXH, regs.E); }, // 0x63
        [&](){ LD_r(specialRegs.IXH, specialRegs.IXH); }, // 0x64
        [&](){ LD_r(specialRegs.IXH, specialRegs.IXL); }, // 0x65
        [&](){ LD_r_pIXn(regs.H); }, // 0x66 LD E,(ix+n)
        [&](){ LD_r(specialRegs.IXH, regs.A); }, // 0x67
        [&](){ LD_r(specialRegs.IXL, regs.B); }, // 0x68
        [&](){ LD_r(specialRegs.IXL, regs.C); }, // 0x69
        [&](){ LD_r(specialRegs.IXL, regs.D); }, // 0x6a
        [&](){ LD_r(specialRegs.IXL, regs.E); }, // 0x6b
        [&](){ LD_r(specialRegs.IXL, specialRegs.IXH); }, // 0x6c
        [&](){ LD_r(specialRegs.IXL, specialRegs.IXL); }, // 0x6d
        [&](){ LD_r_pIXn(regs.L); }, // 0x6e LD E,(ix+n)
        [&](){ LD_r(specialRegs.IXL, regs.A); }, // 0x6f

        [&](){ LD_pIXn_r(regs.B); }, // 0x70
        [&](){ LD_pIXn_r(regs.C); }, // 0x71
        [&](){ LD_pIXn_r(regs.D); }, // 0x72
        [&](){ LD_pIXn_r(regs.E); }, // 0x73
        [&](){ LD_pIXn_r(regs.H); }, // 0x74
        [&](){ LD_pIXn_r(regs.L); }, // 0x75
        [&](){  },                   // 0x76
        [&](){ LD_pIXn_r(regs.B); }, // 0x77
        [&](){  },                   // 0x78
        [&](){  },                   // 0x79
        [&](){  },                   // 0x7a
        [&](){  },                   // 0x7b
        [&](){ LD_r(regs.A, specialRegs.IXH); }, // 0x7c
        [&](){ LD_r(regs.A, specialRegs.IXL); }, // 0x7d
        [&](){ LD_r_pIXn(regs.A); },              // 0x7e
        [&](){  },                                // 0x7f

        [&](){  }, // 0x80
        [&](){  }, // 0x81
        [&](){  }, // 0x82
        [&](){  }, // 0x83
        [&](){ add(specialRegs.IXH); }, // 0x84 add a,ixh
        [&](){ add(specialRegs.IXL); }, // 0x85 add a,ixl
        [&](){ add(fetch_pIXn()); }, // 0x86 add a,(ix+n)
        [&](){  }, // 0x87
        [&](){  }, // 0x88
        [&](){  }, // 0x89
        [&](){  }, // 0x8a
        [&](){  }, // 0x8b
        [&](){ LD_r(regs.A, specialRegs.IXH); },   // 0x8c LD A,IXH
        [&](){ LD_r(regs.A, specialRegs.IXL); },   // 0x8d LD A,IXL
        [&](){ LD_r(regs.A, fetch_pIXn()); },      // 0x8e LD a,(ix+n)
        [&](){  }, // 0x8f

        [&](){  }, // 0x90
        [&](){  }, // 0x91
        [&](){  }, // 0x92
        [&](){  }, // 0x93
        [&](){ sub(specialRegs.IXH); }, // 0x94 SUB IXH
        [&](){ sub(specialRegs.IXL); }, // 0x95 SUB IXL
        [&](){ sub(fetch_pIXn()); },    // 0x96 SUB (IX+n)
        [&](){  }, // 0x97
        [&](){  }, // 0x98
        [&](){  }, // 0x99
        [&](){  }, // 0x9a
        [&](){  }, // 0x9b
        [&](){ sub(specialRegs.IXH,true); }, // 0x9c SBC A,IXH
        [&](){ sub(specialRegs.IXL,true); }, // 0x9d SBC A,IXL
        [&](){ sub(fetch_pIXn(),true); },    // 0x9e SBC A,(IX+n)
        [&](){  }, // 0x9f

        [&](){  }, // 0xa0
        [&](){  }, // 0xa1
        [&](){  }, // 0xa2
        [&](){  }, // 0xa3
        [&](){ and_a_with_value(specialRegs.IXH); }, // 0xa4 AND IXH
        [&](){ and_a_with_value(specialRegs.IXL); }, // 0xa5 AND IXL
        [&](){ and_a_with_value(fetch_pIXn()); },    // 0xa6 AND (IX+n)
        [&](){ }, // 0xa7
        [&](){ }, // 0xa8
        [&](){ }, // 0xa9
        [&](){ }, // 0xaa
        [&](){ }, // 0xab
        [&](){ xor_a_with_value(specialRegs.IXH); }, // 0xac XOR IXH
        [&](){ xor_a_with_value(specialRegs.IXL); }, // 0xad XOR IXL
        [&](){ xor_a_with_value(fetch_pIXn()); },    // 0xae XOR (ix+n)
        [&](){ }, // 0xaf

        [&](){ }, // 0xb0
        [&](){ }, // 0xb1
        [&](){ }, // 0xb2
        [&](){ }, // 0xb3
        [&](){ or_a_with_value(specialRegs.IXH); }, // 0xb4 OR IXH
        [&](){ or_a_with_value(specialRegs.IXL); }, // 0xb5 OR IXL
        [&](){ or_a_with_value(fetch_pIXn()); },    // 0xb6 OR (ix+n)
        [&](){ }, // 0xb7
        [&](){ }, // 0xb8
        [&](){ }, // 0xb9
        [&](){ }, // 0xba
        [&](){ }, // 0xbb
        [&](){ cp_a_with_value( specialRegs.IXH); }, // 0xbc CP IXH
        [&](){ cp_a_with_value( specialRegs.IXL); }, // 0xbd CP IXL
        [&](){ cp_a_with_value( fetch_pIXn()); },    // 0xbe CP (ix+n)
        [&](){ }, // 0xbf

         // 0xc0 - 0xca
        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
        [&](){ decode_ix_bit_instruction(); }, // 0xcb - IX Bit instruction group
        [&](){ },[&](){ },[&](){ },[&](){ },

        // 0xd0 - 0xdf
        [&](){ }, [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

        [&](){ },                           // 0xe0
        [&](){ pop16(specialRegs.IX); }, // 0xe1 POP IX
        [&](){ }, // 0xe2
        [&](){ EX_pSP_IX(); }, // 0xe3 EX (SP),IX
        [&](){ }, // 0xe4
        [&](){ push_ix(); }, // 0xe5
        [&](){ }, // 0xe6
        [&](){ }, // 0xe7
        [&](){ }, // 0xe8
        [&](){ jp_IX(); }, // 0xe9 JP IX
        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
        [&](){ LD_rr(specialRegs.SP, specialRegs.IX); }, // 0xf9 LD SP,IX
        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
    };

    iy_instructions = {

            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, // 0x00 - 0x08
            [&](){ add16(specialRegs.IY,regs.BC); }, // 0x09: ADD IX,BC
            [&](){},[&](){},[&](){},[&](){},[&](){},[&](){},

            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            [&](){ add16(specialRegs.IY,regs.DE); }, // 0x19: ADD IX,DE
            [&](){},[&](){},[&](){},[&](){},[&](){},[&](){}, // 0x1a - 0x1f

            [&](){},                                                    // 0x20
            [&](){ ld_iy_nn(); },                                       // 0x21 LD IY,**
            [&](){ LD_pnn_rr(fetch16BitValue(), specialRegs.IY); },   // 0x22 LD (**),IY
            [&](){ INC_IY(); },                                         // 0x23 INC IY
            [&](){ INC_r(specialRegs.IYH); },                           // 0x24 INC IYH
            [&](){ DEC_r(specialRegs.IYL); },                           // 0x25 INC IYL
            [&](){ ld_iyh_n(fetch8BitValue()); },                       // 0x26 LD IYH,n
            [&](){},                                                    // 0x27
            [&](){},                                                    // 0x28
            [&](){ add16(specialRegs.IY,specialRegs.IY); },             // 0x29 ADD IY,IY
            [&](){ ld_iy_ptr_nn(); },                                   // 0x2a LD IY,(nn)
            [&](){ DEC_IY(); },                                         // 0x2b DEC IY
            [&](){ INC_r(specialRegs.IYL); },                           // 0x2c INC IYL
            [&](){ DEC_r(specialRegs.IYL); },                           // 0x2d DEC IYL
            [&](){ ld_iyl_n(fetch8BitValue()); },                       // 0x2e LD IYL,n
            [&](){},                                                    // 0x2f

            [&](){}, // 0x30
            [&](){}, // 0x31
            [&](){}, // 0x32
            [&](){}, // 0x33
            [&](){ INC_pIYn(); },  // 0x34 INC (IY+n)
            [&](){ DEC_pIYn(); },  // 0x35 DEC (IY+n)
            [&](){ LD_pIYn_n(); }, // 0x36 LD (IY+n),n
            [&](){  }, // 0x37
            [&](){  }, // 0x38
            [&](){ add16(specialRegs.IY,specialRegs.SP,false); }, // 0x39 ADD IY,SP
            [&](){  }, // 0x3A
            [&](){  }, // 0x3B
            [&](){  }, // 0x3C
            [&](){  }, // 0x3d
            [&](){  }, // 0x3e
            [&](){  }, // 0x3f

            [&](){  }, // 0x40
            [&](){  }, // 0x41
            [&](){  }, // 0x42
            [&](){  }, // 0x43
            [&](){ LD_r(regs.B, specialRegs.IYH); }, // 0x44 ld B,IYh
            [&](){ LD_r(regs.B, specialRegs.IYL); }, // 0x45
            [&](){ LD_r_pIYn(regs.B); }, // 0x46
            [&](){  }, // 0x47
            [&](){  }, // 0x48
            [&](){  }, // 0x49
            [&](){  }, // 0x4a
            [&](){  }, // 0x4b
            [&](){ LD_r(regs.C, specialRegs.IYH); }, // 0x4c
            [&](){ LD_r(regs.C, specialRegs.IYL); }, // 0x4d
            [&](){ LD_r_pIYn(regs.C); }, // 0x4e
            [&](){  }, // 0x4f

            [&](){  }, // 0x50
            [&](){  }, // 0x51
            [&](){  }, // 0x52
            [&](){  }, // 0x53
            [&](){ LD_r(regs.D, specialRegs.IYH); }, // 0x54
            [&](){ LD_r(regs.D, specialRegs.IYL); }, // 0x55
            [&](){ LD_r_pIYn(regs.D); }, // 0x56 LD D,(iy+n)
            [&](){  }, // 0x57
            [&](){  }, // 0x58
            [&](){  }, // 0x59
            [&](){  }, // 0x5a
            [&](){  }, // 0x5b
            [&](){ LD_r(regs.E, specialRegs.IYH); }, // 0x5c
            [&](){ LD_r(regs.E, specialRegs.IYL); }, // 0x5d
            [&](){ LD_r_pIXn(regs.E); }, // 0x5e LD E,(ix+n)
            [&](){  }, // 0x5f

            [&](){ LD_r(specialRegs.IYH, regs.B); }, // 0x60
            [&](){ LD_r(specialRegs.IYH, regs.C); }, // 0x61
            [&](){ LD_r(specialRegs.IYH, regs.D); }, // 0x62
            [&](){ LD_r(specialRegs.IYH, regs.E); }, // 0x63
            [&](){ LD_r(specialRegs.IYH, specialRegs.IYH); }, // 0x64
            [&](){ LD_r(specialRegs.IYH, specialRegs.IYL); }, // 0x65
            [&](){ LD_r_pIYn(regs.H); }, // 0x66 LD E,(ix+n)
            [&](){ LD_r(specialRegs.IYH, regs.A); }, // 0x67
            [&](){ LD_r(specialRegs.IYL, regs.B); }, // 0x68
            [&](){ LD_r(specialRegs.IYL, regs.C); }, // 0x69
            [&](){ LD_r(specialRegs.IYL, regs.D); }, // 0x6a
            [&](){ LD_r(specialRegs.IYL, regs.E); }, // 0x6b
            [&](){ LD_r(specialRegs.IYL, specialRegs.IYH); }, // 0x6c
            [&](){ LD_r(specialRegs.IYL, specialRegs.IYL); }, // 0x6d
            [&](){ LD_r_pIYn(regs.L); }, // 0x6e LD E,(ix+n)
            [&](){ LD_r(specialRegs.IYL, regs.A); }, // 0x6f

            [&](){ LD_pIYn_r(regs.B); }, // 0x70
            [&](){ LD_pIYn_r(regs.C); }, // 0x71
            [&](){ LD_pIYn_r(regs.D); }, // 0x72
            [&](){ LD_pIYn_r(regs.E); }, // 0x73
            [&](){ LD_pIYn_r(regs.H); }, // 0x74
            [&](){ LD_pIYn_r(regs.L); }, // 0x75
            [&](){  },                   // 0x76
            [&](){ LD_pIYn_r(regs.B); }, // 0x77
            [&](){  },                   // 0x78
            [&](){  },                   // 0x79
            [&](){  },                   // 0x7a
            [&](){  },                   // 0x7b
            [&](){ LD_r(regs.A, specialRegs.IYH); }, // 0x7c
            [&](){ LD_r(regs.A, specialRegs.IYL); }, // 0x7d
            [&](){ LD_r_pIYn(regs.A); },              // 0x7e
            [&](){  },                                // 0x7f

            [&](){  }, // 0x80
            [&](){  }, // 0x81
            [&](){  }, // 0x82
            [&](){  }, // 0x83
            [&](){ add(specialRegs.IYH); }, // 0x84 add a,iyh
            [&](){ add(specialRegs.IYL); }, // 0x85 add a,iyl
            [&](){ add(fetch_pIYn()); }, // 0x86 add a,(iy+n)
            [&](){  }, // 0x87
            [&](){  }, // 0x88
            [&](){  }, // 0x89
            [&](){  }, // 0x8a
            [&](){  }, // 0x8b
            [&](){ LD_r(regs.A, specialRegs.IYH); },   // 0x8c LD A,IYH
            [&](){ LD_r(regs.A, specialRegs.IYL); },   // 0x8d LD A,IYL
            [&](){ LD_r(regs.A, fetch_pIYn()); },      // 0x8e LD a,(iy+n)
            [&](){  }, // 0x8f

            [&](){  }, // 0x90
            [&](){  }, // 0x91
            [&](){  }, // 0x92
            [&](){  }, // 0x93
            [&](){ sub(specialRegs.IYH); }, // 0x94 SUB IYH
            [&](){ sub(specialRegs.IYL); }, // 0x95 SUB IYL
            [&](){ sub(fetch_pIYn()); },    // 0x96 SUB (IY+n)
            [&](){  }, // 0x97
            [&](){  }, // 0x98
            [&](){  }, // 0x99
            [&](){  }, // 0x9a
            [&](){  }, // 0x9b
            [&](){ sub(specialRegs.IYH,true); }, // 0x9c SBC A,IYH
            [&](){ sub(specialRegs.IYL,true); }, // 0x9d SBC A,IYL
            [&](){ sub(fetch_pIYn(),true); },    // 0x9e SBC A,(IY+n)
            [&](){  }, // 0x9f

            [&](){  }, // 0xa0
            [&](){  }, // 0xa1
            [&](){  }, // 0xa2
            [&](){  }, // 0xa3
            [&](){ and_a_with_value(specialRegs.IYH); }, // 0xa4 AND IYH
            [&](){ and_a_with_value(specialRegs.IYL); }, // 0xa5 AND IYL
            [&](){ and_a_with_value(fetch_pIYn()); },    // 0xa6 AND (IY+n)
            [&](){ }, // 0xa7
            [&](){ }, // 0xa8
            [&](){ }, // 0xa9
            [&](){ }, // 0xaa
            [&](){ }, // 0xab
            [&](){ xor_a_with_value(specialRegs.IYH); }, // 0xac XOR IYH
            [&](){ xor_a_with_value(specialRegs.IYL); }, // 0xad XOR IYL
            [&](){ xor_a_with_value(fetch_pIYn()); },    // 0xae XOR (iy+n)
            [&](){ }, // 0xaf

            [&](){ }, // 0xb0
            [&](){ }, // 0xb1
            [&](){ }, // 0xb2
            [&](){ }, // 0xb3
            [&](){ or_a_with_value(specialRegs.IYH); }, // 0xb4 OR IYH
            [&](){ or_a_with_value(specialRegs.IYL); }, // 0xb5 OR IYL
            [&](){ or_a_with_value(fetch_pIYn()); },    // 0xb6 OR (iy+n)
            [&](){ }, // 0xb7
            [&](){ }, // 0xb8
            [&](){ }, // 0xb9
            [&](){ }, // 0xba
            [&](){ }, // 0xbb
            [&](){ cp_a_with_value( specialRegs.IYH); }, // 0xbc CP IYH
            [&](){ cp_a_with_value( specialRegs.IYL); }, // 0xbd CP IYL
            [&](){ cp_a_with_value( fetch_pIYn()); },    // 0xbe CP (iy+n)
            [&](){ }, // 0xbf

            // 0xc0 - 0xca
            [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
            [&](){ decode_iy_bit_instruction(); }, // 0xcb - IX Bit instruction group
            [&](){ },[&](){ },[&](){ },[&](){ },

            // 0xd0 - 0xdf
            [&](){ }, [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

            [&](){ },                           // 0xe0
            [&](){ pop16(specialRegs.IY); }, // 0xe1 POP IY
            [&](){ }, // 0xe2
            [&](){ EX_pSP_IY(); }, // 0xe3 EX (SP),IX
            [&](){ }, // 0xe4
            [&](){ push_iy(); }, // 0xe5
            [&](){ }, // 0xe6
            [&](){ }, // 0xe7
            [&](){ }, // 0xe8
            [&](){ jp_IY(); }, // 0xe9 JP IY
            [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

            [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
            [&](){ LD_rr(specialRegs.SP, specialRegs.IY); }, // 0xf9 LD SP,IY
            [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
    };


}


// opcode: 0xed
void CPU::decode_extended_instruction()
{
    uint8_t op2 = fetch8BitValue();

    if(op2 <= extended_instructions.size()){
        extended_instructions[op2]();
    } else {
        assert("extended instruction (0xed ..) has an op2 which is out-of-bounds for the extended_instructions lookup table");
    }
}

// opcodes: 0xCB ..
void CPU::decode_bit_instruction()
{
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t& reg = reg_from_regcode(op2 & 0b00000111);
    do_bit_instruction(op2,reg, reg);
}

void CPU::decode_ix_bit_instruction()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    uint8_t op3 = fetch8BitValue(); // fetch next opcode
    uint8_t &reg = mem[specialRegs.IX + offset];
    do_bit_instruction(op3,reg, reg_from_regcode(op3));
}

void CPU::decode_iy_bit_instruction()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    uint8_t op3 = fetch8BitValue(); // fetch next opcode
    uint8_t &reg = mem[specialRegs.IY + offset];
    do_bit_instruction(op3,reg, reg_from_regcode(op3));
}


// opcodes: 0xdd ..
void CPU::decode_ix_instruction()
{
    uint8_t op2 = fetch8BitValue();

    if(op2 < ix_instructions.size()){
        ix_instructions[op2]();
    } else {
        // This should not be possible
    }
}

// opcode: 0xfd ..
void CPU::decode_iy_instruction()
{
    uint8_t op2 = fetch8BitValue();

    if(op2 < iy_instructions.size()){
        iy_instructions[op2]();
    } else {
        // This should not be possible
    }
}

void CPU::reset()
{
    // Initial values
    specialRegs.PC = 0x0000;
    specialRegs.SP = 0xFFFF;
    specialRegs.I = 0;
    specialRegs.R = 0;
    regs.AF = 0xFFFF;

    // Not guarenteed
    regs.BC = 0;
    regs.DE = 0;
    regs.HL = 0;

}

void CPU::AddDebugLog(const std::string& text)
{
    // TODO: also add opcodes
    debug_log_entries.push_back({current_pc, {current_opcode,0,0,0}, text});
}


void CPU::output_to_port( uint8_t port, uint8_t value)
{
    // TODO: write value to port
}
uint8_t CPU::input_from_port( uint8_t port )
{
    // TODO: read value from port instead
    return 0;
}

void CPU::step()
{
    // M1: OP Code fetch
    current_pc = specialRegs.PC;
    current_opcode = fetch8BitValue();


    if(current_opcode < instructions.size()  )
    {
#ifdef DEBUG_LOG
        std::cout << std::nouppercase << std::showbase << std::hex << specialRegs.PC-1 << "[" << static_cast<int>(current_opcode) << "] ";
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
    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry, 1);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN, 0);
    setFlag(FlagBitmaskC, 0);
}

void CPU::set_INC_operation_flags( uint8_t result )
{
    setFlag( FlagBitmaskSign, result & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, result == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, result == 0x80 ); // Set if B was 0x7f before
    setFlag( FlagBitmaskHalfCarry, (result & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, false); // reset
}
void CPU::set_DEC_operation_flags( uint8_t result )
{
    setFlag( FlagBitmaskSign, result & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, result == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, result == 0x7f ); // Set if B was 0x80 before
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
