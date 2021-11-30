#include <cstdint>
#include <vector>
#include <iostream>

#include "cpu.h"

#include "bit_instructions.h"

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

        {"JR NC",&CPU::jr_nc},              // 0x30
        {"LD SP,NN",&CPU::ld_sp_nn},        // 0x31
        {"LD (NN),A",&CPU::ld_ptr_nn_a},    // 0x32
        {"INC SP",&CPU::inc_sp},            // 0x33
        {"INC (HL)",&CPU::inc_ptr_hl},      // 0x34
        {"DEC (HL)",&CPU::dec_ptr_hl},      // 0x35
        {"LD (HL),N",&CPU::ld_ptr_hl_n},    // 0x36
        {"SCF",&CPU::scf},                  // 0x37
        {"JR C,N",&CPU::jr_c},              // 0x38
        {"ADD HL,SP",&CPU::add_hl_sp},      // 0x39
        {"LD A,(NN)",&CPU::ld_a_ptr_nn},    // 0x3a
        {"DEC SP",&CPU::dec_sp},            // 0x3b
        {"INC A",&CPU::inc_a},              // 0x3c
        {"DEC A",&CPU::dec_a},              // 0x3d
        {"LD A,N",&CPU::ld_a_n},            // 0x3e
        {"CCF",&CPU::ccf},                  // 0x3f

        // 0x40 - 0x7f instructions are all LD r,r which is caught in the step() function.
        // excluding the (0x76) HALT instruction, which we explicitly add here.
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

            // 0x40: in b,(c)
            [&](){ in(regs.B,regs.C); },
            // 0x41 out(c),b
            [&](){ out_n_a(); },
            // 0x42: sbc hl,bc
            [&](){ sbc_hl_nn(regs.BC); },
            // 0x43: ld (**),bc
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.BC); },
            // 0x44: neg
            [&](){ neg(); },
            // 0x45: retn
            [&](){ retn(); },
            // 0x46: im 0
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },
            // 0x47: LD I,A
            [&](){ specialRegs.I = regs.A; },
            // 0x48; IN C,(C)
            [&](){ in(regs.C,regs.C); },
            // 0x49: OUT (c),C
            [&](){ out(regs.C,regs.C); },
            // 0x4A: ADC HL,BC
            [&](){ adc_hl_nn(regs.BC); },
            // 0x4B: LD BC,(NN)
            [&](){ ld_rr_ptr_nn(regs.BC,fetch16BitValue()); },
            // 0x4C: neg
            [&](){ neg(); },
            // 0x4d:
            [&](){ reti(); },
            // 0x4e: set undefined IM0/1 mode
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },
            // 0x4f: ld r,a
            [&](){ specialRegs.R = regs.A; },

            [&](){ in(regs.D,regs.C); },                            // 0x50: in d,(c)
            [&](){ out(regs.C,regs.D); },                           // 0x51: out (c),d
            [&](){ sbc_hl_nn(regs.DE); },                           // 0x52: sbc hl,de
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.DE); },      // 0x53: ld (**),de
            [&](){ neg(); },                                        // 0x54: neg
            [&](){ retn(); },                                       // 0x55: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_1); },      // 0x56: im 1
            [&](){ ld_a_i(); },                                     // 0x57: ld a,i
            [&](){ in(regs.E,regs.C); },                            // 0x58: in e,(c)
            [&](){ out(regs.C,regs.E); },                           // 0x59: out (c),e
            [&](){ adc_hl_nn(regs.DE); },                           // 0x5a: adc hl,de
            [&](){ ld_rr_ptr_nn(regs.DE,fetch16BitValue()); },      // 0x5b: ld de,(**)
            [&](){ neg(); },                                        // 0x5c: neg
            [&](){ retn(); },                                       // 0x5d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); },      // 0x5e: im 2
            [&](){ ld_a_r(); },                                     // 0x5f: ld a,r

            [&](){ in(regs.H,regs.C); },                            // 0x60 in h,(c)
            [&](){ out(regs.C,regs.H); },                           // 0x61: out (c),h
            [&](){ sbc_hl_nn(regs.HL); },                           // 0x62
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.HL); },      // 0x63 ld (**),hl
            [&](){ neg(); },                                        // 0x64 neg
            [&](){ retn(); },                                       // 0x65: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },      // 0x66: im 0
            [&](){ rrd(); },                                        // 0x67: rrd
            [&](){ in(regs.L,regs.C); },                            // 0x68: in l,(c)
            [&](){ out(regs.C,regs.L); },                           // 0x69: out (c),l
            [&](){ adc_hl_nn(regs.HL); },                           // 0x6a
            [&](){ ld_rr_ptr_nn(regs.HL,fetch16BitValue()); },      // 0x6b: ld hl,(**)
            [&](){ neg(); },                                        // 0x6c: neg
            [&](){ retn(); },                                       // 0x6d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },      // 0x6e: im 0/1
            [&](){ rld(); },                                        // 0x6f: rld

            [&](){ in(regs.C,regs.C,true); },                       // 0x70: in (c)
            [&](){ out(regs.C,0); },                                // 0x71: out
            [&](){ sbc_hl_nn(specialRegs.SP); },                    // 0x72: sbc hl,sp
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),specialRegs.SP); }, // 0x73: ld (**),hl
            [&](){ neg(); },                                        // 0x74: neg
            [&](){ retn(); },                                       // 0x75:
            [&](){ set_interrupt_mode(InterruptMode::IM_1); },      // 0x76: im 1
            [&](){ invalid_opcode(); },                             // 0x77
            [&](){ in(regs.A,regs.C); },                            // 0x78 in a,(c)
            [&](){ out(regs.C,regs.A); },                           // 0x79 out (c),a
            [&](){ adc_hl_nn(specialRegs.SP); },                    // 0x7a adc hl,sp
            [&](){ ld_rr_ptr_nn(specialRegs.SP,fetch16BitValue()); }, // 0x7b ld sp,(**)
            [&](){ neg(); },                                        // 0x7c neg
            [&](){ retn(); },                                       // 0x7d retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); },      // 0x7e im2
            [&](){ },                                               // 0x7f

            // 0x80 - 0x8f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},
            // 0x90 - 0x9f
            [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){}, [&](){},

            [&](){ ldi(); }, // 0xa0: ldi
            [&](){ cpi(); }, // 0xa1:
            [&](){ ini(); }, // 0xa2:
            [&](){ outi(); }, // 0xa3:
            [&](){ }, // 0xa4
            [&](){ }, // 0xa5
            [&](){ }, // 0xa6
            [&](){ }, // 0xa7
            [&](){ ldd(); }, // 0xa8
            [&](){ cpd(); }, // 0xa9
            [&](){ ind(); }, // 0xaa
            [&](){ outd(); }, // 0xab
            [&](){ }, // 0xac
            [&](){ }, // 0xad
            [&](){ }, // 0xae
            [&](){ }, // 0xaf

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

        [&](){}, // 0x20
        [&](){ ld_ix_nn(); }, // 0x21: LD IX,**
        [&](){ ld_ptr_nn_nn(fetch16BitValue(),specialRegs.IX); }, // 0x22: LD (**),IX
        [&](){ inc_ix(); }, // 0x23 inc ix
        [&](){ inc_r(specialRegs.IXH); }, // 0x24
        [&](){ dec_r(specialRegs.IXL); }, // 0x25
        [&](){ ld_ixh_n(fetch8BitValue()); }, // 0x26
        [&](){}, // 0x27
        [&](){}, // 0x28
        [&](){ add16(specialRegs.IX,specialRegs.IX); }, // 0x29
        [&](){ ld_ix_ptr_nn(); }, // 0x2a
        [&](){ dec_ix(); }, // 0x2b
        [&](){ inc_r(specialRegs.IXL); }, // 0x2c
        [&](){ dec_r(specialRegs.IXL); }, // 0x2d
        [&](){ ld_ixl_n(fetch8BitValue()); }, // 0x2e
        [&](){}, // 0x2f

        [&](){}, // 0x30
        [&](){}, // 0x31
        [&](){}, // 0x32
        [&](){}, // 0x33
        [&](){ INC_pIXn(); }, // 0x34
        [&](){ dec_ptr_ix_n(); }, // 0x35
        [&](){ ld_ptr_ix_n_n(); }, // 0x36
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
        [&](){ ld_r_r(regs.B,specialRegs.IXH); }, // 0x44 ld B,IXh
        [&](){ ld_r_r(regs.B,specialRegs.IXL); }, // 0x45
        [&](){ LD_r_pIXn(regs.B); }, // 0x46
        [&](){  }, // 0x47
        [&](){  }, // 0x48
        [&](){  }, // 0x49
        [&](){  }, // 0x4a
        [&](){  }, // 0x4b
        [&](){ ld_r_r(regs.C, specialRegs.IXH); }, // 0x4c
        [&](){ ld_r_r(regs.C, specialRegs.IXL); }, // 0x4d
        [&](){ LD_r_pIXn(regs.C); }, // 0x4e
        [&](){  }, // 0x4f

        [&](){  }, // 0x50
        [&](){  }, // 0x51
        [&](){  }, // 0x52
        [&](){  }, // 0x53
        [&](){ ld_r_r(regs.D, specialRegs.IXH); }, // 0x54
        [&](){ ld_r_r(regs.D, specialRegs.IXL); }, // 0x55
        [&](){ LD_r_pIXn(regs.D); }, // 0x56 LD D,(ix+n)
        [&](){  }, // 0x57
        [&](){  }, // 0x58
        [&](){  }, // 0x59
        [&](){  }, // 0x5a
        [&](){  }, // 0x5b
        [&](){ ld_r_r(regs.E, specialRegs.IXH); }, // 0x5c
        [&](){ ld_r_r(regs.E, specialRegs.IXL); }, // 0x5d
        [&](){ LD_r_pIXn(regs.E); }, // 0x5e LD E,(ix+n)
        [&](){  }, // 0x5f

        [&](){ ld_r_r(specialRegs.IXH,regs.B); }, // 0x60
        [&](){ ld_r_r(specialRegs.IXH,regs.C); }, // 0x61
        [&](){ ld_r_r(specialRegs.IXH,regs.D); }, // 0x62
        [&](){ ld_r_r(specialRegs.IXH,regs.E); }, // 0x63
        [&](){ ld_r_r(specialRegs.IXH,specialRegs.IXH); }, // 0x64
        [&](){ ld_r_r(specialRegs.IXH,specialRegs.IXL); }, // 0x65
        [&](){ LD_r_pIXn(regs.H); }, // 0x66 LD E,(ix+n)
        [&](){ ld_r_r(specialRegs.IXH,regs.A); }, // 0x67
        [&](){ ld_r_r(specialRegs.IXL,regs.B); }, // 0x68
        [&](){ ld_r_r(specialRegs.IXL,regs.C); }, // 0x69
        [&](){ ld_r_r(specialRegs.IXL,regs.D); }, // 0x6a
        [&](){ ld_r_r(specialRegs.IXL,regs.E); }, // 0x6b
        [&](){ ld_r_r(specialRegs.IXL,specialRegs.IXH); }, // 0x6c
        [&](){ ld_r_r(specialRegs.IXL,specialRegs.IXL); }, // 0x6d
        [&](){ LD_r_pIXn(regs.L); }, // 0x6e LD E,(ix+n)
        [&](){ ld_r_r(specialRegs.IXL,regs.A); }, // 0x6f

        [&](){ LD_pIXn_r(regs.B); }, // 0x70
        [&](){ LD_pIXn_r(regs.C); }, // 0x71
        [&](){ LD_pIXn_r(regs.D); }, // 0x72
        [&](){ LD_pIXn_r(regs.E); }, // 0x73
        [&](){ LD_pIXn_r(regs.H); }, // 0x74
        [&](){ LD_pIXn_r(regs.L); }, // 0x75
        [&](){  },                      // 0x76
        [&](){ LD_pIXn_r(regs.B); }, // 0x77
        [&](){  }, // 0x78
        [&](){  }, // 0x79
        [&](){  }, // 0x7a
        [&](){  }, // 0x7b
        [&](){ ld_r_r(regs.A,specialRegs.IXH); }, // 0x7c
        [&](){ ld_r_r(regs.A,specialRegs.IXL); }, // 0x7d
        [&](){ LD_r_pIXn(regs.A); }, // 0x7e
        [&](){  }, // 0x7f

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
        [&](){ ld_r_r(regs.A,specialRegs.IXH); }, // 0x8c
        [&](){ ld_r_r(regs.A,specialRegs.IXL); }, // 0x8d
        [&](){ ld_r_r(regs.A,fetch_pIXn()); }, // 0x8e LD a,(ix+n)
        [&](){  }, // 0x8f

        [&](){  }, // 0x90
        [&](){  }, // 0x91
        [&](){  }, // 0x92
        [&](){  }, // 0x93
        [&](){ sub(specialRegs.IXH); }, // 0x94
        [&](){ sub(specialRegs.IXL); }, // 0x95
        [&](){ sub(fetch_pIXn()); }, // 0x96
        [&](){  }, // 0x97
        [&](){  }, // 0x98
        [&](){  }, // 0x99
        [&](){  }, // 0x9a
        [&](){  }, // 0x9b
        [&](){ sub(specialRegs.IXH,true); }, // 0x9c
        [&](){ sub(specialRegs.IXL,true); }, // 0x9d
        [&](){ sub(fetch_pIXn(),true); }, // 0x9e
        [&](){  }, // 0x9f

        [&](){  }, // 0xa0
        [&](){  }, // 0xa1
        [&](){  }, // 0xa2
        [&](){  }, // 0xa3
        [&](){ and_a_with_value(specialRegs.IXH); }, // 0xa4
        [&](){ and_a_with_value(specialRegs.IXL); }, // 0xa5
        [&](){ and_a_with_value(fetch_pIXn()); }, // 0xa6
        [&](){ }, // 0xa7
        [&](){ }, // 0xa8
        [&](){ }, // 0xa9
        [&](){ }, // 0xaa
        [&](){ }, // 0xab
        [&](){ xor_a_with_value(specialRegs.IXH); }, // 0xac
        [&](){ xor_a_with_value(specialRegs.IXL); }, // 0xad
        [&](){ xor_a_with_value(fetch_pIXn()); }, // 0xae
        [&](){ }, // 0xaf

        [&](){ }, // 0xb0
        [&](){ }, // 0xb1
        [&](){ }, // 0xb2
        [&](){ }, // 0xb3
        [&](){ or_a_with_value(specialRegs.IXH); }, // 0xb4
        [&](){ or_a_with_value(specialRegs.IXL); }, // 0xb5
        [&](){ or_a_with_value(fetch_pIXn()); }, // 0xb6
        [&](){ }, // 0xb7
        [&](){ }, // 0xb8
        [&](){ }, // 0xb9
        [&](){ }, // 0xba
        [&](){ }, // 0xbb
        [&](){ cp_a_with_value( specialRegs.IXH); }, // 0xbc
        [&](){ cp_a_with_value( specialRegs.IXL); }, // 0xbd
        [&](){ cp_a_with_value( fetch_pIXn()); }, // 0xbe
        [&](){ }, // 0xbf

         // 0xc0 - 0xca
        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
        [&](){ decode_ix_bit_instruction(); }, // 0xcb
        [&](){ },[&](){ },[&](){ },[&](){ },

        // 0xd0 - 0xdf
        [&](){ }, [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

        [&](){ }, // 0xe0
        [&](){ pop16(specialRegs.IX); }, // 0xe1
        [&](){ }, // 0xe2
        [&](){ }, // 0xe3 EX (SP),IX
        [&](){ }, // 0xe4
        [&](){ push_ix(); }, // 0xe5
        [&](){ }, // 0xe6
        [&](){ }, // 0xe7
        [&](){ }, // 0xe8
        [&](){ jp_IX(); }, // 0xe9
        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },

        [&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },[&](){ },
        [&](){ ld_rr_rr(specialRegs.SP,specialRegs.IX); }, // 0xf9
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
    // TODO: implement group
    uint8_t op2 = fetch8BitValue();

    if( op2 == 0xCB ){
        decode_ix_bit_instruction();
        return;
    }
}

// opcode: 0xfd ..
void CPU::decode_iy_instruction()
{
    // TODO: implement group
    uint8_t op2 = fetch8BitValue();

    if( op2 == 0xCB ){
        decode_iy_bit_instruction();
        return;
    }
}

void CPU::step()
{

    // M1: OP Code fetch
    currentOpcode = fetch8BitValue();

    if(currentOpcode < instructions.size()  ){
        (this->*instructions[currentOpcode].code)();
        //instructions[currentOpcode].code();

    }

};

bool CPU::has_parity( uint8_t x )
{
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

// Add a number of cycles that the current operation has spent.
// At the end of each `step`, the final number of cycles is used as a delay to wait before running the next `step`.
void add_cycles(uint8_t cycles ){
    // +15 cycles at læse fra (ix+n)
    // +5 hvis ixl/ixh er involveret
}

// *******************************************************
// generic methods
// *******************************************************


void CPU::add( uint8_t srcValue, bool carry ){

    // Page 107 in z80 technical manual.

    uint16_t result = regs.A + srcValue;
    if ( carry && (regs.F & FlagBitmaskC) ) {
        result++;
    }

    setFlag(FlagBitmaskC,result > 0xff);
    setFlag(FlagBitmaskZero, (result&0xff) == 0);
    setFlag(FlagBitmaskHalfCarry, (regs.A & 0x0f) > (result & 0x0f));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskSign, result & 0x80);

    // TODO: test this!
    // if like signs in numbers being added, and result sign is different, set overflow:
    /*
           ADDITION SIGN BITS
    num1sign num2sign ADDsign
   ---------------------------
        0       0       0
 *OVER* 0       0       1 (adding two positives should be positive)
        0       1       0
        0       1       1
        1       0       0
        1       0       1
 *OVER* 1       1       0 (adding two negatives should be negative)
        1       1       1
        */
    setFlag(FlagBitmaskPV, !((regs.A ^ srcValue) & 0x80) && ((result ^ regs.A) & 0x80) );

    regs.A = static_cast<uint8_t>(result);
}

//  add 16 bit register pair
void CPU::add16( uint16_t& regPair, uint16_t value_to_add , bool carry )
{
    uint32_t result = regPair + value_to_add;
    if ( regs.F & FlagBitmaskC ) {
        result++;
    }

    setFlag(FlagBitmaskC,result > 0xffff);
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskHalfCarry, (regPair & 0x0fffu) > (result & 0x0fffu));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskSign, result & 0x8000);
    setFlag(FlagBitmaskPV, !((regPair ^ value_to_add) & 0x8000) && ((result ^ regPair) & 0x8000) );

    regPair = result;
}

/**
 * Subtracts a value from register A
 * @param srcValue the value to subtract
 * @param carry whether to subtract the carry flag
 * @param onlySetFlagsForComparison Only set the flags for usage by the `cp` compare instruction and don't perform the actual subtraction
 */
void CPU::sub( uint8_t srcValue, bool carry, bool onlySetFlagsForComparison ){
    // TODO: finish this
    uint16_t result = (0xff00 | regs.A) - srcValue;
    if ( carry && (regs.F & FlagBitmaskC) ) {
        result--;
    }
    setFlag(FlagBitmaskZero, (result&0xff) == 0);
    setFlag(FlagBitmaskN,true);
    setFlag(FlagBitmaskC, ((result&0xff00) < 0xff00) );
    setFlag(FlagBitmaskHalfCarry, (0xfff0 & regs.A) < (result & 0xfff0) );
    setFlag(FlagBitmaskSign, result & 0x80 );
 /*

  http://teaching.idallen.com/dat2343/11w/notes/040_overflow.txt

      SUBTRACTION SIGN BITS
    num1sign num2sign SUBsign
   ---------------------------
        0       0       0
        0       0       1
        0       1       0
 *OVER* 0       1       1 (subtract negative is same as adding a positive)
 *OVER* 1       0       0 (subtract positive is same as adding a negative)
        1       0       1
        1       1       0
        1       1       1
  */
    bool isOperandsSignBitDifferent = (regs.A ^ srcValue) & 0x80;
    bool didChangeSignInResult = (regs.A ^ result) & 0x80;
    setFlag( FlagBitmaskPV, isOperandsSignBitDifferent && didChangeSignInResult );

    if(!onlySetFlagsForComparison){
        regs.A = static_cast<uint8_t>(result);
    }
}

void CPU::sub16( uint16_t& regPair, uint16_t value_to_sub , bool carry )
{
    uint32_t result = (0xffff0000 | regs.HL) - value_to_sub;
    if ( regs.F & FlagBitmaskC ) {
        result--;
    }
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskN,true);
    setFlag(FlagBitmaskC, ((result&0xffff0000) < 0xffff0000) );
    setFlag(FlagBitmaskHalfCarry, (0xfffff000 & regs.HL) < (result & 0xfffff000) );
    setFlag(FlagBitmaskSign, result & 0x8000 );

    bool isOperandsSignBitDifferent = (regs.HL ^ value_to_sub) & 0x80;
    bool didChangeSignInResult = (regs.HL ^ result) & 0x80;
    setFlag( FlagBitmaskPV, isOperandsSignBitDifferent && didChangeSignInResult );

    regs.HL = result;
}

// SBC HL,nn
// opcode: 0xed 01ss0010
void CPU::sbc_hl_nn( uint16_t value ){
    sub16(regs.HL,value,true);
}

void CPU::ld_r_r()
{
    uint8_t dstRegCode = (currentOpcode >> 3) & 0b111;
    uint8_t srcRegCode = currentOpcode & 0b111;

    if(srcRegCode == RegisterCode::HLPtr)
    {
        // LD R,(HL)
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = mem[regs.HL]; // Get data from HL location
    }
    if(dstRegCode == RegisterCode::HLPtr)
    {
        // LD (HL),R
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        mem[regs.HL] = *srcRegPtr;
    }
    else
    { // LD R,R
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = *srcRegPtr;
    }
}

// cycles: 4 (8 when src or dst is an index register: ixl,ixh,iyl,iyh
void CPU::ld_r_r( uint8_t& dstReg, uint8_t value )
{
    dstReg = value;
}

void CPU::ld_rr_rr( uint16_t& dstReg, uint16_t& value )
{
    dstReg = value;
}

void CPU::inc_r(uint8_t &reg)
{
    reg++;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x80 ); // Set if B was 0x7f before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, false); // reset
}

void CPU::dec_r(uint8_t &reg)
{
    reg--;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x7f ); // Set if B was 0x80 before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0b00001111 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, true); // set
}

void CPU::out( uint8_t dstPort, uint8_t value )
{
    output_to_port(dstPort,value);
}

void CPU::in( uint8_t& dstReg , uint8_t portNumber, bool only_set_flags )
{
    uint8_t value = input_from_port(portNumber);
    if(!only_set_flags){
        dstReg = value;
    }
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV, has_parity(value));
    setFlag(FlagBitmaskZero,value == 0);
    setFlag(FlagBitmaskSign, value & 0x80);
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
    regs.C = fetch8BitValue();
    regs.B = fetch8BitValue();
};

// ld (bc),a
// Opcode: 02
// Cycles: 7
void CPU::load_bc_a(){
    mem[regs.BC] = regs.A;
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
    regs.B = fetch8BitValue();
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
    add16(regs.HL,regs.BC);
}

// LD A,(BC)
// opcode: 0x0A
// cycles: 7
// flags: -
void CPU::load_a_ptr_bc(){
    regs.A = mem[regs.BC];
}

// DEC BC
// opcode: 0x0b
// cycles: 6
void CPU::dec_bc(){
    regs.BC--;
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
    regs.C = fetch8BitValue();
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
        int8_t jumpOffset = fetch8BitValue();
        specialRegs.PC-=2; // measure from the start of this instruction opcode
        specialRegs.PC += jumpOffset;
    }
}

// ld de,nn          
// opcode: 11 n  n
// cycles: 10
void CPU::load_de_nn(){
    regs.E = fetch8BitValue();
    regs.D = fetch8BitValue();
};

// load (de),a
// opcode: 0x12
// cycles: 7
void CPU::load_ptr_de_a(){
    mem[regs.DE] = regs.A;
}

// LD a,i - load the interrupt vector register I into A
// opcode: 0xed 0x57
// cycles: 9
void CPU::ld_a_i()
{
    regs.A = specialRegs.I;
    regs.F &= 0b00000001;
    // Bits affected:
    setFlag(FlagBitmaskSign,specialRegs.I & 0x80);
    setFlag(FlagBitmaskZero, specialRegs.I == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV,IFF2); // P/V: contents of IFF2
    setFlag(FlagBitmaskN,0);
}

// Load A,R - Loads memory refresh register R into A
// opcode: 0xed 0x5f
// cycles: 9
void CPU::ld_a_r()
{
    regs.A = specialRegs.R;
    setFlag(FlagBitmaskSign, specialRegs.R & 0x80);
    setFlag(FlagBitmaskZero, specialRegs.R == 0);
    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskPV, IFF2); // "If an interrupt occurs during execution of this instruction, the Parity flag will contain a 0" (p.51 Z80 tech ref)
    setFlag(FlagBitmaskN,0);
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
    regs.D = fetch8BitValue();
}

// opcode: 0x17
// cycles: 4
void CPU::rla(){
    // rotate left and set carry 
    // Set bit 0 to previous carry.
    uint8_t newcarry = regs.A & 0x80;
    regs.A <<= 1;
    regs.A |= (regs.F&FlagBitmaskC);
    setFlag(FlagBitmaskHalfCarry,false);
    setFlag(FlagBitmaskN,false);
    setFlag(FlagBitmaskC,newcarry);
}

// jr n
// opcode: 0x18
// cycles: 12
void CPU::jr_n(){
    int8_t jumpOffset = fetch8BitValue();
    specialRegs.PC -= 2; // start calculation from beginning of this instruction
    specialRegs.PC += jumpOffset;
}
// add hl,de
// opcode: 0x19
// cycleS: 11
// flag: C N H
void CPU::add_hl_de() {
    add16(regs.HL,regs.DE);
}

// LD A,(DE)
// opcode: 0x1a
// cycles: 7
// flags: -
void CPU::load_a_ptr_de(){
    regs.A = mem[regs.DE];
}

// DEC DE
// opcode: 0x1b
// cycles: 6
void CPU::dec_de(){
    regs.DE--;
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
    regs.E = fetch8BitValue();
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
    int8_t jumpOffset = fetch8BitValue();
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
    regs.L = fetch8BitValue();
    regs.H = fetch8BitValue();
}

// LD (NN),HL
// opcode: 0x22
// flags: -
void CPU::ld_ptr_nn_hl(){
    uint16_t addr = fetch16BitValue();
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
    regs.H = fetch8BitValue();
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
    int8_t offset = fetch8BitValue();
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
    add16(regs.HL,regs.HL);
}

// ld hl,(nn)
// opcode: 0x2a
// cycles: 16
// flags: -
void CPU::ld_hl_ptr_nn(){
    uint16_t addr = fetch16BitValue();
    regs.L = mem[addr];
    regs.H = mem[addr+1];
}

// dec hl
// opcode: 0x2b
// cycles: 6
// flags: -
void CPU::dec_hl(){
    regs.HL--;
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
    regs.L = fetch8BitValue();
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

// JR NC - Jump if not carry
// opcode: 0x30
void CPU::jr_nc()
{
    int8_t offset = fetch8BitValue();
    if(!(regs.F & FlagBitmaskC)){ // If zero
        specialRegs.PC -= 2; // start calculation from beginning of this instruction
        specialRegs.PC += offset;
    }
}

// LD SP,NN
// opcode: 0x31
// flags: -
void CPU::ld_sp_nn()
{
    specialRegs.SP = fetch16BitValue();
}

// LD (NN),A
// opcode: 0x32
// flags: -
void CPU::ld_ptr_nn_a()
{
    mem[fetch16BitValue()] = regs.A;
}

void CPU::ld_ptr_nn_n( uint16_t location, uint8_t value )
{
    mem[location] = value;
}

// cycles: 20
// flags: -
void CPU::ld_ptr_nn_nn( uint16_t location, uint16_t value)
{
    mem[location] = static_cast<uint8_t>(value);
    mem[location+1] = value >> 8;
}

// LD rr,(nn)
// cycles: 20
// flags: -
void CPU::ld_rr_ptr_nn( uint16_t& regPair, uint16_t addr )
{
    uint16_t value = mem[addr] + (mem[addr+1]<<8);
    regPair = value;
}

// INC SP
// opcode: 0x33
// flags: -
void CPU::inc_sp()
{
    specialRegs.SP++;

}

// INC (HL)
// opcode: 0x34
// N PV H S Z
void CPU::inc_ptr_hl()
{
    uint8_t result = mem[regs.HL]++;
    setFlag(FlagBitmaskSign, result & 0x80);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00010000 );
    setFlag(FlagBitmaskPV, result ==  0x80);
    setFlag(FlagBitmaskN,0);
}

// DEC (HL)
// opcode: 0x35
void CPU::dec_ptr_hl()
{
    uint8_t result = mem[regs.HL]--;
    setFlag(FlagBitmaskSign, result & 0x80);
    setFlag(FlagBitmaskZero, result == 0);
    setFlag(FlagBitmaskHalfCarry, (result & 0b00011111) == 0b00001111 );
    setFlag(FlagBitmaskPV, result ==  0x7f);
    setFlag(FlagBitmaskN,1);
}

// LD (HL),N
// opcode: 0x36
void CPU::ld_ptr_hl_n()
{
    mem[regs.HL] = fetch8BitValue();
}

// SCF - Set carry flag
// opcode: 0x37
// cycles: 4
// flags: C H N
void CPU::scf()
{
    setFlag(FlagBitmaskC,true);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskN,0);
}

// JR C - jump if carry flag is set
// opcode: 0x38
// flags: -
void CPU::jr_c()
{
    int8_t offset = fetch8BitValue();
    if((regs.F & FlagBitmaskC)){ // If zero
        specialRegs.PC -= 2; // start calculation from beginning of this instruction
        specialRegs.PC += offset;
    }
}

// ADD HL,SP
// opcode: 0x39
// cycles: 11
// flags: C N H
void CPU::add_hl_sp()
{
    add16(regs.HL,specialRegs.SP);
}

// LD A,(NN)
// opcode: 0x3a
void CPU::ld_a_ptr_nn()
{
    regs.A = mem[fetch16BitValue()];
}

// DEC SP
// opcode: 0x3b
void CPU::dec_sp()
{
    specialRegs.SP--;
}

// opcode: 0x3c
// flags: -
void CPU::inc_a()
{
    inc_r(regs.A);
}

// opcode: 0x3d
void CPU::dec_a()
{
    dec_r(regs.A);
}

// opcode: 0x3e
// flags: -
void CPU::ld_a_n()
{
    regs.A = fetch8BitValue();
}

// CCF - invert carry flag
// opcode: 0x3f
// cycles: 4
// flags: C N H
void CPU::ccf()
{
    setFlag(FlagBitmaskHalfCarry, regs.F & FlagBitmaskC);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,!(regs.F & FlagBitmaskC));
}

void CPU::halt(){
    // TODO: implement this
}

// ADD a,r
// cycles: 4
// flags: s z h pv n c
void CPU::add_a_r(){
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);

    add(srcRegValue,false);
}

// cycles: 7
void CPU::add_a_n(){
    uint8_t srcRegValue = fetch8BitValue();
    add(srcRegValue,false);
}

void CPU::adc_a_r(){
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);

    add(srcRegValue,true);
}

// ADC A,N
// opcode: 0xce
// cycles: 7
void CPU::adc_a_n(){
    add(fetch8BitValue(),true);
}

void CPU::sub_r(){
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);
    sub(srcRegValue,false, false);
}

// SUB N
// opcode: 0xd6
// cycles: 7
void CPU::sub_n(){
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue,false, false);
}

void CPU::sbc_r(){
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);
    sub(srcRegValue,true, false);
}

// SBC N
// opcode: 0xde
// cycles: 7
void CPU::sbc_n(){
    uint8_t srcRegValue = fetch8BitValue();
    sub(srcRegValue,true, false);
}


void CPU::and_a_with_value(uint8_t value )
{
    regs.A &= value;
    set_AND_operation_flags();
}

// cycles: 4
void CPU::and_r()
{
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);
    regs.A &= srcRegValue;
    set_AND_operation_flags();
}

void CPU::and_n()
{
    regs.A &= fetch8BitValue();
    set_AND_operation_flags();
}

// cycles:
// xor r 4
// xor n 7
// xor (hl) 7
// xor (IX+d) 19
// xor (IY+d) 19
void CPU::xor_r()
{
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);

    xor_a_with_value(srcRegValue);
}

// opcode: 0xee
// cycles: 7
void CPU::xor_n()
{
    xor_a_with_value(fetch8BitValue());
}

void CPU::xor_a_with_value( uint8_t value )
{
    regs.A ^= value;

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
}

// cycles:
// or r 4
// or n 7
// or (hl) 7
// or (IX+d) 19
// or (IY+d) 19
void CPU::or_r()
{
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);

    or_a_with_value(srcRegValue);
}

// OR N
// opcode: 0xf6
// cycles: 7
void CPU::or_n()
{
    or_a_with_value(fetch8BitValue());
}

void CPU::or_a_with_value(uint8_t value)
{
    regs.A |= value;

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,0);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
}

void CPU::cp_a_with_value( uint8_t value )
{
    sub(value,false, true);
}

void CPU::cp_r()
{
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);
    sub(srcRegValue,false, true);
}

// CP N
// Compare A with N by internally doing a sub but only setting flags
// opcode: 0xfe
// cycles: 7
void CPU::cp_n()
{
    sub(fetch8BitValue(),false, true);
}

// cycles: 11 (true) 5 (false)
void CPU::ret_cc(){

    uint8_t conditionCode = (currentOpcode & 0b00111000) >> 3;
    bool conditionValue = false;

    switch (conditionCode)
    {
        case 0: conditionValue = !(regs.F & FlagBitmaskZero); break; // Non zero
        case 1: conditionValue = (regs.F & FlagBitmaskZero);  break; // zero
        case 2: conditionValue = !(regs.F & FlagBitmaskC);  break; // non carry
        case 3: conditionValue = (regs.F & FlagBitmaskC);  break; // carry
        case 4: conditionValue = !(regs.F & FlagBitmaskPV);  break; // parity odd
        case 5: conditionValue = (regs.F & FlagBitmaskPV);  break; // parity even
        case 6: conditionValue = !(regs.F & FlagBitmaskSign);  break; // parity even
        case 7: conditionValue = (regs.F & FlagBitmaskSign);  break; // parity even
        default: break;
    }

    if(conditionValue){
        uint8_t lobyte = mem[specialRegs.SP++];
        uint8_t hibyte = mem[specialRegs.SP++];
        specialRegs.PC = (hibyte<<8) + lobyte;
    }
}

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
    uint8_t regPairCode = (currentOpcode & 0b00110000) >> 4;
    switch (regPairCode)
    {
        case 0:
            pop16(regs.BC);
            break;
        case 1:
            pop16(regs.DE);
            break;
        case 2:
            pop16(regs.HL);
            break;
    }
};

// cycles: 10
void CPU::jp_cc_nn()
{
    uint8_t conditionCode = (currentOpcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)){
        specialRegs.PC = location;
    }
}

// cycles: 10
void CPU::jp_nn()
{
    specialRegs.PC = fetch16BitValue();
}

void CPU::jp_IX()
{
    specialRegs.PC = specialRegs.IX;
}

void CPU::jp_IY()
{
    specialRegs.PC = specialRegs.IY;
}

// JP HL
// opcode: 0xe9
// cycles: 4
// flags: -
void CPU::jp_ptr_hl(){
    specialRegs.PC = regs.HL;
}


// cycles: 17 (true, 10 (false)
void CPU::call_cc_nn()
{
    uint8_t conditionCode = (currentOpcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)){
        mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
        mem[--specialRegs.SP] = static_cast<uint8_t >(specialRegs.PC);      // (SP-2) = PC_h
        specialRegs.PC = location;
    }
}

// opcode: 0xCD
// cycles: 17
void CPU::call(){
    uint16_t location = fetch16BitValue();
    mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
    mem[--specialRegs.SP] = static_cast<uint8_t>(specialRegs.PC);      // (SP-2) = PC_h
    specialRegs.PC = location;
}

// cyckes; 11
void CPU::push_bc(){
    mem[--specialRegs.SP] = regs.B;
    mem[--specialRegs.SP] = regs.C;
}

// cyckes; 11
void CPU::push_de(){
    mem[--specialRegs.SP] = regs.D;
    mem[--specialRegs.SP] = regs.E;
}

// cyckes; 11
void CPU::push_hl(){
    mem[--specialRegs.SP] = regs.H;
    mem[--specialRegs.SP] = regs.L;
}

// cyckes; 11
void CPU::push_af(){
    mem[--specialRegs.SP] = regs.A;
    mem[--specialRegs.SP] = regs.F;
}

// cycles 15
void CPU::push_ix(){
    mem[--specialRegs.SP] = specialRegs.IXH;
    mem[--specialRegs.SP] = specialRegs.IXL;
}

void CPU::push_iy(){
    mem[--specialRegs.SP] = specialRegs.IYH;
    mem[--specialRegs.SP] = specialRegs.IYL;
}


// RST
//
// current PC is pushed onto stack, and PC is reset to an offset based t,
// where t is contained in the opcode: (11 ttt 111).
//
// cycles: 11
void CPU::rst()
{
    uint8_t location[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};
    uint8_t locationCode = (currentOpcode & 0b00111000) >> 3;

    mem[--specialRegs.SP] = specialRegs.PC >> 8;
    mem[--specialRegs.SP] = static_cast<uint8_t>(specialRegs.PC);

    specialRegs.PC = location[locationCode];
}

// RET
// PC_l = (SP), PC_h = (SP+1)
// cycles: 10
void CPU::ret()
{
    uint8_t lobyte = mem[specialRegs.SP++];
    uint8_t hibyte = mem[specialRegs.SP++];
    specialRegs.PC = (hibyte<<8) + lobyte;
}

// OUT (N),A
// opcode: 0xD3
// cycles: 11
void CPU::out_n_a(){
    out(fetch8BitValue(),regs.A);
}

// IN A,(N)
// opcode: 0xDB
// flags: -
// cycles: 11
void CPU::in_a_n(){
    in(regs.A, fetch8BitValue());
}

// EXX
// exchange BC, DE, HL registers with aux versions
// opcode: 0xd9
// cycles: 4
void CPU::exx()
{
    std::swap(regs.B,auxRegs.B);
    std::swap(regs.C,auxRegs.C);

    std::swap(regs.D,auxRegs.D);
    std::swap(regs.E,auxRegs.E);

    std::swap(regs.H,auxRegs.H);
    std::swap(regs.L,auxRegs.L);
}

// EX (SP),HL
// opcode: 0xe3
// cycles: 19
// flags: -
void CPU::EX_pSP_HL()
{
    std::swap( regs.L , mem[specialRegs.SP] );
    std::swap( regs.H , mem[specialRegs.SP+1] );
}

// EX DE,HL
// opcode: 0xEB
// cycles: 4
// flags: -
void CPU::ex_de_hl()
{
    std::swap( regs.D, regs.H);
    std::swap( regs.E, regs.L);
}

void CPU::disable_interrupts()
{
    // TODO: disable interrupts
}

void CPU::enable_interrupts()
{
    // TODO: enable interrupts
}

// LD SP,HL
// opcode: 0xf9
// flags: -
// cycles: 6
void CPU::ld_sp_hl()
{
    specialRegs.SP = regs.HL;
}

// NEG : A = 0-A
// opcode: 0xed 0x44
// flags S Z H PV N C
// cycles: 8
void CPU::neg()
{
    uint8_t result = 0-regs.A;
    setFlag(FlagBitmaskPV,regs.A == 0x80);
    setFlag(FlagBitmaskC,regs.A != 0);
    setFlag(FlagBitmaskHalfCarry, (0xf0 & regs.A) < (result & 0xf0) );
    setFlag(FlagBitmaskSign, result & 0x80);
    setFlag(FlagBitmaskZero, result == 0);
    regs.A = result;
}

// RETN - "used at the end of a non-maskable interrupt rountine (located at 0x0066)"
// opcode: 0xed 0x45
void CPU::retn()
{
    ret();
    IFF = IFF2;

}

void CPU::reti()
{
    ret();
}

void CPU::adc_hl_nn(uint16_t value)
{
    add16(regs.HL,value,true);
}

// RRD - do some swapping of nibbles between A and (HL) (see details on p.219 z80 tech ref.)
// opcode: 0xed 0x67
// cycles: 18
// flags: s z h pv n
void CPU::rrd()
{
    uint8_t data = mem[regs.HL];
    uint8_t old_data_lownib = data & 0xf;
    data = data >> 4;
    data |= (regs.A << 4);
    regs.A &= 0xf0;
    regs.A |= old_data_lownib;
    mem[regs.HL] = data;
    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,false);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,false);
}

// RLD - do some swapping of nibbles between A and (HL) (see details on p.217 z80 tech ref.)
// opcode: 0xed 0x6f
// cycles: 18
// flags: s z h pv n
void CPU::rld()
{
    uint16_t hl = regs.HL;
    uint8_t data = mem[hl];
    uint8_t old_data_hinib = data & 0xf0;
    data = data << 4;
    data |= (regs.A & 0x0f);
    regs.A &= 0xf0;
    regs.A |= (old_data_hinib>>4);
    mem[hl] = data;
    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,false);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,false);
}

// LDI
// opcode: 0xed 0xa0
// cycles: 16
// flags: H PV N
void CPU::ldi( bool decrease, bool repeat )
{
    if(regs.BC > 0)
    {
        do
        {
            mem[regs.DE] = mem[regs.HL]; // (de) = (hl)

            if (decrease)
            {
                regs.DE--;
                regs.HL--;
            } else
            {
                regs.DE++;
                regs.HL++;
            }

            regs.BC--;

        } while (repeat && regs.BC > 0);
    }

    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN, 0);
    setFlag(FlagBitmaskPV, regs.BC != 0);
}

void CPU::ldir()
{
    ldi(false,true);
}

void CPU::ldd()
{
    ldi(true,false);
}

void CPU::lddr()
{
    ldi(true,true);
}

// CPI
// opcode: 0xed 0xa1
// cycles: 16
void CPU::cpi( bool decrease, bool repeat )
{
    if( regs.BC > 0)
    {
        do
        {
            uint8_t result = regs.A - mem[regs.HL];

            if(decrease){
                regs.HL--;
            } else {
                regs.HL++;
            }

            regs.BC--;

            setFlag(FlagBitmaskSign,result & 0x80);
            setFlag(FlagBitmaskZero, result == 0);
            setFlag(FlagBitmaskHalfCarry, (regs.A & 0xf0) < (result & 0xf0) );
        }
        while (repeat && regs.BC > 0);
    }

    setFlag(FlagBitmaskPV,regs.BC == 0);
    setFlag(FlagBitmaskN,1);
}

void CPU::cpir()
{
    cpi(false,true);
}

void CPU::cpd()
{
    cpi(true,false);
}

void CPU::cpdr()
{
    cpi(true,true);
}

// INI
// opcode: 0xed 0xa2
// cycles: 16
void CPU::ini( bool decrease , bool repeat )
{
    if( regs.B > 0)
    {
        do{
            uint8_t value = input_from_port(regs.C);
            mem[regs.HL] = value;

            regs.B--;

            if(decrease){
                regs.HL--;
            } else {
                regs.HL++;
            }
        } while ( repeat && regs.B > 0);
    }

    setFlag(FlagBitmaskZero,regs.B==0);
    setFlag(FlagBitmaskN,true);
};

void CPU::inir()
{
    ini(false,true);
}

void CPU::ind()
{
    ini(true,false);
}

void CPU::indr()
{
    ini(true,true);
}

// OUTI
// opcode: 0xed 0xa3
// cycles: 16
void CPU::outi( bool decrease, bool repeat )
{
    if( regs.B > 0)
    {
        do {
            output_to_port(regs.C, mem[regs.HL]);
            regs.B--;
            if(decrease){
                regs.HL--;
            } else {
                regs.HL++;
            }

        } while (repeat && regs.B > 0);
    }

    setFlag(FlagBitmaskZero, regs.B==0 );
    setFlag(FlagBitmaskN,true);
}

void CPU::otir()
{
    outi(false,true);
}

void CPU::outd()
{
    outi(true,false);
}

void CPU::otdr()
{
    outi(true,true);
}

// LD IX,nn
// opcode: 0xdd 0x21
// cycles: 14
void CPU::ld_ix_nn()
{
    specialRegs.IX = fetch16BitValue();
}

// LD IX,nn
// opcode: 0xfd 0x21
// cycles: 14
void CPU::ld_iy_nn()
{
    specialRegs.IY = fetch16BitValue();
}

// LD IX,(nn)
// opcode: 0xdd 0x2a
// cycles: 20
// flags: -
void CPU::ld_ix_ptr_nn()
{
    uint16_t addr = fetch16BitValue();
    specialRegs.IXH = mem[addr+1];
    specialRegs.IXL = mem[addr];
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
}

// INC IX
// cycles: 10
void CPU::inc_ix()
{
    specialRegs.IX++;
}

// DEC IX
// cycles: 10
void CPU::dec_ix()
{
    specialRegs.IX--;
}

// INC IY
// cycles: 10
void CPU::inc_iy()
{
    specialRegs.IY++;
}

// DEC IY
// cycles: 10
void CPU::dec_iy()
{
    specialRegs.IY--;
}

// LD IXH,n
// cycles: 8
void CPU::ld_ixh_n( uint8_t value )
{
    specialRegs.IXH = value;
}

// LD IXL,n
// cycles: 8
void CPU::ld_ixl_n( uint8_t value )
{
    specialRegs.IXL = value;
}

// LD IYH,n
// cycles: 8
void CPU::ld_iyh_n( uint8_t value )
{
    specialRegs.IYH = value;
}

// LD IYL,n
// cycles: 8
void CPU::ld_iyl_n( uint8_t value )
{
    specialRegs.IYL = value;
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::ld_ptr_ix_n_n()
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    mem[specialRegs.IX + offset] = fetch8BitValue();
}

// LD (IX+d),n
// cycles: 19
// flags: -
void CPU::LD_pIXn_r(uint8_t& reg )
{
    auto ptr = fetch_pIXn();
    ptr = reg;
}

// LD r,(ix+n)
// cycles: 19
void CPU::LD_r_pIXn(uint8_t& dst_reg )
{
    dst_reg = fetch_pIXn();
}

// LD r,(iy+n)
// cycles: 19
void CPU::LD_r_pIYn( uint8_t& dst_reg )
{
    auto offset = static_cast<int8_t>(fetch8BitValue());
    dst_reg = mem[specialRegs.IY + offset];
}


// INC (ix+n)
// cycles: 23
void CPU::INC_pIXn()
{
    uint8_t& location = fetch_pIXn();
    setFlag(FlagBitmaskPV,location == 0x7f); // set if location was 0x7f before inc operation
    location++;
    setFlag(FlagBitmaskSign,location & 0x80);
    setFlag(FlagBitmaskZero,location == 0);
    setFlag(FlagBitmaskHalfCarry, (location & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag(FlagBitmaskN,false);
}

// DEC (ix+n)
// cycles: 23
void CPU::dec_ptr_ix_n()
{
    uint8_t& location = fetch_pIXn();
    setFlag(FlagBitmaskPV,location == 0x80); // set if location was 0x80 before dec operation
    uint8_t oldvalue = location;
    location--;
    setFlag(FlagBitmaskSign,location & 0x80);
    setFlag(FlagBitmaskZero,location == 0);
    setFlag(FlagBitmaskHalfCarry, (location & 0b00001111) == 0b00001111 ); // Set if carry over from bit 4
    setFlag(FlagBitmaskN,true);
}


void CPU::invalid_opcode()
{
    // TODO: maybe log this?
}
