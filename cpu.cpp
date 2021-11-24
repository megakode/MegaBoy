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
    using OP = std::function<void()>;
    std::vector<OP> extended_opcodes = {

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
            [&](){ sbc_hl_nn(regs.value_in_bc()); },
            // 0x43: ld (**),bc
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.value_in_bc()); },
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
            [&](){ adc_hl_nn(regs.value_in_bc()); },
            // 0x4B: LD BC,(NN)
            [&](){ uint16_t addr = fetch16BitValue(); regs.C = mem[addr]; regs.B = mem[addr+1]; },
            // 0x4C: neg
            [&](){ neg(); },
            // 0x4d:
            [&](){ reti(); },
            // 0x4e: set undefined IM0/1 mode
            [&](){ set_interrupt_mode(InterruptMode::IM_0); },
            // 0x4f: ld r,a
            [&](){  specialRegs.R = regs.A; },

            [&](){ in(regs.D,regs.C); }, // 0x50: in d,(c)
            [&](){ out(regs.C,regs.D); }, // 0x51: out (c),d
            [&](){ sbc_hl_nn(regs.value_in_de()); }, // 0x52: sbc hl,de
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.value_in_de()); }, // 0x53: ld (**),de
            [&](){ neg(); }, // 0x54: neg
            [&](){ retn(); }, // 0x55: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_1); }, // 0x56: im 1
            [&](){ regs.A = specialRegs.I; }, // 0x57: ld a,i
            [&](){ in(regs.E,regs.C); }, // 0x58: in e,(c)
            [&](){ out(regs.C,regs.E); }, // 0x59: out (c),e
            [&](){ adc_hl_nn(regs.value_in_de()); }, // 0x5a: adc hl,de
            [&](){ uint16_t addr = fetch16BitValue(); regs.E = mem[addr]; regs.D = mem[addr+1]; }, // 0x5b: ld de,(**)
            [&](){ neg(); }, // 0x5c: neg
            [&](){ retn(); }, // 0x5d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); }, // 0x5e: im 2
            [&](){ regs.A = specialRegs.R; }, // 0x5f: ld a,r

            [&](){ in(regs.H,regs.C); }, // 0x60 in h,(c)
            [&](){ out(regs.C,regs.H); }, // 0x61: out (c),h
            [&](){ sbc_hl_nn(regs.value_in_hl()); }, // 0x62
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),regs.value_in_hl()); }, // 0x63 ld (**),hl
            [&](){ neg(); }, // 0x64 neg
            [&](){ retn(); }, // 0x65: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); }, // 0x66: im 0
            [&](){ rrd(); }, // 0x67: rrd
            [&](){ in(regs.L,regs.C); }, // 0x68: in l,(c)
            [&](){ out(regs.C,regs.L); }, // 0x69: out (c),l
            [&](){ adc_hl_nn(regs.value_in_hl()); }, // 0x6a
            [&](){ uint16_t addr = fetch16BitValue(); regs.L = mem[addr]; regs.H = mem[addr+1]; }, // 0x6b: ld hl,(**)
            [&](){ neg(); }, // 0x6c: neg
            [&](){ retn(); }, // 0x6d: retn
            [&](){ set_interrupt_mode(InterruptMode::IM_0); }, // 0x6e: im 0/1
            [&](){ rld(); }, // 0x6f: rld

            [&](){ in(regs.C,regs.C,true); }, // 0x70: in (c)
            [&](){ out(regs.C,0); }, // 0x71: out
            [&](){ sbc_hl_nn(specialRegs.SP); }, // 0x72: sbc hl,sp
            [&](){ ld_ptr_nn_nn(fetch16BitValue(),specialRegs.SP); }, // 0x73: ld (**),hl
            [&](){ neg(); }, // 0x74: neg
            [&](){ retn(); }, // 0x75:
            [&](){ set_interrupt_mode(InterruptMode::IM_1); }, // 0x76: im 1
            [&](){ invalid_opcode(); }, // 0x77
            [&](){ in(regs.A,regs.C); }, // 0x78 in a,(c)
            [&](){ out(regs.C,regs.A); }, // 0x79 out (c),a
            [&](){ adc_hl_nn(specialRegs.SP); }, // 0x7a adc hl,sp
            [&](){ uint16_t addr = fetch16BitValue(); specialRegs.SP = mem[addr]; }, // 0x7b ld sp,(**)
            [&](){ neg(); }, // 0x7c neg
            [&](){ retn(); }, // 0x7d retn
            [&](){ set_interrupt_mode(InterruptMode::IM_2); }, // 0x7e im2
            [&](){ }, // 0x7f

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
        {"BIT opcode group",&CPU::bit_instruction_group}, // 0xcb
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
        {"IX opcode group",&CPU::ix_instruction_group}, // 0xdd
        {"SBC N",&CPU::sbc_n},              // 0xde
        {"RST 18h",&CPU::rst},              // 0xdf

        {"RET PO",&CPU::ret_cc},            // 0xe0
        {"POP HL",&CPU::pop_qq},            // 0xe1
        {"JP PO,NN",&CPU::jp_cc_nn},        // 0xe2
        {"EX (SP),HL",&CPU::ex_ptr_sp_hl},  // 0xe3
        {"CALL PO,NN",&CPU::call_cc_nn},    // 0xe4
        {"PUSH HL",&CPU::push_hl},          // 0xe5
        {"AND N",&CPU::and_n},              // 0xe6
        {"RST 20h",&CPU::rst},              // 0xe7
        {"RET PE",&CPU::ret_cc},            // 0xe8
        {"JP (HL)",&CPU::jp_ptr_hl},        // 0xe9
        {"JP PE,NN",&CPU::jp_cc_nn},        // 0xea
        {"EX DH,HL",&CPU::ex_de_hl},        // 0xeb
        {"CALL PE,NN",&CPU::call_cc_nn},    // 0xec
        {"Extended instruction group",&CPU::extended_instruction_group}, // 0xed
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
        {"IY instruction group",&CPU::iy_instruction_group}, // 0xfd
        {"CP N",&CPU::cp_n},                // 0xfe
        {"RST 38h",&CPU::rst}               // 0xff

    };

}


// opcode: 0xed
void CPU::extended_instruction_group()
{
    // TODO: implement group
    uint8_t op2 = fetch8BitValue();

    // Load A,I
    if(op2 == 0x57){
        regs.A = specialRegs.I;
        regs.F &= 0b00000001;
        // Bits affected:
        setFlag(FlagBitmaskSign,specialRegs.I & 0x80);
        setFlag(FlagBitmaskZero, specialRegs.I == 0);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV,IFF2); // P/V: contents of IFF2
        setFlag(FlagBitmaskN,0);
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
        uint16_t addr = fetch16BitValue();
        regs.C = mem[addr];
        regs.B = mem[addr+1];
    } else

        // Load DE,(nn) - M:6 T:20
    if(op2==0b01011011){
        uint16_t addr = fetch16BitValue();
        regs.E = mem[addr];
        regs.D = mem[addr+1];
    } else

        // Load HL,(nn) - M:6 T:20
    if(op2==0b01101011){
        uint16_t addr = fetch16BitValue();
        regs.L = mem[addr];
        regs.H = mem[addr+1];
    } else

        // Load SP,(nn) - M:6 T:20
    if(op2==0b01111011){
        uint16_t addr = fetch16BitValue();
        uint8_t lowbyte = mem[addr];
        uint16_t hibyte = mem[addr+1];
        specialRegs.SP = (hibyte<<8) + lowbyte;
    } else

        // Load (nn),BC - M:6 T:20
    if(op2==0b01000011){
        uint16_t addr = fetch16BitValue();
        mem[addr] = regs.C;
        mem[(uint16_t)(addr+1)] = regs.B;
    }else

        // Load (nn),DE - M:6 T:20
    if(op2==0b01010011){
        uint16_t addr = fetch16BitValue();
        mem[addr] = regs.E;
        mem[(uint16_t)(addr+1)] = regs.D;
    }else

        // Load (nn),HL - M:6 T:20
    if(op2==0b01100011){
        uint16_t addr = fetch16BitValue();
        mem[addr] = regs.L;
        mem[(uint16_t)(addr+1)] = regs.H;
    }

    // Load (nn),SP - M:6 T:20
    if(op2==0b01110011){
        uint16_t addr = fetch16BitValue();
        mem[addr] = (uint8_t)specialRegs.SP;
        mem[(uint16_t)(addr+1)] = (specialRegs.SP>>8);
    }
}

// opcodes: 0xCB ..

void CPU::bit_instruction_group()
{
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t& reg = reg_from_regcode(op2 & 0b00000111);
    do_bit_instruction(op2,reg, reg);
}

void CPU::bit_instruction_group_ix()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t &reg = mem[specialRegs.IX + offset];
    do_bit_instruction(op2,reg, reg_from_regcode(op2));
}

void CPU::bit_instruction_group_iy()
{
    int8_t offset = static_cast<int8_t>(fetch8BitValue());
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t &reg = mem[specialRegs.IY + offset];
    do_bit_instruction(op2,reg, reg_from_regcode(op2));
}

/// Perform a bit instruction from the "bit opcode group"
/// \param op2 The opcode which contains the information about the operation
/// \param reg The register to operate on
/// \param copyResultReg Additionally copy the result to this register (used by the undocumente IX/IY bit opcodes). set to the same as reg to disable copying the result.

void CPU::do_bit_instruction( uint8_t op2, uint8_t& reg , uint8_t& copyResultToReg )
{
    // RLC r
    // opcode: 0x00 - 0x07
    // cycles: 8
    if( op2 >= 0 && op2 <= 0x07 )
    {
        uint8_t carry = reg & 0x80;
        reg = reg << 1;

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 1 : 0);
    } else

    // RRC r
    // opcode: 0x08 - 0x0f
    // cycles:
    // RRC r : 8
    // RRC (HL) : 15
    // RRC (IX+d): 23
    // RRC (IY+d): 23
    if( (op2 & 0b11111000) == 0b00001000)
    {
        uint8_t carry = reg & 0x01;
        reg = reg >> 1;

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);

        reg |= (carry ? 0x80 : 0);
    } else

    // RL r
    // opcode: 0x10 - 0x17
    // cycles:
    // RL r: 8
    // RL (hl): 15
    // RL (ix+d): 23
    // RL (iy+d): 23
    // flags: S Z H PV N C
    if( (op2 & 0b11111000) == 0b00010000)
    {
        uint8_t carry = reg & 0x80;

        reg = reg << 1;
        reg |= (regs.F & FlagBitmaskC) ? 1 : 0;

        setFlag( FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

    // RR r
    // opcode: 0x18 - 0x1f
    // cycles:
    // RR r: 8
    // RR (hl): 15
    // RR (ix+d): 23
    // RR (iy+d): 23
    // flags: S Z H PV N C
    if( (op2 & 0b11111000) == 0b00011000)
    {
        uint8_t carry = reg & 0x01;

        reg = reg >> 1;
        reg |= (regs.F & FlagBitmaskC) ? 0x80 : 0;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

    // SLA r
    // opcode: 0x20 - 0x27
    // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x20 && op2 <= 0x27 )
    {
        uint8_t carry = reg & 0x80;

        reg = reg << 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

    // SRA r
    // opcode: 0x28 - 0x2f
    // cycles: r:8 (hl):15 (ix+d):23
    if( op2 >= 0x28 && op2 <= 0x2f )
    {
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x01;
        int8_t signedValue = reg;
        // TODO: page 210. Make sure this is an arithmetic shift that preserves bit 7
        signedValue = signedValue >> 1;
        reg = signedValue;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

    // SLL r
    // opcode: 0x30 - 0x37
    if( op2 >= 0x30 && op2 <= 0x37 ){
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x80;

        reg = reg << 1;
        reg |= 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else

    // SRL r
    // Shift right
    if( op2 >= 0x38 && op2 <= 0x3f)
    {
        uint8_t &reg = reg_from_regcode(op2 & 0b00000111);
        uint8_t carry = reg & 0x01;

        reg = reg >> 1;

        setFlag(FlagBitmaskSign, reg & 0x80);
        setFlag(FlagBitmaskHalfCarry,0);
        setFlag(FlagBitmaskPV, has_parity(reg));
        setFlag(FlagBitmaskN,0);
        setFlag(FlagBitmaskC, carry);
    } else


    // BIT b,r
    // Test bit b in register r and sets Z flag accordingly
    // opcode: 0x40 - 0x7f
    // flag: Z H N
    if( op2 >= 0x40 && op2 <= 0x7f )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t result = reg & (1 << bitNumber);
        setFlag(FlagBitmaskZero, result == 0);
        setFlag(FlagBitmaskHalfCarry,1);
        setFlag(FlagBitmaskN,0);
    } else

    // RES b,r
    // Reset bit b in register r
    // opcode: 0x80 - 0xbf (01 bbb rrr)
    // flags: -
    // cycles: 8
    if( op2 >= 0x80 && op2 <= 0xbf )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t bitMask = ~(1 << bitNumber);
        reg &= bitMask;
    } else

    // SET b,r
    // Set bit b in register r
    // opcode: 0xc0 - 0xff (01 bbb rrr)
    // flags: -
    // cycles: 8
    if( op2 >= 0xc0 && op2 <= 0xff )
    {
        uint8_t bitNumber = (op2 & 0b00111000) >> 3;
        uint8_t bitMask = (1 << bitNumber);
        reg |= bitMask;
    }

    // The undocumented IX and IY bit instructions also loads a copy of the result from (IX+d) or (IY+d)
    // into the register encoded in the opcode
    if( &reg != &copyResultToReg) {
        copyResultToReg = reg;
    }

}

// opcodes: 0xdd ..
void CPU::ix_instruction_group()
{
    // TODO: implement group

    uint8_t op2 = fetch8BitValue();

    if( op2 == 0xCB ){
        bit_instruction_group_ix();
        return;
    }

    // Load r,(IX + d) (11 011 101,01rrr101,dddddddd)
    /*
    if( (op2 & 0b11000111) == 0b01000101){
        uint8_t reg = (op2 & 0b00111000) >> 3;
        int8_t d = fetch8BitValue();
        uint16_t addr = specialRegs.IX + d;
        loadRN(reg,mem[addr]);
    } else
    */
    // Load (IX+d), r (11011101,01110rrr,dddddddd )
    if( (op2 & 0b11111000) == 0b01110000 ){
        uint8_t reg = (op2 & 0b111);
        int8_t d = fetch8BitValue();
        uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
        mem[specialRegs.IX + d] = *regPtr;
    } else

        // Load (IX + d),n (DD,36,d,n)
    if(op2 == 0x36){
        int8_t d = fetch8BitValue();
        uint8_t value = fetch8BitValue();
        mem[specialRegs.IX + d] = value;
    } else

        // Load IX,nn - M:4 T:14
    if(op2 == 0x21){
        specialRegs.IX = fetch16BitValue();
    }

    // Load IX,(nn) - M:6 T:20
    if(op2 == 0x2a){
        uint16_t addr = fetch16BitValue();
        uint8_t lobyte = mem[addr];
        uint8_t hibyte = mem[addr+1];
        specialRegs.IX = (hibyte<<8) + lobyte;
    }

    // Load (nn),IX - (0xdd,0x22,n,n)- M:6 T:20
    if(op2 == 0x22){
        uint16_t addr = fetch16BitValue();
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
}

// opcode: 0xfd ..
void CPU::iy_instruction_group()
{
    // TODO: implement group
    uint8_t op2 = fetch8BitValue();

    if( op2 == 0xCB ){
        bit_instruction_group_iy();
        return;
    }
/*
    // Load r,(IY + d) (11 111 101,01rrr110,dddddddd)
    if( (op2 & 11000111) == 01000110 ){
        uint8_t reg = (op2 & 0b0011100) >> 3;
        uint8_t value = fetch8BitValue();
        uint16_t addr = specialRegs.IY + (int8_t)regs.D;
        loadRN(reg,mem[addr]);
    } else
*/
    // Load (IY + d),r (11111101, 01110rrr, dddddddd)
    if( (op2 & 0b11111000) == 0b01110000){
        uint8_t reg = (op2 & 0b111);
        int8_t d = fetch8BitValue();
        uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
        mem[specialRegs.IY + d] = *regPtr;
    }else
        // Load (IY + d),n (FD,36,d,n)
    if(op2 == 0x36){
        int8_t d = fetch8BitValue();
        uint8_t value = fetch8BitValue();
        mem[specialRegs.IY + d] = value;
    }else

        // Load IX,nn - M:4 T:14
    if(op2 == 0x21){
        specialRegs.IY = fetch16BitValue();
    } else

        // Load IY,(nn) - M:6 T:20
    if(op2 == 0x2a){
        uint16_t addr = fetch16BitValue();
        uint8_t lobyte = mem[addr];
        uint8_t hibyte = mem[addr+1];
        specialRegs.IY = (hibyte<<8) + lobyte;
    } else

        // Load (nn),IY - (0xfd,0x22,n,n)- M:6 T:20
    if(op2 == 0x22){
        uint16_t addr = fetch16BitValue();
        mem[addr] = (uint8_t)specialRegs.IY;
        mem[addr+1] = specialRegs.IY>>8;
    }else

        // Load SP,IY - M:2 T:10
    if(op2 == 0xF9){
        specialRegs.SP = specialRegs.IY;
    } else

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
}

void CPU::step()
{

    // M1: OP Code fetch
    currentOpcode = fetch8BitValue();

    if(currentOpcode < opCodeLookup.size()  ){
        (this->*opCodeLookup[currentOpcode].code)();
        return;
        // (this->*lookup[opcode].addrmode)();
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

    regs.A = result;
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
    setFlag(FlagBitmaskN,1);
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
        regs.A = result;
    }
}

// SBC HL,nn
// opcode: 0xed 01ss0010
void CPU::sbc_hl_nn( uint16_t value ){
    uint16_t hl = regs.value_in_hl();
    uint32_t result = (0xffff0000 | hl) - value;
    if ( regs.F & FlagBitmaskC ) {
        result--;
    }
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskN,1);
    setFlag(FlagBitmaskC, ((result&0xffff0000) < 0xffff0000) );
    setFlag(FlagBitmaskHalfCarry, (0xfffff000 & regs.A) < (result & 0xfffff000) );
    setFlag(FlagBitmaskSign, result & 0x8000 );

    bool isOperandsSignBitDifferent = (hl ^ value) & 0x80;
    bool didChangeSignInResult = (hl ^ result) & 0x80;
    setFlag( FlagBitmaskPV, isOperandsSignBitDifferent && didChangeSignInResult );

    regs.H = hl >> 8;
    regs.L = hl;
}

void CPU::ld_r_r()
{
    uint8_t dstRegCode = (currentOpcode >> 3) & 0b111;
    uint8_t srcRegCode = currentOpcode & 0b111;

    if(srcRegCode == RegisterCode::HLPtr)
    {
        // LD R,(HL)
        uint16_t srcMemAddr = regs.value_in_hl(); // Get memory address from HL register pair
        uint8_t data = mem[srcMemAddr]; // Get data from HL location
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = data;
    }
    if(dstRegCode == RegisterCode::HLPtr)
    {
        // LD (HL),R
        uint16_t dstMemAddr = regs.value_in_hl(); // Get memory address from HL register pair
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        mem[dstMemAddr] = *srcRegPtr;
    }
    else
    { // LD R,R
        uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcRegCode;
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstRegCode;
        *dstRegPtr = *srcRegPtr;
    }
}

void CPU::inc_r(uint8_t &reg)
{
    reg++;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x80 ); // Set if B was 0x7f before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, 0); // reset
}

void CPU::dec_r(uint8_t &reg)
{
    reg--;
    setFlag( FlagBitmaskSign, reg & 0x80 ); // Set if result is negative (a.k.a. if sign bit is set)
    setFlag( FlagBitmaskZero, reg == 0 );   // Set if result is zero
    setFlag( FlagBitmaskPV, reg == 0x7f ); // Set if B was 0x80 before
    setFlag( FlagBitmaskHalfCarry, (reg & 0b00001111) == 0b00001111 ); // Set if carry over from bit 3
    setFlag( FlagBitmaskN, 1); // set
}

// Increase register pair value
void CPU::inc_rr(uint8_t& hibyte, uint8_t& lobyte)
{
    lobyte++;
    if(lobyte==0)hibyte++;
}

// Increase register pair value
void CPU::dec_rr(uint8_t& hibyte, uint8_t& lobyte)
{
    lobyte--;
    if(lobyte==0xff)hibyte--;
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
    regs.B = fetch8BitValue();
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
    uint16_t addr = fetch16BitValue();
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

void CPU::ld_ptr_nn_nn( uint16_t location, uint16_t value)
{
    mem[location] = value;
    mem[location+1] = value >> 8;
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
    uint8_t result = mem[regs.value_in_hl()]++;
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
    uint8_t result = mem[regs.value_in_hl()]--;
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
    mem[regs.value_in_hl()] = fetch8BitValue();
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
    uint32_t result = regs.value_in_hl() + specialRegs.SP;
    setFlag(FlagBitmaskC,result & 0x100000000);
    setFlag(FlagBitmaskHalfCarry, (regs.value_in_hl() & 0xfff) > (result & 0xfff));
    setFlag(FlagBitmaskN,0);
    regs.L = result;
    regs.H = result >> 8;
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


// cycles: 4
void CPU::and_r()
{
    uint8_t srcRegCode = currentOpcode & 0b111;
    uint8_t srcRegValue = value_from_regcode(srcRegCode);

    regs.A &= srcRegValue;

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,1);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
}

void CPU::and_n()
{
    regs.A &= fetch8BitValue();

    setFlag(FlagBitmaskSign, regs.A & 0x80);
    setFlag(FlagBitmaskZero, regs.A == 0);
    setFlag(FlagBitmaskHalfCarry,1);
    setFlag(FlagBitmaskPV, has_parity(regs.A));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,0);
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

// cycles: 10
void CPU::pop_qq()
{
    uint8_t regPairCode = (currentOpcode & 0b00110000) >> 4;
    uint8_t lobyte = mem[specialRegs.SP++];
    uint8_t hibyte = mem[specialRegs.SP++];

    switch (regPairCode)
    {
        case 0:
            regs.B = hibyte;
            regs.C = lobyte;
            break;
        case 1:
            regs.D = hibyte;
            regs.E = lobyte;
            break;
        case 2:
            regs.H = hibyte;
            regs.L = lobyte;
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

// JP HL
// opcode: 0xe9
// cycles: 4
// flags: -
void CPU::jp_ptr_hl(){
    specialRegs.PC = regs.value_in_hl();
}


// cycles: 17 (true, 10 (false)
void CPU::call_cc_nn()
{
    uint8_t conditionCode = (currentOpcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)){
        mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
        mem[--specialRegs.SP] = specialRegs.PC;      // (SP-2) = PC_h
        specialRegs.PC = location;
    }
}

// opcode: 0xCD
// cycles: 17
void CPU::call(){
    uint16_t location = fetch16BitValue();
    mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
    mem[--specialRegs.SP] = specialRegs.PC;      // (SP-2) = PC_h
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
    mem[--specialRegs.SP] = specialRegs.PC;

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
void CPU::ex_ptr_sp_hl()
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
    specialRegs.SP = regs.value_in_hl();
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
    uint16_t hl = regs.value_in_hl();
    uint32_t result = hl + value;
    if ( regs.F & FlagBitmaskC ) {
        result++;
    }

    setFlag(FlagBitmaskC,result > 0xffff);
    setFlag(FlagBitmaskZero, (result&0xffff) == 0);
    setFlag(FlagBitmaskHalfCarry, (hl & 0x0fff) > (result & 0x0fff));
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskSign, result & 0x8000);
    setFlag(FlagBitmaskPV, !((hl ^ value) & 0x8000) && ((result ^ hl) & 0x8000) );

    regs.H = result >> 8;
    regs.L = result;
}

// RRD - do some swapping of nibbles between A and (HL) (see details on p.219 z80 tech ref.)
// opcode: 0xed 0x67
// cycles: 18
// flags: s z h pv n
void CPU::rrd()
{
    uint8_t hl = regs.value_in_hl();
    uint8_t data = mem[hl];
    uint8_t old_data_lownib = data & 0xf;
    data = data >> 4;
    data |= (regs.A << 4);
    regs.A &= 0xf0;
    regs.A |= old_data_lownib;
    mem[hl] = data;
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
    uint8_t hl = regs.value_in_hl();
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
    if(regs.value_in_bc() > 0)
    {
        do
        {
            mem[regs.value_in_de()] = mem[regs.value_in_hl()]; // (de) = (hl)

            if (decrease)
            {
                dec_rr(regs.D, regs.E); // inc de
                dec_rr(regs.H, regs.L); // inc hl
            } else
            {
                inc_rr(regs.D, regs.E); // inc de
                inc_rr(regs.H, regs.L); // inc hl
            }

            dec_rr(regs.B, regs.C); // dec bc

        } while (repeat && regs.value_in_bc() > 0);
    }

    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN, 0);
    setFlag(FlagBitmaskPV, regs.value_in_bc() != 0);
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
    if( regs.value_in_bc() > 0)
    {
        do
        {
            uint8_t result = regs.A - mem[regs.value_in_hl()];

            if(decrease){
                dec_rr(regs.H,regs.L); // inc hl
            } else {
                inc_rr(regs.H,regs.L); // inc hl
            }

            dec_rr(regs.B,regs.C); // dec bc

            setFlag(FlagBitmaskSign,result & 0x80);
            setFlag(FlagBitmaskZero, result == 0);
            setFlag(FlagBitmaskHalfCarry, (regs.A & 0xf0) < (result & 0xf0) );
        }
        while (repeat && regs.value_in_bc() > 0);
    }

    setFlag(FlagBitmaskPV,regs.value_in_bc() == 0);
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
            mem[regs.value_in_hl()] = value;

            regs.B--;

            if(decrease){
                dec_rr(regs.H,regs.L);
            } else {
                inc_rr(regs.H,regs.L);
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
            output_to_port(regs.C, mem[regs.value_in_hl()]);
            regs.B--;
            if(decrease){
                dec_rr(regs.H, regs.L);
            } else {
                inc_rr(regs.H, regs.L);
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



void CPU::invalid_opcode()
{
    // TODO: maybe log this?
}