#include <format>
#include "disassembler.h"

Disassembler::Disassembler()
{
    // Based on tables from:
    // https://clrhome.org/table/
    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    // https://gbdev.io/gb-opcodes/optables/

    instructions = std::vector<Instruction>{
        {.numBytes = 1, .cycles = 4, .text = "NOP"},                       // 0x00 NOP
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::LD_BC_nnnn},  // 0x01 LD BC,nnnn
        {.numBytes = 1, .cycles = 8, .text = "LD (BC),A"},                 // 0x02 "LD (BC),A"
        {.numBytes = 1, .cycles = 8, .text = "INC BC"},                    // 0x03 "INC BC"
        {.numBytes = 1, .cycles = 4, .text = "INC B"},                     // 0x04 "INC B"
        {.numBytes = 1, .cycles = 4, .text = "DEC B"},                     // 0x05 "DEC B"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},       // 0x06 "LD B,N"
        {.numBytes = 1, .cycles = 4, .text = "RLCA"},                      // 0x07 "RLCA"
        {.numBytes = 3, .cycles = 20, .text = &Disassembler::LD_pnnnn_SP}, // 0x08 "LD_(nnnn),SP"
        {.numBytes = 1, .cycles = 8, .text = "ADD HL,BC"},                 // 0x09 "ADD HL,BC"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(BC)"},                 // 0x0a "LD A,(BC)"
        {.numBytes = 1, .cycles = 8, .text = "DEC BC"},                    // 0x0b "DEC BC"
        {.numBytes = 1, .cycles = 4, .text = "INC C"},                     // 0x0c "INC C"
        {.numBytes = 1, .cycles = 4, .text = "DEC C"},                     // 0x0d "DEC C"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},       // 0x0e "LD C,n"
        {.numBytes = 1, .cycles = 4, .text = "RRCA"},                      // 0x0f "RRCA"

        {.numBytes = 2, .cycles = 4, .text = "STOP"},                     // 0x10 "STOP"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::LD_DE_nnnn}, // 0x11 "LD DE,nnnn"
        {.numBytes = 1, .cycles = 8, .text = "LD (DE),A"},                // 0x12 "LD (DE),A"
        {.numBytes = 1, .cycles = 8, .text = "INC DE"},                   // 0x13 "INC DE"
        {.numBytes = 1, .cycles = 4, .text = "INC D"},                    // 0x14 "INC D"
        {.numBytes = 1, .cycles = 4, .text = "DEC D"},                    // 0x15 "DEC D"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},      // 0x16 "LD D,N"
        {.numBytes = 1, .cycles = 4, .text = "RLA"},                      // 0x17 "RLA"
        {.numBytes = 2, .cycles = 12, .text = &Disassembler::JR_n},       // 0x18 "JR nn" (signed)
        {.numBytes = 1, .cycles = 8, .text = "ADD HL,DE"},                // 0x19 "ADD HL,DE"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(DE)"},                // 0x1a "LD A,(DE)"
        {.numBytes = 1, .cycles = 8, .text = "DEC DE"},                   // 0x1b "DEC DE"
        {.numBytes = 1, .cycles = 4, .text = "INC E"},                    // 0x1c "INC E"
        {.numBytes = 1, .cycles = 4, .text = "DEC E"},                    // 0x1d "DEC E"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},      // 0x1e "LD E,N"
        {.numBytes = 1, .cycles = 4, .text = "RLA"},                      // 0x1f "RRA"

        {.numBytes = 2, .cycles = 8, .text = &Disassembler::JR_cc},       // 0x20 "JR NZ nn"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::LD_HL_nnnn}, // 0x21 "LD HL,nnnn"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL+),A"},               // 0x22 "LD (HL+),A"
        {.numBytes = 1, .cycles = 8, .text = "INC HL"},                   // 0x23 "INC HL"
        {.numBytes = 1, .cycles = 4, .text = "INC H"},                    // 0x24 "INC H"
        {.numBytes = 1, .cycles = 4, .text = "DEC H"},                    // 0x25 "DEC H"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},      // 0x26 "LD H,N"
        {.numBytes = 1, .cycles = 4, .text = "DAA"},                      // 0x27 "DAA"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::JR_cc},       // 0x28 "JR Z nn"
        {.numBytes = 1, .cycles = 8, .text = "ADD HL,HL"},                // 0x29 "ADD HL,HL"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(HL+)"},               // 0x2a "LD A,(HL+)"
        {.numBytes = 1, .cycles = 8, .text = "DEC HL"},                   // 0x2b "DEC HL"
        {.numBytes = 1, .cycles = 4, .text = "INC L"},                    // 0x2c "INC L"
        {.numBytes = 1, .cycles = 4, .text = "DEC L"},                    // 0x2d "DEC L"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},      // 0x2e "LD L,n"
        {.numBytes = 1, .cycles = 4, .text = "CPL"},                      // 0x2f "CPL"

        {.numBytes = 2, .cycles = 8, .text = &Disassembler::JR_cc},       // 0x30 "JR NC nn",
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::LD_SP_nnnn}, // 0x31 "LD SP,nnnn",
        {.numBytes = 1, .cycles = 8, .text = "LD (HL-),A"},               // 0x32 "LD (HL-),A"
        {.numBytes = 1, .cycles = 8, .text = "INC SP"},                   // 0x33 "INC SP"
        {.numBytes = 1, .cycles = 12, .text = "INC (HL)"},                // 0x34 "INC (HL)"
        {.numBytes = 1, .cycles = 12, .text = "DEC (HL)"},                // 0x35 "DEC (HL)"
        {.numBytes = 2, .cycles = 12, .text = &Disassembler::LD_r_n},     // 0x36 "LD (HL),n"
        {.numBytes = 1, .cycles = 4, .text = "SCF"},                      // 0x37 "SCF"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::JR_cc},       // 0x38 "JR C nn"
        {.numBytes = 1, .cycles = 8, .text = "ADD HL,SP"},                // 0x39 "ADD HL,SP"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(HL-)"},               // 0x3a "LD A,(HL-)"
        {.numBytes = 1, .cycles = 8, .text = "DEC SP"},                   // 0x3b "DEC SP"
        {.numBytes = 1, .cycles = 4, .text = "INC A"},                    // 0x3c "INC A"
        {.numBytes = 1, .cycles = 4, .text = "DEC A"},                    // 0x3d "DEC A"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::LD_r_n},      // 0x3e "LD A,N"
        {.numBytes = 1, .cycles = 4, .text = "CCF"},                      // 0x3f "CCF"

        {.numBytes = 1, .cycles = 4, .text = "LD B,B"},    // 0x40 "LD B,B"
        {.numBytes = 1, .cycles = 4, .text = "LD B,C"},    // 0x41 "LD B,C"
        {.numBytes = 1, .cycles = 4, .text = "LD B,D"},    // 0x42 "LD B,D"
        {.numBytes = 1, .cycles = 4, .text = "LD B,E"},    // 0x43 "LD B,E"
        {.numBytes = 1, .cycles = 4, .text = "LD B,H"},    // 0x44 "LD B,H"
        {.numBytes = 1, .cycles = 4, .text = "LD B,L"},    // 0x45 "LD B,L"
        {.numBytes = 1, .cycles = 8, .text = "LD B,(HL)"}, // 0x46 "LD B,(hl)"
        {.numBytes = 1, .cycles = 4, .text = "LD B,A"},    // 0x47 "LD B,A"
        {.numBytes = 1, .cycles = 4, .text = "LD C,B"},    // 0x48 "LD C,B"
        {.numBytes = 1, .cycles = 4, .text = "LD C,C"},    // 0x49 "LD C,C"
        {.numBytes = 1, .cycles = 4, .text = "LD C,D"},    // 0x4a "LD C,D"
        {.numBytes = 1, .cycles = 4, .text = "LD C,E"},    // 0x4b "LD C,E"
        {.numBytes = 1, .cycles = 4, .text = "LD C,H"},    // 0x4c "LD C,H"
        {.numBytes = 1, .cycles = 4, .text = "LD C,L"},    // 0x4d "LD C,L"
        {.numBytes = 1, .cycles = 8, .text = "LD C,(HL)"}, // 0x4e "LD C,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "LD C,A"},    // 0x4f "LD C,A"

        {.numBytes = 1, .cycles = 4, .text = "LD D,B"},    // 0x50 "LD D,B"
        {.numBytes = 1, .cycles = 4, .text = "LD D,C"},    // 0x51 "LD D,C"
        {.numBytes = 1, .cycles = 4, .text = "LD D,D"},    // 0x52 "LD D,D"
        {.numBytes = 1, .cycles = 4, .text = "LD D,E"},    // 0x53 "LD D,E"
        {.numBytes = 1, .cycles = 4, .text = "LD D,H"},    // 0x54 "LD D,H"
        {.numBytes = 1, .cycles = 4, .text = "LD D,L"},    // 0x55 "LD D,L"
        {.numBytes = 1, .cycles = 8, .text = "LD D,(HL)"}, // 0x56 "LD D,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "LD D,A"},    // 0x57 "LD D,A"
        {.numBytes = 1, .cycles = 4, .text = "LD E,B"},    // 0x58 "LD E,B"
        {.numBytes = 1, .cycles = 4, .text = "LD E,C"},    // 0x59 "LD E,C"
        {.numBytes = 1, .cycles = 4, .text = "LD E,D"},    // 0x5a "LD E,D"
        {.numBytes = 1, .cycles = 4, .text = "LD E,E"},    // 0x5b "LD E,E"
        {.numBytes = 1, .cycles = 4, .text = "LD E,H"},    // 0x5c "LD E,H"
        {.numBytes = 1, .cycles = 4, .text = "LD E,L"},    // 0x5d "LD E,L"
        {.numBytes = 1, .cycles = 8, .text = "LD E,(HL)"}, // 0x5e "LD E,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "LD E,A"},    // 0x5f "LD E,A"

        {.numBytes = 1, .cycles = 4, .text = "LD H,B"},    // 0x60 "LD H,B"
        {.numBytes = 1, .cycles = 4, .text = "LD H,C"},    // 0x61 "LD H,C"
        {.numBytes = 1, .cycles = 4, .text = "LD H,D"},    // 0x62 "LD H,D"
        {.numBytes = 1, .cycles = 4, .text = "LD H,E"},    // 0x63 "LD H,E"
        {.numBytes = 1, .cycles = 4, .text = "LD H,H"},    // 0x64 "LD H,H"
        {.numBytes = 1, .cycles = 4, .text = "LD H,L"},    // 0x65 "LD H,L"
        {.numBytes = 1, .cycles = 8, .text = "LD H,(HL)"}, // 0x66 "LD H,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "LD H,A"},    // 0x67 "LD H,A"
        {.numBytes = 1, .cycles = 4, .text = "LD L,B"},    // 0x68 "LD L,B"
        {.numBytes = 1, .cycles = 4, .text = "LD L,C"},    // 0x69 "LD L,C"
        {.numBytes = 1, .cycles = 4, .text = "LD L,D"},    // 0x6a "LD L,D"
        {.numBytes = 1, .cycles = 4, .text = "LD L,E"},    // 0x6b "LD L,E"
        {.numBytes = 1, .cycles = 4, .text = "LD L,H"},    // 0x6c "LD L,H"
        {.numBytes = 1, .cycles = 4, .text = "LD L,L"},    // 0x6d "LD L,L"
        {.numBytes = 1, .cycles = 8, .text = "LD L,(HL)"}, // 0x6e "LD L,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "LD L,A"},    // 0x6f "LD L,A

        {.numBytes = 1, .cycles = 8, .text = "LD (HL),B"}, // 0x70 "LD (HL),B"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),C"}, // 0x71 "LD R,R"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),D"}, // 0x72 "LD R,R"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),E"}, // 0x73 "LD R,R"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),H"}, // 0x74 "LD R,R"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),L"}, // 0x75 "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "HALT"},      // 0x76 "HALT"
        {.numBytes = 1, .cycles = 8, .text = "LD (HL),A"}, // 0x77 "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,B"},    // 0x78 "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,C"},    // 0x79 "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,D"},    // 0x7a "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,E"},    // 0x7b "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,H"},    // 0x7c "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,L"},    // 0x7d "LD R,R"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(HL)"}, // 0x7e "LD R,R"
        {.numBytes = 1, .cycles = 4, .text = "LD A,A"},    // 0x7f "LD R,R"

        {.numBytes = 1, .cycles = 4, .text = "ADD A,B"},    // 0x80 "ADD A,B"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,C"},    // 0x81 "ADD A,C"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,D"},    // 0x82 "ADD A,D"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,E"},    // 0x83 "ADD A,E"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,H"},    // 0x84 "ADD A,H"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,L"},    // 0x85 "ADD A,L"
        {.numBytes = 1, .cycles = 8, .text = "ADD A,(HL)"}, // 0x86 "ADD A,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "ADD A,A"},    // 0x87 "ADD A,A"

        {.numBytes = 1, .cycles = 4, .text = "ADC A,B"},    // 0x88 "ADC A,B"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,C"},    // 0x89 "ADC A,C"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,D"},    // 0x8a "ADC A,D"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,E"},    // 0x8b "ADC A,E"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,H"},    // 0x8c "ADC A,H"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,L"},    // 0x8d "ADC A,L"
        {.numBytes = 1, .cycles = 8, .text = "ADC A,(HL)"}, // 0x8e "ADC A,(HL)"
        {.numBytes = 1, .cycles = 4, .text = "ADC A,A"},    // 0x8f "ADC A,A"

        {.numBytes = 1, .cycles = 4, .text = "SUB B"},    // 0x90 "SUB B"
        {.numBytes = 1, .cycles = 4, .text = "SUB C"},    // 0x91 "SUB C"
        {.numBytes = 1, .cycles = 4, .text = "SUB D"},    // 0x92 "SUB D"
        {.numBytes = 1, .cycles = 4, .text = "SUB E"},    // 0x93 "SUB E"
        {.numBytes = 1, .cycles = 4, .text = "SUB H"},    // 0x94 "SUB H"
        {.numBytes = 1, .cycles = 4, .text = "SUB L"},    // 0x95 "SUB L"
        {.numBytes = 1, .cycles = 8, .text = "SUB (HL)"}, // 0x96 "SUB (HL)"
        {.numBytes = 1, .cycles = 4, .text = "SUB A"},    // 0x97 "SUB A"

        {.numBytes = 1, .cycles = 4, .text = "SBC B"},    // 0x98 "SBC B"
        {.numBytes = 1, .cycles = 4, .text = "SBC C"},    // 0x99 "SBC C"
        {.numBytes = 1, .cycles = 4, .text = "SBC D"},    // 0x9a "SBC D"
        {.numBytes = 1, .cycles = 4, .text = "SBC E"},    // 0x9b "SBC E"
        {.numBytes = 1, .cycles = 4, .text = "SBC H"},    // 0x9c "SBC H"
        {.numBytes = 1, .cycles = 4, .text = "SBC L"},    // 0x9d "SBC L"
        {.numBytes = 1, .cycles = 8, .text = "SBC (HL)"}, // 0x9e "SBC (HL)"
        {.numBytes = 1, .cycles = 4, .text = "SBC A"},    // 0x9f "SBC A"

        {.numBytes = 1, .cycles = 4, .text = "AND B"},    // 0xa0 "AND B"
        {.numBytes = 1, .cycles = 4, .text = "AND C"},    // 0xa1 "AND C"
        {.numBytes = 1, .cycles = 4, .text = "AND D"},    // 0xa2 "AND D"
        {.numBytes = 1, .cycles = 4, .text = "AND E"},    // 0xa3 "AND E"
        {.numBytes = 1, .cycles = 4, .text = "AND H"},    // 0xa4 "AND H"
        {.numBytes = 1, .cycles = 4, .text = "AND L"},    // 0xa5 "AND L"
        {.numBytes = 1, .cycles = 8, .text = "AND (HL)"}, // 0xa6 "AND (HL)"
        {.numBytes = 1, .cycles = 4, .text = "AND A"},    // 0xa7 "AND A"

        {.numBytes = 1, .cycles = 4, .text = "XOR B"},    // 0xa8 "XOR B"
        {.numBytes = 1, .cycles = 4, .text = "XOR C"},    // 0xa9 "XOR C"
        {.numBytes = 1, .cycles = 4, .text = "XOR D"},    // 0xaa "XOR D"
        {.numBytes = 1, .cycles = 4, .text = "XOR E"},    // 0xab "XOR E"
        {.numBytes = 1, .cycles = 4, .text = "XOR H"},    // 0xac "XOR H"
        {.numBytes = 1, .cycles = 4, .text = "XOR L"},    // 0xad "XOR L"
        {.numBytes = 1, .cycles = 8, .text = "XOR (HL)"}, // 0xae "XOR (HL)"
        {.numBytes = 1, .cycles = 4, .text = "XOR A"},    // 0xaf "XOR A"

        {.numBytes = 1, .cycles = 4, .text = "OR B"},    // 0xb0 "OR B"
        {.numBytes = 1, .cycles = 4, .text = "OR C"},    // 0xb1 "OR C"
        {.numBytes = 1, .cycles = 4, .text = "OR D"},    // 0xb2 "OR D"
        {.numBytes = 1, .cycles = 4, .text = "OR E"},    // 0xb3 "OR E"
        {.numBytes = 1, .cycles = 4, .text = "OR H"},    // 0xb4 "OR H"
        {.numBytes = 1, .cycles = 4, .text = "OR L"},    // 0xb5 "OR L"
        {.numBytes = 1, .cycles = 4, .text = "OR (HL)"}, // 0xb6 "OR (HL)"
        {.numBytes = 1, .cycles = 4, .text = "OR A"},    // 0xb7 "OR A"

        {.numBytes = 1, .cycles = 4, .text = "CP B"},    // 0xb8 "CP B"
        {.numBytes = 1, .cycles = 4, .text = "CP C"},    // 0xb9 "CP C"
        {.numBytes = 1, .cycles = 4, .text = "CP D"},    // 0xba "CP D"
        {.numBytes = 1, .cycles = 4, .text = "CP E"},    // 0xbb "CP E"
        {.numBytes = 1, .cycles = 4, .text = "CP H"},    // 0xbc "CP H"
        {.numBytes = 1, .cycles = 4, .text = "CP L"},    // 0xbd "CP L"
        {.numBytes = 1, .cycles = 8, .text = "CP (HL)"}, // 0xbe "CP (HL)"
        {.numBytes = 1, .cycles = 4, .text = "CP A"},    // 0xbf "CP A"

        {.numBytes = 1, .cycles = 8, .text = "RET NZ"},                              // 0xc0 "RET NZ" TODO: cycles=20 if returning
        {.numBytes = 1, .cycles = 12, .text = "POP BC"},                             // 0xc1 "POP BC"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::JP_cc_nnnn},            // 0xc2 "JP NZ nnnn"
        {.numBytes = 3, .cycles = 16, .text = &Disassembler::JP_nnnn},               // 0xc3 "JP NN"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::CALL_cc_nnnn},          // 0xc4 "CALL NZ nnnn"
        {.numBytes = 1, .cycles = 16, .text = "PUSH BC"},                            // 0xc5 "PUSH BC"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::ADD_A_n},                // 0xc6 "ADD A,n"
        {.numBytes = 1, .cycles = 16, .text = "RST 00h"},                            // 0xc7 "RST 00h"
        {.numBytes = 1, .cycles = 8, .text = "RET Z"},                               // 0xc8 "RET Z"
        {.numBytes = 1, .cycles = 16, .text = "RET"},                                // 0xc9 "RET"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::JP_cc_nnnn},            // 0xca "JP Z nnnn"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::decode_bit_instruction}, // 0xcb "BIT opcode group" // TODO: cycles are 16 when dealing with a [HL] instruction, but we can't change that pr. extended instructions as they are hardcoded pr. instruction
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::CALL_cc_nnnn},          // 0xcc "CALL Z,nnnn"
        {.numBytes = 3, .cycles = 24, .text = &Disassembler::CALL_nnnn},             // 0xcd "CALL nnnn"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::ADC_A_n},                // 0xce "ADC A,N"
        {.numBytes = 1, .cycles = 16, .text = "RST 08h"},                            // 0xcf "RST 08h"

        {.numBytes = 1, .cycles = 8, .text = "RET NC"},                      // 0xd0 "RET NC"
        {.numBytes = 1, .cycles = 12, .text = "POP DE"},                     // 0xd1 "POP DE"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::JP_cc_nnnn},    // 0xd2 "JP NC,nnnn"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xd3 "OUT (N),A"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::CALL_cc_nnnn},  // 0xd4 "CALL NC,nnnn"
        {.numBytes = 1, .cycles = 16, .text = "PUSH DE"},                    // 0xd5 "PUSH DE"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::SUB_n},          // 0xd6 "SUB N"
        {.numBytes = 1, .cycles = 16, .text = "RST 10h"},                    // 0xd7 "RST 10h"
        {.numBytes = 1, .cycles = 8, .text = "RET C"},                       // 0xd8 "RET C"
        {.numBytes = 1, .cycles = 16, .text = "RETI"},                       // 0xd9 "RETI"
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::JP_cc_nnnn},    // 0xda "JP C,nnnn"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xdb
        {.numBytes = 3, .cycles = 12, .text = &Disassembler::CALL_cc_nnnn},  // 0xdc "CALL C,nnnn"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xdd
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::SBC_n},          // 0xde "SBC N"
        {.numBytes = 1, .cycles = 16, .text = "RST 18h"},                    // 0xdf "RST 18h"

        {.numBytes = 2, .cycles = 12, .text = &Disassembler::LD_ff00n_A},    // 0xe0 LD_ff00 + n, A
        {.numBytes = 1, .cycles = 12, .text = "POP HL"},                     // 0xe1 "POP HL"
        {.numBytes = 1, .cycles = 8, .text = "LD (0xFF00 + C),A"},           // 0xe2 "LD (FF00 + c),a"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xe3 -
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xe4 -
        {.numBytes = 1, .cycles = 16, .text = "PUSH HL"},                    // 0xe5 "PUSH HL"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::AND_n},          // 0xe6 "AND N"
        {.numBytes = 1, .cycles = 16, .text = "RST 20h"},                    // 0xe7 "RST 20h"
        {.numBytes = 2, .cycles = 16, .text = &Disassembler::ADD_SP_s8},     // 0xe8 "ADD SP,s8" (signed)
        {.numBytes = 1, .cycles = 4, .text = "JP (HL)"},                     // 0xe9 "JP (HL)"
        {.numBytes = 3, .cycles = 16, .text = &Disassembler::LD_pnnnn_A},    // 0xea "LD (nnnn),A"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xeb
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xec
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xed
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::XOR_n},          // 0xee "XOR N"
        {.numBytes = 1, .cycles = 16, .text = "RST 28h"},                    // 0xef "RST 28h"

        {.numBytes = 2, .cycles = 12, .text = &Disassembler::LD_A_ff00n},    // 0xf0 "LD A,(ff00 + n)"
        {.numBytes = 1, .cycles = 12, .text = "POP AF"},                     // 0xf1 "POP AF"
        {.numBytes = 1, .cycles = 8, .text = "LD A,(0xff00 + C)"},           // 0xf2 "LD A,(ff00 + C)"
        {.numBytes = 1, .cycles = 4, .text = "DI"},                          // 0xf3 "DI"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xf4
        {.numBytes = 1, .cycles = 16, .text = "PUSH AF"},                    // 0xf5 "PUSH AF"
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::OR_n},           // 0xf6 "OR N"
        {.numBytes = 1, .cycles = 16, .text = "RST 30h"},                    // 0xf7 "RST 30h"
        {.numBytes = 2, .cycles = 12, .text = &Disassembler::LD_HL_SPs8},    // 0xf8 "LD HL,SP+s8"
        {.numBytes = 1, .cycles = 8, .text = "LD SP,HL"},                    // 0xf9 "LD SP,HL"
        {.numBytes = 3, .cycles = 16, .text = &Disassembler::LD_A_pnnnn},    // 0xfa "LD A,(nnnn)"
        {.numBytes = 1, .cycles = 4, .text = "EI"},                          // 0xfb "EI"
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xfc
        {.numBytes = 1, .cycles = 0, .text = &Disassembler::invalid_opcode}, // 0xfd
        {.numBytes = 2, .cycles = 8, .text = &Disassembler::CP_n},           // 0xfe "CP N"
        {.numBytes = 1, .cycles = 16, .text = "RST 38"}                      // 0xff "RST 38h"
    };
}

DisassemblyLine Disassembler::DisassembleAddress(uint16_t address, const HostMemory &hostmem)
{
    InstructionBytes bytes = {hostmem.Read(address), hostmem.Read(address + 1), hostmem.Read(address + 2)};
    Instruction &inst = instructions[bytes.data[0]]; // todo: bound check
    DisassemblyLine line;

    if (std::holds_alternative<DisAsmFunc>(inst.text))
    {
        line.text = std::get<DisAsmFunc>(inst.text)(bytes);
    }
    else
    {
        line.text = std::get<std::string>(inst.text);
    }

    line.cycles = inst.cycles;
    line.instructionBytes = bytes;
    line.numberOfBytes = inst.numBytes;
    line.PC = address;

    return std::move(line);
}

std::string Disassembler::invalid_opcode(const InstructionBytes &bytes)
{
    return std::format("Invalid opcode ({:02X})", bytes.data[0]);
}