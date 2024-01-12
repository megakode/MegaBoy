#include "disassembler.h"

// Lambda that just returns a fixed text as a result om disassembling the given instruction bytes
#define DISASMTEXT(t) [](auto) { return t; }
#define DISASMFUNC(f) [this](const InstructionBytes &ib) { return this->##f(ib); }

Disassembler::Disassembler()
{
    // Based on tables from:
    // https://clrhome.org/table/
    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

    instructions = {
        {.numBytes = 1, .cycles = 4, DISASMTEXT("NOP")},        // 0x00 NOP
        {.numBytes = 3, .cycles = 12, DISASMFUNC(LD_BC_nnnn)},  // 0x01 LD BC,nn
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (BC),A")},  // 0x02 "LD (BC),A"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("INC BC")},     // 0x03 "INC BC"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC B")},      // 0x04 "INC B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC B")},      // 0x05 "DEC B"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},       // 0x06 "LD B,N"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("RLCA")},       // 0x07 "RLCA"
        {.numBytes = 3, .cycles = 20, DISASMFUNC(LD_pnnnn_SP)}, // 0x08 "LD_(nnnn),SP"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("ADD HL,BC")},  // 0x09 "ADD HL,BC"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD A,(BC)")},  // 0x0a "LD A,(BC)"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("DEC BC")},     // 0x0b "DEC BC"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC C")},      // 0x0c "INC C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC C")},      // 0x0d "DEC C"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},       // 0x0e "LD C,n"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("RRCA")},       // 0x0f "RRCA"

        {.numBytes = 2, .cycles = 4, DISASMTEXT("STOP")},      // 0x10 "STOP"
        {.numBytes = 3, .cycles = 12, DISASMFUNC(LD_DE_nnnn)}, // 0x11 "LD DE,nnnn"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (DE),A")}, // 0x12 "LD (DE),A"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("INC DE")},    // 0x13 "INC DE"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC D")},     // 0x14 "INC D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC D")},     // 0x15 "DEC D"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},      // 0x16 "LD D,N"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("RLA")},       // 0x17 "RLA"
        // {12, &Disassembler::JR_n},       // 0x18 "JR N"
        {.numBytes = 1, .cycles = (8), DISASMTEXT("ADD HL,DE")}, // 0x19 "ADD HL,DE"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD A,(DE)")},   // 0x1a "LD A,(DE)"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("DEC DE")},      // 0x1b "DEC DE"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC E")},       // 0x1c "INC E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC E")},       // 0x1d "DEC E"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},        // 0x1e "LD E,N"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("RLA")},         // 0x1f "RRA"

        // {.numBytes = 2, .cycles = 4, DISASMTEXT("JR NZ")},      // 0x20 "JR NZ"
        {.numBytes = 3, .cycles = 12, DISASMFUNC(LD_HL_nnnn)},  // 0x21 "LD HL,nnnn"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL+),A")}, // 0x22 "LD (HL+),A"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("INC HL")},     // 0x23 "INC HL"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC H")},      // 0x24 "INC H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC H")},      // 0x25 "DEC H"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},       // 0x26 "LD H,N"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DAA")},        // 0x27 "DAA"
        // { 8, &Disassembler::JR_z},       // 0x28 "JR Z"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("ADD HL,HL")},  // 0x29 "ADD HL,HL"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD A,(HL+)")}, // 0x2a "LD A,(HL+)"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("DEC HL")},     // 0x2b "DEC HL"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC L")},      // 0x2c "INC L"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC L")},      // 0x2d "DEC L"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},       // 0x2e "LD L,n"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("CPL")},        // 0x2f "CPL"

        // { 8, &Disassembler::JR_nc},      // 0x30 "JR NC",
        {.numBytes = 3, .cycles = 12, DISASMFUNC(LD_SP_nnnn)},  // 0x31 "LD SP,nnnn",
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL-),A")}, // 0x32 "LD (HL-),A"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("INC SP")},     // 0x33 "INC SP"
        {.numBytes = 1, .cycles = 12, DISASMTEXT("INC (HL)")},  // 0x34 "INC (HL)"
        {.numBytes = 1, .cycles = 12, DISASMTEXT("DEC (HL)")},  // 0x35 "DEC (HL)"
        {.numBytes = 2, .cycles = 12, DISASMFUNC(LD_r_n)},      // 0x36 "LD (HL),n"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("SCF")},        // 0x37 "SCF"
        // {12, &Disassembler::JR_c},       // 0x38 "JR C,N"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("ADD HL,SP")},  // 0x39 "ADD HL,SP"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD A,(HL-)")}, // 0x3a "LD A,(HL-)"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("DEC SP")},     // 0x3b "DEC SP"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("INC A")},      // 0x3c "INC A"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("DEC A")},      // 0x3d "DEC A"
        {.numBytes = 2, .cycles = 8, DISASMFUNC(LD_r_n)},       // 0x3e "LD A,N"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("CCF")},        // 0x3f "CCF"

        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,B")},    // 0x40 "LD B,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,C")},    // 0x41 "LD B,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,D")},    // 0x42 "LD B,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,E")},    // 0x43 "LD B,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,H")},    // 0x44 "LD B,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,L")},    // 0x45 "LD B,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD B,(HL)")}, // 0x46 "LD B,(hl)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD B,A")},    // 0x47 "LD B,A"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,B")},    // 0x48 "LD C,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,C")},    // 0x49 "LD C,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,D")},    // 0x4a "LD C,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,E")},    // 0x4b "LD C,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,H")},    // 0x4c "LD C,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,L")},    // 0x4d "LD C,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD C,(HL)")}, // 0x4e "LD C,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD C,A")},    // 0x4f "LD C,A"

        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,B")},    // 0x50 "LD D,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,C")},    // 0x51 "LD D,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,D")},    // 0x52 "LD D,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,E")},    // 0x53 "LD D,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,H")},    // 0x54 "LD D,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,L")},    // 0x55 "LD D,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD D,(HL)")}, // 0x56 "LD D,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD D,A")},    // 0x57 "LD D,A"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,B")},    // 0x58 "LD E,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,C")},    // 0x59 "LD E,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,D")},    // 0x5a "LD E,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,E")},    // 0x5b "LD E,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,H")},    // 0x5c "LD E,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,L")},    // 0x5d "LD E,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD E,(HL)")}, // 0x5e "LD E,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD E,A")},    // 0x5f "LD E,A"

        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,B")},    // 0x60 "LD H,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,C")},    // 0x61 "LD H,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,D")},    // 0x62 "LD H,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,E")},    // 0x63 "LD H,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,H")},    // 0x64 "LD H,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,L")},    // 0x65 "LD H,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD H,(HL)")}, // 0x66 "LD H,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD H,A")},    // 0x67 "LD H,A"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,B")},    // 0x68 "LD L,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,C")},    // 0x69 "LD L,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,D")},    // 0x6a "LD L,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,E")},    // 0x6b "LD L,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,H")},    // 0x6c "LD L,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,L")},    // 0x6d "LD L,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD L,(HL)")}, // 0x6e "LD L,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD L,A")},    // 0x6f "LD L,A

        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),B")}, // 0x70 "LD (HL),B"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),C")}, // 0x71 "LD R,R"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),D")}, // 0x72 "LD R,R"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),E")}, // 0x73 "LD R,R"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),H")}, // 0x74 "LD R,R"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),L")}, // 0x75 "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("HALT")},      // 0x76 "HALT"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD (HL),A")}, // 0x77 "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,B")},    // 0x78 "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,C")},    // 0x79 "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,D")},    // 0x7a "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,E")},    // 0x7b "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,H")},    // 0x7c "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,L")},    // 0x7d "LD R,R"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("LD A,(HL)")}, // 0x7e "LD R,R"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("LD A,A")},    // 0x7f "LD R,R"

        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,B")},    // 0x80 "ADD A,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,C")},    // 0x81 "ADD A,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,D")},    // 0x82 "ADD A,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,E")},    // 0x83 "ADD A,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,H")},    // 0x84 "ADD A,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,L")},    // 0x85 "ADD A,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("ADD A,(HL)")}, // 0x86 "ADD A,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADD A,A")},    // 0x87 "ADD A,A"

        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,B")}, // 0x88 "ADC A,B"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,C")}, // 0x89 "ADC A,C"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,D")}, // 0x8a "ADC A,D"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,E")}, // 0x8b "ADC A,E"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,H")}, // 0x8c "ADC A,H"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,L")},// 0x8d "ADC A,L"
        {.numBytes = 1, .cycles = 8, DISASMTEXT("ADC A,(HL)")}, // 0x8e "ADC A,(HL)"
        {.numBytes = 1, .cycles = 4, DISASMTEXT("ADC A,A")}, // 0x8f "ADC A,A"

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