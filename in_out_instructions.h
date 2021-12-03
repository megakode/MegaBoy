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



// OUT (N),A
// opcode: 0xD3
// cycles: 11
void CPU::out_n_a(){
    uint8_t value = fetch8BitValue();
    out(value,regs.A);
#ifdef DEBUG_LOG
    std::cout << "OUT (" << (int)value << "),A [A=" << (int)regs.A << "]" << std::endl;
#endif
}

// IN A,(N)
// opcode: 0xDB
// flags: -
// cycles: 11
void CPU::in_a_n(){
    in(regs.A, fetch8BitValue());
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
}

void CPU::outd()
{
    outi(true,false);
}

void CPU::otdr()
{
    outi(true,true);
}
