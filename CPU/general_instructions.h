//
// Created by sbeam on 03/12/2021.
//

#pragma once

// nop
// opcode: 00
// Cycles: 4
void CPU::NOP(){
#ifdef DEBUG_LOG
    AddDebugLog("NOP");
#endif
};

/// Strange 1 or 2 byte instruction, that is not used in any commercial game, so dont bother implementing it correctly.
/// It is described with a flowchart here:
/// https://gbdev.io/pandocs/Reducing_Power_Consumption.html
void CPU::STOP()
{
    // Do nothing
}

void CPU::disable_interrupts()
{
    interrupt_master_enabled = false;
#ifdef DEBUG_LOG
    AddDebugLog("DI");
#endif
}

void CPU::enable_interrupts()
{
    interrupt_master_enabled = true;
#ifdef DEBUG_LOG
    AddDebugLog("EI");
#endif
}

void CPU::halt(){
    // TODO: implement this
#ifdef DEBUG_LOG
    AddDebugLog("HALT");
#endif
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

    setFlag( FlagBitmaskZero, regs.A == 0);

#ifdef DEBUG_LOG
    AddDebugLog("DAA");
#endif
}

// CPL
// Complement A (bitwise invert)
// opcode: 0x2f
// cycles: 4
// flags: N H
void CPU::cpl(){
    regs.A = ~regs.A;
    setFlag(FlagBitmaskHalfCarry,true);
    setFlag(FlagBitmaskN,true);
#ifdef DEBUG_LOG
    AddDebugLog("CPL");
#endif
}

// NEG : A = 0-A
// opcode: 0xed 0x44
// flags S Z H PV N C
// cycles: 8
void CPU::NEG()
{
    uint8_t result = 0-regs.A;
    setFlag(FlagBitmaskC,regs.A != 0);
    setFlag(FlagBitmaskHalfCarry, (0xf0 & regs.A) < (result & 0xf0) );
    setFlag(FlagBitmaskZero, result == 0);
    regs.A = result;
#ifdef DEBUG_LOG
    AddDebugLog("NEG");
#endif
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
#ifdef DEBUG_LOG
    AddDebugLog("SCF");
#endif
}

// CCF - invert carry flag
// opcode: 0x3f
// cycles: 4
// flags: C N H
void CPU::ccf()
{
    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN,0);
    setFlag(FlagBitmaskC,!(regs.F & FlagBitmaskC));
#ifdef DEBUG_LOG
    AddDebugLog("CCF");
#endif
}
