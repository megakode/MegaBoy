//
// Created by sbeam on 03/12/2021.
//

#pragma once


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


void CPU::OUT_pC_A()
{
    out(regs.C, regs.A);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),A");
#endif
}

void CPU::OUT_pC_0() {
    out(regs.C, 0);

#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),0");
#endif
}

void CPU::OUT_pC_L() {
    out(regs.C, regs.L);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),L");
#endif
}

void CPU::OUT_pC_H() {
    out(regs.C, regs.H);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),H");
#endif
}

void CPU::OUT_pC_E() {
    out(regs.C, regs.E);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),E");
#endif
}

void CPU::OUT_pC_D() {
    out(regs.C, regs.D);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),D");
#endif
}

void CPU::OUT_pC_C() {
    out(regs.C, regs.C);
#ifdef DEBUG_LOG
    AddDebugLog("OUT (C),C");
#endif
}


// OUT (N),A
// opcode: 0xD3
// cycles: 11
void CPU::out_n_a(){
    uint8_t value = fetch8BitValue();
    out(value,regs.A);
#ifdef DEBUG_LOG
    AddDebugLog(std::format("OUT ({:#04x}),A",value));
#endif
}

// IN A,(N)
// opcode: 0xDB
// flags: -
// cycles: 11
void CPU::in_a_n(){
    uint8_t port = fetch8BitValue();
    in(regs.A,port);
#ifdef DEBUG_LOG
    AddDebugLog(std::format("IN A,({:#04x})",port));
#endif
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
#ifdef DEBUG_LOG
    AddDebugLog("INIR");
#endif
}

void CPU::ind()
{
    ini(true,false);
#ifdef DEBUG_LOG
    AddDebugLog("IND");
#endif
}

void CPU::indr()
{
    ini(true,true);
#ifdef DEBUG_LOG
    AddDebugLog("INDR");
#endif
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
#ifdef DEBUG_LOG
    AddDebugLog("OTIR");
#endif
}

void CPU::outd()
{
    outi(true,false);
#ifdef DEBUG_LOG
    AddDebugLog("OUTD");
#endif
}

void CPU::otdr()
{
    outi(true,true);
#ifdef DEBUG_LOG
    AddDebugLog("OTDR");
#endif
}
