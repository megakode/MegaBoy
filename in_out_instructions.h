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


void CPU::ind()
{
    ini(true,false);
#ifdef DEBUG_LOG
    AddDebugLog("IND");
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