#include <cstdint>
#include <iostream>

#include "cpu.h"

#include "bit_instructions.h"
#include "load_instructions.h"
#include "jump_instructions.h"
#include "arithmetic_instructions.h"
#include "general_instructions.h"


CPU::CPU(HostMemory& mem) : mem(mem) {

    // Based on tables from:
    // https://clrhome.org/table/

    instructions = {
        { 4, &CPU::NOP},        // 0x00 NOP
        {12, &CPU::LD_BC_nn},   // 0x01 LD BC,NN
        { 8, &CPU::LD_pBC_A},   // 0x02 "LD (BC),A"
        { 8, &CPU::INC_BC},     // 0x03 "INC BC"
        { 4, &CPU::INC_B},      // 0x04 {"INC B"
        { 4, &CPU::DEC_B},      // 0x05 "DEC B"
        { 8, &CPU::LD_r_n},     // 0x06 "LD B,N"
        { 4, &CPU::rlca},       // 0x07 "RLCA"
        {20, &CPU::LD_pnn_SP},  // 0x08 "EXAF"
        { 8, &CPU::add_hl_bc},  // 0x09 "ADD HL,BC"
        { 8, &CPU::LD_A_pBC},   // 0x0a "LD A,(BC)"
        { 8, &CPU::dec_bc},     // 0x0b "DEC BC"
        { 4, &CPU::inc_c},      // 0x0c "INC C"
        { 4, &CPU::dec_c},      // 0x0d "DEC C"
        { 8, &CPU::LD_r_n},     // 0x0e {"LD C,N"
        { 4, &CPU::rrca},       // 0x0f "RRCA"

        { 4, &CPU::STOP},       // 0x10 "DJNZ N"
        {12, &CPU::LD_DE_nn},   // 0x11 "LD DE,NN"
        { 8, &CPU::LD_pDE_A},   // 0x12 "LD (DE),A"
        { 8, &CPU::INC_DE},     // 0x13 "INC DE"
        { 4, &CPU::inc_d},      // 0x14 "INC D"
        { 4, &CPU::dec_d},      // 0x15 "DEC D"
        { 8, &CPU::LD_r_n},     // 0x16 "LD D,N"
        { 4, &CPU::rla},        // 0x17 "RLA"
        {12, &CPU::jr_n},       // 0x18 "JR N"
        { 8, &CPU::ADD_HL_DE},  // 0x19 "ADD HL,DE"
        { 8, &CPU::LD_A_pDE},   // 0x1a "LD A,(DE)"
        { 8, &CPU::DEC_DE},     // 0x1b "DEC DE"
        { 4, &CPU::INC_E},      // 0x1c "INC E"
        { 4, &CPU::DEC_E},      // 0x1d "DEC E"
        { 8, &CPU::LD_r_n},     // 0x1e "LD E,N"
        { 4, &CPU::rra},        // 0x1f "RRA"

        { 8, &CPU::jr_nz},      // 0x20 "JR NZ"
        {12, &CPU::LD_HL_nn},   // 0x21 "LD HL,NN"
        { 8, &CPU::LDI_pHL_A},  // 0x22 "LD (NN),HL"
        { 8, &CPU::INC_HL},     // 0x23 "INC HL"
        { 4, &CPU::INC_H},      // 0x24 "INC H"
        { 4, &CPU::DEC_H},      // 0x25 "DEC H"
        { 8, &CPU::LD_r_n},     // 0x26 "LD H,N"
        { 4, &CPU::daa},        // 0x27 "DAA"
        { 8, &CPU::jr_z},       // 0x28 "JR Z"
        { 8, &CPU::ADD_HL_HL},  // 0x29 "ADD HL,HL"
        { 8, &CPU::LDI_A_pHL},  // 0x2a "LD HL,(NN)"
        { 8, &CPU::DEC_HL},     // 0x2b "DEC HL"
        { 4, &CPU::INC_L},      // 0x2c "INC L"
        { 4, &CPU::DEC_L},      // 0x2d "DEC L"
        { 8, &CPU::LD_r_n},     // 0x2e "LD L,N"
        { 4, &CPU::cpl},        // 0x2f "CPL"

        { 8, &CPU::jr_nc},      // 0x30 "JR NC",
        {12, &CPU::LD_SP_nn},   // 0x31 "LD SP,NN",
        { 8, &CPU::LDD_pHL_A},  // 0x32 "LD (NN),A"
        { 8, &CPU::INC_SP},     // 0x33 "INC SP"
        {12, &CPU::INC_pHL},    // 0x34 "INC (HL)"
        {12, &CPU::DEC_pHL},    // 0x35 "DEC (HL)"
        {12, &CPU::LD_pHL_n},   // 0x36 "LD (HL),N"
        { 4, &CPU::scf},        // 0x37 "SCF"
        {12, &CPU::jr_c},       // 0x38 "JR C,N"
        { 8, &CPU::add_hl_sp},  // 0x39 "ADD HL,SP"
        { 8, &CPU::LDD_A_pHL},  // 0x3a "LD A,(NN)"
        { 8, &CPU::DEC_SP},     // 0x3b "DEC SP"
        { 4, &CPU::INC_A},      // 0x3c {"INC A"
        { 4, &CPU::DEC_A},      // 0x3d "DEC A"
        { 8, &CPU::LD_r_n},     // 0x3e "LD A,N"
        { 4, &CPU::ccf},        // 0x3f "CCF"

        {4, &CPU::LD_r_r},      // 0x40 "LD B,B"
        {4, &CPU::LD_r_r},      // 0x41 "LD B,C"
        {4, &CPU::LD_r_r},      // 0x42 "LD B,D"
        {4, &CPU::LD_r_r},      // 0x43 "LD B,E"
        {4, &CPU::LD_r_r},      // 0x44 "LD B,H"
        {4, &CPU::LD_r_r},      // 0x45 "LD B,L"
        {8, &CPU::LD_r_r},      // 0x46 "LD B,(hl)"
        {4, &CPU::LD_r_r},      // 0x47 "LD B,A"
        {4, &CPU::LD_r_r},      // 0x48 "LD C,B"
        {4, &CPU::LD_r_r},      // 0x49 "LD C,C"
        {4, &CPU::LD_r_r},      // 0x4a "LD C,D"
        {4, &CPU::LD_r_r},      // 0x4b "LD C,E"
        {4, &CPU::LD_r_r},      // 0x4c "LD C,H"
        {4, &CPU::LD_r_r},      // 0x4d "LD C,L"
        {8, &CPU::LD_r_r},      // 0x4e "LD C,(HL)"
        {4, &CPU::LD_r_r},      // 0x4f "LD C,A"

        {4, &CPU::LD_r_r},      // 0x50 "LD D,B"
        {4, &CPU::LD_r_r},      // 0x51 "LD D,C"
        {4, &CPU::LD_r_r},      // 0x52 "LD D,D"
        {4, &CPU::LD_r_r},      // 0x53 "LD D,E"
        {4, &CPU::LD_r_r},      // 0x54 "LD D,H"
        {4, &CPU::LD_r_r},      // 0x55 "LD D,L"
        {8, &CPU::LD_r_r},      // 0x56 "LD D,(HL)"
        {4, &CPU::LD_r_r},      // 0x57 "LD D,A"
        {4, &CPU::LD_r_r},      // 0x58 "LD E,B"
        {4, &CPU::LD_r_r},      // 0x59 "LD E,C"
        {4, &CPU::LD_r_r},      // 0x5a "LD E,D"
        {4, &CPU::LD_r_r},      // 0x5b "LD E,E"
        {4, &CPU::LD_r_r},      // 0x5c "LD E,H"
        {4, &CPU::LD_r_r},      // 0x5d "LD E,L"
        {8, &CPU::LD_r_r},      // 0x5e "LD E,(HL)"
        {4, &CPU::LD_r_r},      // 0x5f "LD E,A"

        {4, &CPU::LD_r_r},      // 0x60 "LD H,B"
        {4, &CPU::LD_r_r},      // 0x61 "LD H,C"
        {4, &CPU::LD_r_r},      // 0x62 "LD H,D"
        {4, &CPU::LD_r_r},      // 0x63 "LD H,E"
        {4, &CPU::LD_r_r},      // 0x64 "LD H,H"
        {4, &CPU::LD_r_r},      // 0x65 "LD H,L"
        {8, &CPU::LD_r_r},      // 0x66 "LD H,(HL)"
        {4, &CPU::LD_r_r},      // 0x67 "LD H,A"
        {4, &CPU::LD_r_r},      // 0x68 "LD L,B"
        {4, &CPU::LD_r_r},      // 0x69 "LD L,C"
        {4, &CPU::LD_r_r},      // 0x6a "LD L,D"
        {4, &CPU::LD_r_r},      // 0x6b "LD L,E"
        {4, &CPU::LD_r_r},      // 0x6c "LD L,H"
        {4, &CPU::LD_r_r},      // 0x6d "LD L,L"
        {8, &CPU::LD_r_r},      // 0x6e "LD L,(HL)"
        {4, &CPU::LD_r_r},      // 0x6f "LD L,A

        {8, &CPU::LD_r_r},      // 0x70 "LD R,R"
        {8, &CPU::LD_r_r},      // 0x71 "LD R,R"
        {8, &CPU::LD_r_r},      // 0x72 "LD R,R"
        {8, &CPU::LD_r_r},      // 0x73 "LD R,R"
        {8, &CPU::LD_r_r},      // 0x74 "LD R,R"
        {8, &CPU::LD_r_r},      // 0x75 "LD R,R"
        {4, &CPU::halt},        // 0x76 "HALT"
        {8, &CPU::LD_r_r},      // 0x77 "LD R,R"
        {4, &CPU::LD_r_r},      // 0x78 "LD R,R"
        {4, &CPU::LD_r_r},      // 0x79 "LD R,R"
        {4, &CPU::LD_r_r},      // 0x7a "LD R,R"
        {4, &CPU::LD_r_r},      // 0x7b "LD R,R"
        {4, &CPU::LD_r_r},      // 0x7c "LD R,R"
        {4, &CPU::LD_r_r},      // 0x7d "LD R,R"
        {8, &CPU::LD_r_r},      // 0x7e "LD R,R"
        {4, &CPU::LD_r_r},      // 0x7f "LD R,R"

        {4, &CPU::add_a_r},          // 0x80 "ADD A,B"
        {4, &CPU::add_a_r},          // 0x81 "ADD A,C"
        {4, &CPU::add_a_r},          // 0x82 "ADD A,D"
        {4, &CPU::add_a_r},          // 0x83 "ADD A,E"
        {4, &CPU::add_a_r},          // 0x84 "ADD A,H"
        {4, &CPU::add_a_r},          // 0x85 "ADD A,L"
        {8, &CPU::add_a_r},          // 0x86 "ADD A,(HL)"
        {4, &CPU::add_a_r},          // 0x87 "ADD A,A"

        {4, &CPU::adc_a_r},          // 0x88 "ADC A,B"
        {4, &CPU::adc_a_r},          // 0x89 "ADC A,C"
        {4, &CPU::adc_a_r},          // 0x8a "ADC A,D"
        {4, &CPU::adc_a_r},          // 0x8b "ADC A,E"
        {4, &CPU::adc_a_r},          // 0x8c "ADC A,H"
        {4, &CPU::adc_a_r},          // 0x8d "ADC A,L"
        {8, &CPU::adc_a_r},          // 0x8e "ADC A,(HL)"
        {4, &CPU::adc_a_r},          // 0x8f "ADC A,A"

        {4, &CPU::sub_r},              // 0x90 "SUB B"
        {4, &CPU::sub_r},              // 0x91 "SUB C"
        {4, &CPU::sub_r},              // 0x92 "SUB D"
        {4, &CPU::sub_r},              // 0x93 "SUB E"
        {4, &CPU::sub_r},              // 0x94 "SUB H"
        {4, &CPU::sub_r},              // 0x95 "SUB L"
        {8, &CPU::sub_r},              // 0x96 "SUB (HL)"
        {4, &CPU::sub_r},              // 0x97 "SUB A"

        {4, &CPU::sbc_r},              // 0x98 "SBC B"
        {4, &CPU::sbc_r},              // 0x99 "SBC C"
        {4, &CPU::sbc_r},              // 0x9a "SBC D"
        {4, &CPU::sbc_r},              // 0x9b "SBC E"
        {4, &CPU::sbc_r},              // 0x9c "SBC H"
        {4, &CPU::sbc_r},              // 0x9d "SBC L"
        {8, &CPU::sbc_r},              // 0x9e "SBC (HL)"
        {4, &CPU::sbc_r},              // 0x9f "SBC A"

        {4, &CPU::and_r},             // 0xa0 "AND B"
        {4, &CPU::and_r},             // 0xa1 "AND C"
        {4, &CPU::and_r},             // 0xa2 "AND D"
        {4, &CPU::and_r},             // 0xa3 "AND E"
        {4, &CPU::and_r},             // 0xa4 "AND H"
        {4, &CPU::and_r},             // 0xa5 "AND L"
        {8, &CPU::and_r},             // 0xa6 "AND (HL)"
        {4, &CPU::and_r},             // 0xa7 "AND A"

        {4, &CPU::xor_r},             // 0xa8 "XOR B"
        {4, &CPU::xor_r},             // 0xa9 "XOR C"
        {4, &CPU::xor_r},             // 0xaa "XOR D"
        {4, &CPU::xor_r},             // 0xab "XOR E"
        {4, &CPU::xor_r},             // 0xac "XOR H"
        {4, &CPU::xor_r},             // 0xad "XOR L"
        {8, &CPU::xor_r},             // 0xae "XOR (HL)"
        {4, &CPU::xor_r},             // 0xaf "XOR A"

        {4, &CPU::or_r},             // 0xb0 "OR B"
        {4, &CPU::or_r},             // 0xb1 "OR C"
        {4, &CPU::or_r},             // 0xb2 "OR D"
        {4, &CPU::or_r},             // 0xb3 "OR E"
        {4, &CPU::or_r},             // 0xb4 "OR H"
        {4, &CPU::or_r},             // 0xb5 "OR L"
        {8, &CPU::or_r},             // 0xb6 "OR (HL)"
        {4, &CPU::or_r},             // 0xb7 "OR A"

        {4, &CPU::CP_r},             // 0xb8 "CP B"
        {4, &CPU::CP_r},             // 0xb9 "CP C"
        {4, &CPU::CP_r},             // 0xba "CP D"
        {4, &CPU::CP_r},             // 0xbb "CP E"
        {4, &CPU::CP_r},             // 0xbc "CP H"
        {4, &CPU::CP_r},             // 0xbd "CP L"
        {8, &CPU::CP_r},             // 0xbe "CP (HL)"
        {4, &CPU::CP_r},             // 0xbf "CP A"

        { 8, &CPU::RET_cc},            // 0xc0 "RET NZ"
        {12, &CPU::pop_qq},            // 0xc1 "POP BC"
        {12, &CPU::jp_cc_nn},        // 0xc2 "JP NZ NN"
        {16, &CPU::jp_nn},              // 0xc3 "JP NN"
        {12, &CPU::call_cc_nn},    // 0xc4 "CALL NZ,NN"
        {16, &CPU::push_bc},          // 0xc5 "PUSH BC"
        { 8, &CPU::add_a_n},          // 0xc6 "ADD A,n"
        {16, &CPU::rst},              // 0xc7 "RST 00h"
        { 8, &CPU::RET_cc},             // 0xc8 "RET Z"
        {16, &CPU::RET},                  // 0xc9 "RET"
        {12, &CPU::jp_cc_nn},         // 0xca "JP Z,**"
        { 4, &CPU::decode_bit_instruction}, // 0xcb "BIT opcode group"
        {12, &CPU::call_cc_nn},     // 0xcc "CALL Z,nn"
        {24, &CPU::call},             // 0xcd "CALL nn"
        { 8, &CPU::adc_a_n},          // 0xce "ADC A,N"
        {16, &CPU::rst},              // 0xcf "RST 08h"

        { 8, &CPU::RET_cc},         // 0xd0 "RET NC"
        {12, &CPU::pop_qq},         // 0xd1 "POP DE"
        {12, &CPU::jp_cc_nn},       // 0xd2 "JP NC,NN"
        { 0, &CPU::invalid_opcode}, // 0xd3 "OUT (N),A"
        {12, &CPU::call_cc_nn},     // 0xd4 "CALL NC,NN"
        {16, &CPU::push_de},        // 0xd5 "PUSH DE"
        { 8, &CPU::sub_n},          // 0xd6 "SUB N"
        {16, &CPU::rst},            // 0xd7 "RST 10h"
        { 8, &CPU::RET_cc},         // 0xd8 "RET C"
        {16, &CPU::RETI},           // 0xd9 "EXX"
        {12, &CPU::jp_cc_nn},       // 0xda "JP C,NN"
        { 0, &CPU::invalid_opcode}, // 0xdb "IN A,(n)"
        {12, &CPU::call_cc_nn},     // 0xdc "CALL C,NN"
        { 0, &CPU::invalid_opcode}, // 0xdd
        { 8, &CPU::sbc_n},          // 0xde "SBC N"
        {16, &CPU::rst},            // 0xdf "RST 18h"

        {12, &CPU::LD_ff00n_A},     // 0xe0 "RET PO"
        {12, &CPU::pop_qq},         // 0xe1 "POP HL"
        { 8, &CPU::LD_ff00C_A},     // 0xe2 "JP PO,NN"
        { 0, &CPU::invalid_opcode}, // 0xe3 -
        { 0, &CPU::invalid_opcode}, // 0xe4 -
        {16, &CPU::push_hl},        // 0xe5 "PUSH HL"
        { 8, &CPU::and_n},          // 0xe6 "AND N"
        {16, &CPU::rst},            // 0xe7 "RST 20h"
        {16, &CPU::ADD_SP_s8},      // 0xe8 "RET PE"
        { 4, &CPU::jp_ptr_hl},      // 0xe9 "JP (HL)"
        {16, &CPU::LD_pnn_A},       // 0xea "JP PE,NN"
        { 0, &CPU::invalid_opcode}, // 0xeb "EX DH,HL"
        { 0, &CPU::invalid_opcode}, // 0xec "CALL PE,NN"
        { 0, &CPU::invalid_opcode}, // 0xed Extended instructions
        { 8, &CPU::xor_n},          // 0xee "XOR N"
        {16, &CPU::rst},            // 0xef "RST 28h"

        {12, &CPU::LD_A_ff00n},        // 0xf0 "RET P"
        {12, &CPU::pop_qq},            // 0xf1 "POP AF"
        { 8, &CPU::LD_A_ff00C},        // 0xf2 "JP P,NN"
        { 4, &CPU::disable_interrupts},// 0xf3 "DI"
        { 0, &CPU::invalid_opcode},    // 0xf4 "CALL P,NN"
        {16, &CPU::push_af},           // 0xf5 "PUSH AF"
        { 8, &CPU::or_n},              // 0xf6 "OR N"
        {16, &CPU::rst},               // 0xf7 "RST 30h"
        {12, &CPU::LD_HL_SPs8},        // 0xf8 "RET M"
        { 8, &CPU::ld_sp_hl},          // 0xf9 "LD SP,HL"
        {16, &CPU::LD_A_pnn},          // 0xfa "JP M,NN"
        { 4, &CPU::enable_interrupts}, // 0xfb "EI"
        { 0, &CPU::invalid_opcode},    // 0xfc "CALL M,NN"
        { 0, &CPU::invalid_opcode},    // 0xfd
        { 8, &CPU::CP_n},              // 0xfe "CP N"
        {16, &CPU::rst}                // 0xff "RST 38h"
    };

}

// opcodes: 0xCB ..
void CPU::decode_bit_instruction()
{
    uint8_t op2 = fetch8BitValue(); // fetch next opcode
    uint8_t& reg = reg_from_regcode(op2 & 0b00000111);
    // Get the number of cycles that the bit instruction spent
    additional_cycles_spent += do_bit_instruction(op2, reg);
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

void CPU::AddDebugLog(const char *text, va_list args)
{
    constexpr int buffer_length = 200;
    char buffer[buffer_length];
    snprintf(buffer, buffer_length, text,args);
    std::string buffAsStdStr = buffer;
    // TODO: also add opcodes
    //std::cout << current_pc << ": " << text << std::endl;
    debug_log_entries.push_back({current_pc, {current_opcode,0,0,0}, text});
}



uint8_t CPU::step()
{
    // M1: OP Code fetch
    current_pc = regs.PC;
    current_opcode = fetch8BitValue();

#ifdef DEBUG_LOG
    std::cout << std::nouppercase << std::showbase << std::hex << regs.PC-1 << "[" << static_cast<int>(current_opcode) << "] ";
#endif
    (this->*instructions[current_opcode].code)();

    // Cycles spent in this step = base instruction cycles + additional cycles spent (i.e. jumps and conditions taking longer depending on outcome)
    uint8_t cycles_spent = this->instructions[current_opcode].cycles + additional_cycles_spent;

    additional_cycles_spent = 0;
    // TODO: wait `cycles_spent` number of cycles
    return cycles_spent;
};

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
    std::cout << "INVALID OPCODE: " << (int)current_opcode << std::endl;
#endif
}