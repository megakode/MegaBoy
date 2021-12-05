//
// Created by sbeam on 03/12/2021.
//

#pragma once

// cycles: 10
void CPU::jp_cc_nn()
{
    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)){
        specialRegs.PC = location;
#ifdef DEBUG_LOG
        AddDebugLog(std::format("JP {},{:#06x} (true)",name_from_condition(conditionCode),location));
#endif
    }
    else
    {
#ifdef DEBUG_LOG
        AddDebugLog(std::format("JP {},{:#06x} (false)",name_from_condition(conditionCode),location));
#endif
    }

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
    int8_t jumpOffset = static_cast<int8_t>(fetch8BitValue());
    if(regs.B != 0) {
        specialRegs.PC -= 2; // measure from the start of this instruction opcode
        specialRegs.PC += jumpOffset;
    }

    #ifdef DEBUG_LOG
    AddDebugLog(std::format("DJNZ {}", jumpOffset));
    #endif
}

// JR NC - Jump if not carry
// opcode: 0x30
void CPU::jr_nc()
{
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if(!(regs.F & FlagBitmaskC)){ // If zero
        specialRegs.PC += offset;
    }

#ifdef DEBUG_LOG
    AddDebugLog(std::format("JR NC,{}",offset));
#endif
}

// JR C - jump if carry flag is set
// opcode: 0x38
// flags: -
void CPU::jr_c()
{
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if((regs.F & FlagBitmaskC)){ // If zero
        specialRegs.PC -= 2; // start calculation from beginning of this instruction
        specialRegs.PC += offset;
    }
#ifdef DEBUG_LOG
    AddDebugLog(std::format("JR C,{}",offset));
#endif
}


// jr z,n
// opcode: 0x28
// cycles: 12/7
// flags: -
void CPU::jr_z(){
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if((regs.F & FlagBitmaskZero)){ // If zero
        specialRegs.PC += offset;
    }

#ifdef DEBUG_LOG
    AddDebugLog(std::format("JR Z,{}",offset));
#endif
}

// JR NZ
// opcode: 0x20
// cycles: 12/7
void CPU::jr_nz(){
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if(!(regs.F & FlagBitmaskZero)){ // If not zero
        specialRegs.PC += offset; // start calculation from beginning of this instruction
    }

#ifdef DEBUG_LOG
    AddDebugLog(std::format("JR NZ,{}",offset));
#endif
}


/// jr n - Jump relative
/// opcode: 0x18
/// cycles: 12
void CPU::jr_n(){
    int8_t jumpOffset =  static_cast<int8_t>(fetch8BitValue());
    specialRegs.PC -= 2; // start calculation from beginning of this instruction
    specialRegs.PC += jumpOffset;
}

// JP nn - Jump absolute
// opcode: 0xc3
// cycles: 10
void CPU::jp_nn()
{
    specialRegs.PC = fetch16BitValue();

#ifdef DEBUG_LOG
    AddDebugLog(std::format("JP {}",specialRegs.PC));
#endif
}

// JP IX
// opcode: 0xdd 0xe9
// flags: -
// cycles: 8
void CPU::jp_IX()
{
    specialRegs.PC = specialRegs.IX;
}

// JP IY
// opcode: 0xfd 0xe9
// flags: -
// cycles: 8
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

// CALL cc,nn
// opcode: 0xcc nn nn
// cycles: 17 (true) 10 (false)
// flags: -
void CPU::call_cc_nn()
{
    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)){
        mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
        mem[--specialRegs.SP] = static_cast<uint8_t >(specialRegs.PC);      // (SP-2) = PC_h
        specialRegs.PC = location;
    }
}

// CALL
// opcode: 0xCD
// cycles: 17
void CPU::call(){
    uint16_t location = fetch16BitValue();
    mem[--specialRegs.SP] = specialRegs.PC >> 8; // (SP-1) = PC_h
    mem[--specialRegs.SP] = static_cast<uint8_t>(specialRegs.PC);      // (SP-2) = PC_h
    specialRegs.PC = location;

#ifdef DEBUG_LOG
    std::cout << "CALL " << location << std::endl;
#endif
}

// RET
// PC_l = (SP), PC_h = (SP+1)
// opcode: 0xc9
// cycles: 10
// flags: -
void CPU::ret()
{
    uint8_t lobyte = mem[specialRegs.SP++];
    uint8_t hibyte = mem[specialRegs.SP++];
    specialRegs.PC = (hibyte<<8) + lobyte;
#ifdef DEBUG_LOG
    std::cout << "RET [PC=" << specialRegs.PC << "]" << std::endl;
#endif
}


// RETN - "used at the end of a non-maskable interrupt rountine (located at 0x0066)"
// opcode: 0xed 0x45
// cycles: 14
void CPU::retn()
{
    ret();
    IFF = IFF2;

}

// RETI
// opcode: 0xed 0x4d
// cycles: 14
// flags: -
void CPU::reti()
{
    ret();
}


// RET cc (NZ/Z/NC/C/PO/PE/P/N)
// cycles: 11 (true) 5 (false)
// flags: -
void CPU::ret_cc(){

    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
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


// RST
//
// current PC is pushed onto stack, and PC is reset to an offset based on t,
// where t is contained in the opcode: (11 ttt 111).
//
// cycles: 11
void CPU::rst()
{
    uint8_t location[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};
    uint8_t locationCode = (current_opcode & 0b00111000) >> 3;

    mem[--specialRegs.SP] = specialRegs.PC >> 8;
    mem[--specialRegs.SP] = static_cast<uint8_t>(specialRegs.PC);

    specialRegs.PC = location[locationCode];
}