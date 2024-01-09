#include "disassembler.h"

// Lambda that just returns a fixed text as a result om disassembling the given instruction bytes
#define DISASMTEXT(t) [](auto) { return t; }
#define DISASMFUNC(f) [this](const InstructionBytes &ib) { return this->##f(ib); }

Disassembler::Disassembler()
{
    // Based on tables from:
    // https://clrhome.org/table/
    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

    // Instruction ins = {.numBytes = 3, .cycles = 12, .code = [this](const InstructionBytes &bts)
    //{return this->LD_BC_nn(bts);}}X;
    Instruction ins = {.numBytes = 3, .cycles = 12, .code = DISASMFUNC(LD_BC_nn)};

    instructions = {
        {.numBytes = 1, .cycles = 4, DISASMTEXT("NOP")},       // 0x00 NOP
                                                               // {.numBytes = 3, .cycles = 12, DISASMFUNC(LD_BC_nn)},   // 0x01 LD BC,NN
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (BC),A")}, // 0x02 "LD (BC),A"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("INC BC")}     // 0x03 "INC BC"
        // { 4, &Disassembler::INC_B},      // 0x04 "INC B"
        // { 4, &Disassembler::DEC_B},      // 0x05 "DEC B"
        // { 8, &Disassembler::LD_r_n},     // 0x06 "LD B,N"
        // { 4, &Disassembler::RLCA},       // 0x07 "RLCA"
        // {20, &Disassembler::LD_pnn_SP},  // 0x08 "LD_(nnnn),SP"
        // { 8, &Disassembler::ADD_HL_BC},  // 0x09 "ADD HL,BC"
        // { 8, &Disassembler::LD_A_pBC},   // 0x0a "LD A,(BC)"
        // { 8, &Disassembler::DEC_BC},     // 0x0b "DEC BC"
        // { 4, &Disassembler::INC_C},      // 0x0c "INC C"
        // { 4, &Disassembler::DEC_C},      // 0x0d "DEC C"
        // { 8, &Disassembler::LD_r_n},     // 0x0e "LD C,N"
        // { 4, &Disassembler::RRCA},       // 0x0f "RRCA"

        // { 4, &Disassembler::STOP},       // 0x10 "DJNZ N"
        // {12, &Disassembler::LD_DE_nn},   // 0x11 "LD DE,NN"
        // { 8, &Disassembler::LD_pDE_A},   // 0x12 "LD (DE),A"
        // { 8, &Disassembler::INC_DE},     // 0x13 "INC DE"
        // { 4, &Disassembler::INC_D},      // 0x14 "INC D"
        // { 4, &Disassembler::DEC_D},      // 0x15 "DEC D"
        // { 8, &Disassembler::LD_r_n},     // 0x16 "LD D,N"
        // { 4, &Disassembler::RLA},        // 0x17 "RLA"
        // {12, &Disassembler::JR_n},       // 0x18 "JR N"
        // { 8, &Disassembler::ADD_HL_DE},  // 0x19 "ADD HL,DE"
        // { 8, &Disassembler::LD_A_pDE},   // 0x1a "LD A,(DE)"
        // { 8, &Disassembler::DEC_DE},     // 0x1b "DEC DE"
        // { 4, &Disassembler::INC_E},      // 0x1c "INC E"
        // { 4, &Disassembler::DEC_E},      // 0x1d "DEC E"
        // { 8, &Disassembler::LD_r_n},     // 0x1e "LD E,N"
        // { 4, &Disassembler::RRA},        // 0x1f "RRA"

        // { 8, &Disassembler::JR_nz},      // 0x20 "JR NZ"
        // {12, &Disassembler::LD_HL_nn},   // 0x21 "LD HL,NN"
        // { 8, &Disassembler::LDI_pHL_A},  // 0x22 "LD (NN),HL"
        // { 8, &Disassembler::INC_HL},     // 0x23 "INC HL"
        // { 4, &Disassembler::INC_H},      // 0x24 "INC H"
        // { 4, &Disassembler::DEC_H},      // 0x25 "DEC H"
        // { 8, &Disassembler::LD_r_n},     // 0x26 "LD H,N"
        // { 4, &Disassembler::daa},        // 0x27 "DAA"
        // { 8, &Disassembler::JR_z},       // 0x28 "JR Z"
        // { 8, &Disassembler::ADD_HL_HL},  // 0x29 "ADD HL,HL"
        // { 8, &Disassembler::LDI_A_pHL},  // 0x2a "LD HL,(NN)"
        // { 8, &Disassembler::DEC_HL},     // 0x2b "DEC HL"
        // { 4, &Disassembler::INC_L},      // 0x2c "INC L"
        // { 4, &Disassembler::DEC_L},      // 0x2d "DEC L"
        // { 8, &Disassembler::LD_r_n},     // 0x2e "LD L,N"
        // { 4, &Disassembler::cpl},        // 0x2f "CPL"

        // { 8, &Disassembler::JR_nc},      // 0x30 "JR NC",
        // {12, &Disassembler::LD_SP_nn},   // 0x31 "LD SP,NN",
        // { 8, &Disassembler::LDD_pHL_A},  // 0x32 "LD (NN),A"
        // { 8, &Disassembler::INC_SP},     // 0x33 "INC SP"
        // {12, &Disassembler::INC_pHL},    // 0x34 "INC (HL)"
        // {12, &Disassembler::DEC_pHL},    // 0x35 "DEC (HL)"
        // {12, &Disassembler::LD_pHL_n},   // 0x36 "LD (HL),N"
        // { 4, &Disassembler::scf},        // 0x37 "SCF"
        // {12, &Disassembler::JR_c},       // 0x38 "JR C,N"
        // { 8, &Disassembler::ADD_HL_SP},  // 0x39 "ADD HL,SP"
        // { 8, &Disassembler::LDD_A_pHL},  // 0x3a "LD A,(NN)"
        // { 8, &Disassembler::DEC_SP},     // 0x3b "DEC SP"
        // { 4, &Disassembler::INC_A},      // 0x3c {"INC A"
        // { 4, &Disassembler::DEC_A},      // 0x3d "DEC A"
        // { 8, &Disassembler::LD_r_n},     // 0x3e "LD A,N"
        // { 4, &Disassembler::ccf},        // 0x3f "CCF"

        // {4, &Disassembler::LD_r_r},      // 0x40 "LD B,B"
        // {4, &Disassembler::LD_r_r},      // 0x41 "LD B,C"
        // {4, &Disassembler::LD_r_r},      // 0x42 "LD B,D"
        // {4, &Disassembler::LD_r_r},      // 0x43 "LD B,E"
        // {4, &Disassembler::LD_r_r},      // 0x44 "LD B,H"
        // {4, &Disassembler::LD_r_r},      // 0x45 "LD B,L"
        // {8, &Disassembler::LD_r_r},      // 0x46 "LD B,(hl)"
        // {4, &Disassembler::LD_r_r},      // 0x47 "LD B,A"
        // {4, &Disassembler::LD_r_r},      // 0x48 "LD C,B"
        // {4, &Disassembler::LD_r_r},      // 0x49 "LD C,C"
        // {4, &Disassembler::LD_r_r},      // 0x4a "LD C,D"
        // {4, &Disassembler::LD_r_r},      // 0x4b "LD C,E"
        // {4, &Disassembler::LD_r_r},      // 0x4c "LD C,H"
        // {4, &Disassembler::LD_r_r},      // 0x4d "LD C,L"
        // {8, &Disassembler::LD_r_r},      // 0x4e "LD C,(HL)"
        // {4, &Disassembler::LD_r_r},      // 0x4f "LD C,A"

        // {4, &Disassembler::LD_r_r},      // 0x50 "LD D,B"
        // {4, &Disassembler::LD_r_r},      // 0x51 "LD D,C"
        // {4, &Disassembler::LD_r_r},      // 0x52 "LD D,D"
        // {4, &Disassembler::LD_r_r},      // 0x53 "LD D,E"
        // {4, &Disassembler::LD_r_r},      // 0x54 "LD D,H"
        // {4, &Disassembler::LD_r_r},      // 0x55 "LD D,L"
        // {8, &Disassembler::LD_r_r},      // 0x56 "LD D,(HL)"
        // {4, &Disassembler::LD_r_r},      // 0x57 "LD D,A"
        // {4, &Disassembler::LD_r_r},      // 0x58 "LD E,B"
        // {4, &Disassembler::LD_r_r},      // 0x59 "LD E,C"
        // {4, &Disassembler::LD_r_r},      // 0x5a "LD E,D"
        // {4, &Disassembler::LD_r_r},      // 0x5b "LD E,E"
        // {4, &Disassembler::LD_r_r},      // 0x5c "LD E,H"
        // {4, &Disassembler::LD_r_r},      // 0x5d "LD E,L"
        // {8, &Disassembler::LD_r_r},      // 0x5e "LD E,(HL)"
        // {4, &Disassembler::LD_r_r},      // 0x5f "LD E,A"

        // {4, &Disassembler::LD_r_r},      // 0x60 "LD H,B"
        // {4, &Disassembler::LD_r_r},      // 0x61 "LD H,C"
        // {4, &Disassembler::LD_r_r},      // 0x62 "LD H,D"
        // {4, &Disassembler::LD_r_r},      // 0x63 "LD H,E"
        // {4, &Disassembler::LD_r_r},      // 0x64 "LD H,H"
        // {4, &Disassembler::LD_r_r},      // 0x65 "LD H,L"
        // {8, &Disassembler::LD_r_r},      // 0x66 "LD H,(HL)"
        // {4, &Disassembler::LD_r_r},      // 0x67 "LD H,A"
        // {4, &Disassembler::LD_r_r},      // 0x68 "LD L,B"
        // {4, &Disassembler::LD_r_r},      // 0x69 "LD L,C"
        // {4, &Disassembler::LD_r_r},      // 0x6a "LD L,D"
        // {4, &Disassembler::LD_r_r},      // 0x6b "LD L,E"
        // {4, &Disassembler::LD_r_r},      // 0x6c "LD L,H"
        // {4, &Disassembler::LD_r_r},      // 0x6d "LD L,L"
        // {8, &Disassembler::LD_r_r},      // 0x6e "LD L,(HL)"
        // {4, &Disassembler::LD_r_r},      // 0x6f "LD L,A

        // {8, &Disassembler::LD_r_r},      // 0x70 "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x71 "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x72 "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x73 "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x74 "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x75 "LD R,R"
        // {4, &Disassembler::halt},        // 0x76 "HALT"
        // {8, &Disassembler::LD_r_r},      // 0x77 "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x78 "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x79 "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x7a "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x7b "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x7c "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x7d "LD R,R"
        // {8, &Disassembler::LD_r_r},      // 0x7e "LD R,R"
        // {4, &Disassembler::LD_r_r},      // 0x7f "LD R,R"

        // {&Disassembler::ADD_A_r}, // 0x80 "ADD A,B"
        // {&Disassembler::ADD_A_r}, // 0x81 "ADD A,C"
        // {&Disassembler::ADD_A_r}, // 0x82 "ADD A,D"
        // {&Disassembler::ADD_A_r}, // 0x83 "ADD A,E"
        // {&Disassembler::ADD_A_r}, // 0x84 "ADD A,H"
        // {&Disassembler::ADD_A_r}, // 0x85 "ADD A,L"
        // {&Disassembler::ADD_A_r}, // 0x86 "ADD A,(HL)"
        // {&Disassembler::ADD_A_r}, // 0x87 "ADD A,A"

        // {&Disassembler::ADC_A_r}, // 0x88 "ADC A,B"
        // {&Disassembler::ADC_A_r}, // 0x89 "ADC A,C"
        // {&Disassembler::ADC_A_r}, // 0x8a "ADC A,D"
        // {&Disassembler::ADC_A_r}, // 0x8b "ADC A,E"
        // {&Disassembler::ADC_A_r}, // 0x8c "ADC A,H"
        // {&Disassembler::ADC_A_r}, // 0x8d "ADC A,L"
        // {&Disassembler::ADC_A_r}, // 0x8e "ADC A,(HL)"
        // {&Disassembler::ADC_A_r}, // 0x8f "ADC A,A"

        // {&Disassembler::SUB_r}, // 0x90 "SUB B"
        // {&Disassembler::SUB_r}, // 0x91 "SUB C"
        // {&Disassembler::SUB_r}, // 0x92 "SUB D"
        // {&Disassembler::SUB_r}, // 0x93 "SUB E"
        // {&Disassembler::SUB_r}, // 0x94 "SUB H"
        // {&Disassembler::SUB_r}, // 0x95 "SUB L"
        // {&Disassembler::SUB_r}, // 0x96 "SUB (HL)"
        // {&Disassembler::SUB_r}, // 0x97 "SUB A"

        // {&Disassembler::SBC_r}, // 0x98 "SBC B"
        // {&Disassembler::SBC_r}, // 0x99 "SBC C"
        // {&Disassembler::SBC_r}, // 0x9a "SBC D"
        // {&Disassembler::SBC_r}, // 0x9b "SBC E"
        // {&Disassembler::SBC_r}, // 0x9c "SBC H"
        // {&Disassembler::SBC_r}, // 0x9d "SBC L"
        // {&Disassembler::SBC_r}, // 0x9e "SBC (HL)"
        // {&Disassembler::SBC_r}, // 0x9f "SBC A"

        // {&Disassembler::AND_r}, // 0xa0 "AND B"
        // {&Disassembler::AND_r}, // 0xa1 "AND C"
        // {&Disassembler::AND_r}, // 0xa2 "AND D"
        // {&Disassembler::AND_r}, // 0xa3 "AND E"
        // {&Disassembler::AND_r}, // 0xa4 "AND H"
        // {&Disassembler::AND_r}, // 0xa5 "AND L"
        // {&Disassembler::AND_r}, // 0xa6 "AND (HL)"
        // {&Disassembler::AND_r}, // 0xa7 "AND A"

        // {&Disassembler::XOR_r}, // 0xa8 "XOR B"
        // {&Disassembler::XOR_r}, // 0xa9 "XOR C"
        // {&Disassembler::XOR_r}, // 0xaa "XOR D"
        // {&Disassembler::XOR_r}, // 0xab "XOR E"
        // {&Disassembler::XOR_r}, // 0xac "XOR H"
        // {&Disassembler::XOR_r}, // 0xad "XOR L"
        // {&Disassembler::XOR_r}, // 0xae "XOR (HL)"
        // {&Disassembler::XOR_r}, // 0xaf "XOR A"

        // {&Disassembler::OR_r}, // 0xb0 "OR B"
        // {&Disassembler::OR_r}, // 0xb1 "OR C"
        // {&Disassembler::OR_r}, // 0xb2 "OR D"
        // {&Disassembler::OR_r}, // 0xb3 "OR E"
        // {&Disassembler::OR_r}, // 0xb4 "OR H"
        // {&Disassembler::OR_r}, // 0xb5 "OR L"
        // {&Disassembler::OR_r}, // 0xb6 "OR (HL)"
        // {&Disassembler::OR_r}, // 0xb7 "OR A"

        // {&Disassembler::CP_r}, // 0xb8 "CP B"
        // {&Disassembler::CP_r}, // 0xb9 "CP C"
        // {&Disassembler::CP_r}, // 0xba "CP D"
        // {&Disassembler::CP_r}, // 0xbb "CP E"
        // {&Disassembler::CP_r}, // 0xbc "CP H"
        // {&Disassembler::CP_r}, // 0xbd "CP L"
        // {&Disassembler::CP_r}, // 0xbe "CP (HL)"
        // {&Disassembler::CP_r}, // 0xbf "CP A"

        // { 8, &Disassembler::RET_cc},        // 0xc0 "RET NZ"
        // {12, &Disassembler::pop_qq},        // 0xc1 "POP BC"
        // {12, &Disassembler::JP_cc_nn},      // 0xc2 "JP NZ NN"
        // {16, &Disassembler::JP_nn},         // 0xc3 "JP NN"
        // {12, &Disassembler::CALL_cc_nn},    // 0xc4 "CALL NZ,NN"
        // {16, &Disassembler::push_bc},       // 0xc5 "PUSH BC"
        // { 8, &Disassembler::ADD_A_n},       // 0xc6 "ADD A,n"
        // {16, &Disassembler::RST},           // 0xc7 "RST 00h"
        // { 8, &Disassembler::RET_cc},        // 0xc8 "RET Z"
        // {16, &Disassembler::RET},           // 0xc9 "RET"
        // {12, &Disassembler::JP_cc_nn},      // 0xca "JP Z,**"
        // { 4, &Disassembler::decode_bit_instruction}, // 0xcb "BIT opcode group"
        // {12, &Disassembler::CALL_cc_nn},    // 0xcc "CALL Z,nn"
        // {24, &Disassembler::CALL},          // 0xcd "CALL nn"
        // { 8, &Disassembler::ADC_A_n},       // 0xce "ADC A,N"
        // {16, &Disassembler::RST},           // 0xcf "RST 08h"

        // { 8, &Disassembler::RET_cc},        // 0xd0 "RET NC"
        // {12, &Disassembler::pop_qq},        // 0xd1 "POP DE"
        // {12, &Disassembler::JP_cc_nn},      // 0xd2 "JP NC,NN"
        // { 0, &Disassembler::invalid_opcode},// 0xd3 "OUT (N),A"
        // {12, &Disassembler::CALL_cc_nn},    // 0xd4 "CALL NC,NN"
        // {16, &Disassembler::push_de},       // 0xd5 "PUSH DE"
        // { 8, &Disassembler::SUB_n},         // 0xd6 "SUB N"
        // {16, &Disassembler::RST},           // 0xd7 "RST 10h"
        // { 8, &Disassembler::RET_cc},        // 0xd8 "RET C"
        // {16, &Disassembler::RETI},          // 0xd9 "EXX"
        // {12, &Disassembler::JP_cc_nn},      // 0xda "JP C,NN"
        // { 0, &Disassembler::invalid_opcode},// 0xdb
        // {12, &Disassembler::CALL_cc_nn},    // 0xdc "CALL C,NN"
        // { 0, &Disassembler::invalid_opcode},// 0xdd
        // { 8, &Disassembler::SBC_n},         // 0xde "SBC N"
        // {16, &Disassembler::RST},           // 0xdf "RST 18h"

        // {12, &Disassembler::LD_ff00n_A},     // 0xe0 LD_ff00 + n, A
        // {12, &Disassembler::pop_qq},         // 0xe1 "POP HL"
        // { 8, &Disassembler::LD_ff00C_A},     // 0xe2 "JP PO,NN"
        // { 0, &Disassembler::invalid_opcode}, // 0xe3 -
        // { 0, &Disassembler::invalid_opcode}, // 0xe4 -
        // {16, &Disassembler::push_hl},        // 0xe5 "PUSH HL"
        // { 8, &Disassembler::AND_n},          // 0xe6 "AND N"
        // {16, &Disassembler::RST},            // 0xe7 "RST 20h"
        // {16, &Disassembler::ADD_SP_s8},      // 0xe8 "RET PE"
        // { 4, &Disassembler::JP_pHL},         // 0xe9 "JP (HL)"
        // {16, &Disassembler::LD_pnn_A},       // 0xea "LD (nn),A"
        // { 0, &Disassembler::invalid_opcode}, // 0xeb
        // { 0, &Disassembler::invalid_opcode}, // 0xec
        // { 0, &Disassembler::invalid_opcode}, // 0xed
        // { 8, &Disassembler::XOR_n},          // 0xee "XOR N"
        // {16, &Disassembler::RST},            // 0xef "RST 28h"

        // {12, &Disassembler::LD_A_ff00n},        // 0xf0 "RET P"
        // {12, &Disassembler::pop_qq},            // 0xf1 "POP AF"
        // { 8, &Disassembler::LD_A_ff00C},        // 0xf2 "JP P,NN"
        // { 4, &Disassembler::disable_interrupts},// 0xf3 "DI"
        // { 0, &Disassembler::invalid_opcode},    // 0xf4
        // {16, &Disassembler::push_af},           // 0xf5 "PUSH AF"
        // { 8, &Disassembler::OR_n},              // 0xf6 "OR N"
        // {16, &Disassembler::RST},               // 0xf7 "RST 30h"
        // {12, &Disassembler::LD_HL_SPs8},        // 0xf8 "LD HL,SP+s8"
        // { 8, &Disassembler::ld_sp_hl},          // 0xf9 "LD SP,HL"
        // {16, &Disassembler::LD_A_pnn},          // 0xfa "JP M,NN"
        // { 4, &Disassembler::enable_interrupts}, // 0xfb "EI"
        // { 0, &Disassembler::invalid_opcode},    // 0xfc
        // { 0, &Disassembler::invalid_opcode},    // 0xfd
        // { 8, &Disassembler::CP_n},              // 0xfe "CP N"
        // {16, &Disassembler::RST}                // 0xff "RST 38h"
    };
}

DisassemblyLine Disassembler::DisassembleAddress(uint16_t address, const HostMemory &memref)
{
    InstructionBytes bytes = {memref.Read(address), memref.Read(address + 1), memref.Read(address + 2)};

    // Instruction inst = (this->*instructions)[bytes.data[0]];

    DisassemblyLine line;

    // line.text = inst.code(bytes);

    return std::move(line);
}