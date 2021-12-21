//
// Created by sbeam on 03/12/2021.
//

#pragma once

/// jp cc nn
/// cycles: 10
void CPU::jp_cc_nn()
{
    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    uint16_t location = fetch16BitValue();

    if(is_condition_true(conditionCode)) {
        regs.PC = location;
    }

#ifdef DEBUG_LOG
        AddDebugLog("JP %s,%04x",name_from_condition(conditionCode).c_str(),location);
#endif

}

// JR NC - Jump if not carry
// opcode: 0x30
void CPU::jr_nc()
{
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if(!(regs.F & FlagBitmaskC)){
        regs.PC += offset;
    }

#ifdef DEBUG_LOG
    AddDebugLog("JR NC,%+i",offset);
#endif
}

// JR C - jump if carry flag is set
// opcode: 0x38
// flags: -
void CPU::jr_c()
{
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if((regs.F & FlagBitmaskC)){
        regs.PC += offset;
    }
#ifdef DEBUG_LOG
    AddDebugLog("JR C,%+i",offset);
#endif
}


/// jr z,n
/// opcode: 0x28
/// cycles: 12/7
/// flags: -
void CPU::jr_z(){
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if((regs.F & FlagBitmaskZero)){
        regs.PC += offset;
    }

#ifdef DEBUG_LOG
    AddDebugLog("JR Z,%+i",offset);
#endif
}

/// JR NZ
/// opcode: 0x20
/// cycles: 12/7
void CPU::jr_nz(){
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    if(!(regs.F & FlagBitmaskZero)){ // If not zero
        regs.PC += offset; // start calculation from beginning of this instruction
    }

#ifdef DEBUG_LOG
    AddDebugLog("JR NZ,%+i",offset);
#endif
}


/// jr n - Jump relative
/// opcode: 0x18
/// cycles: 12
void CPU::jr_n(){
    int8_t offset =  static_cast<int8_t>(fetch8BitValue());
    regs.PC += offset;

#ifdef DEBUG_LOG
    AddDebugLog("JR %+i",offset);
#endif
}

/// JP nn - Jump absolute
/// opcode: 0xc3
/// cycles: 10
void CPU::jp_nn()
{
    regs.PC = fetch16BitValue();

#ifdef DEBUG_LOG
    AddDebugLog("JP %04x",regs.PC);
#endif
}


// JP HL
// opcode: 0xe9
// cycles: 4
// flags: -
void CPU::jp_ptr_hl(){
    regs.PC = regs.HL;
#ifdef DEBUG_LOG
    AddDebugLog("JP HL");
#endif
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
        mem[--regs.SP] = regs.PC >> 8; // (SP-1) = PC_h
        mem[--regs.SP] = static_cast<uint8_t >(regs.PC);      // (SP-2) = PC_h
        regs.PC = location;
    }
#ifdef DEBUG_LOG
    std::string conditionName = name_from_condition(conditionCode);
    AddDebugLog("CALL %s,%04x",conditionName,regs.PC);
#endif
}

// CALL
// opcode: 0xCD
// cycles: 17
void CPU::call(){
    uint16_t location = fetch16BitValue();
    mem[--regs.SP] = regs.PC >> 8; // (SP-1) = PC_h
    mem[--regs.SP] = static_cast<uint8_t>(regs.PC);      // (SP-2) = PC_h
    regs.PC = location;

#ifdef DEBUG_LOG
    AddDebugLog("CALL %04x",location);
#endif
}

// RET
// PC_l = (SP), PC_h = (SP+1)
// opcode: 0xc9
// cycles: 10
// flags: -
void CPU::RET()
{
    uint8_t lobyte = mem[regs.SP++];
    uint8_t hibyte = mem[regs.SP++];
    regs.PC = (hibyte<<8) + lobyte;
#ifdef DEBUG_LOG
    AddDebugLog("RET");
#endif
}

// RETI
// opcode: 0xed 0x4d
// cycles: 14
// flags: -
void CPU::RETI()
{
    RET();
#ifdef DEBUG_LOG
    AddDebugLog("RETI");
#endif
}


// RET cc (NZ/Z/NC/C/PO/PE/P/N)
// cycles: 11 (true) 5 (false)
// flags: -
void CPU::RET_cc(){

    uint8_t conditionCode = (current_opcode & 0b00111000) >> 3;
    bool conditionValue = false;

    switch (conditionCode)
    {
        case 0: conditionValue = !(regs.F & FlagBitmaskZero); break; // Non zero
        case 1: conditionValue = (regs.F & FlagBitmaskZero);  break; // zero
        case 2: conditionValue = !(regs.F & FlagBitmaskC);  break; // non carry
        case 3: conditionValue = (regs.F & FlagBitmaskC);  break; // carry
        default: break;
    }

    if(conditionValue){
        uint8_t lobyte = mem[regs.SP++];
        uint8_t hibyte = mem[regs.SP++];
        regs.PC = (hibyte<<8) + lobyte;
    }

#ifdef DEBUG_LOG
    std::string conditionName = name_from_condition(conditionCode);
    AddDebugLog("RET %s",conditionName);
#endif
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

    mem[--regs.SP] = regs.PC >> 8;
    mem[--regs.SP] = static_cast<uint8_t>(regs.PC);

    regs.PC = location[locationCode];

#ifdef DEBUG_LOG
    AddDebugLog("RST %04x",location[locationCode]);
#endif
}