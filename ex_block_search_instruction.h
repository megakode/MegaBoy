//
// Exchange, block transfer and search instructions
// Created by sbeam on 03/12/2021.
//

#pragma once

// *****************************************************************
// Block
// *****************************************************************

// CPI
// opcode: 0xed 0xa1
// cycles: 16
void CPU::cpi( bool decrease, bool repeat )
{
    if( regs.BC > 0)
    {
        do
        {
            uint8_t result = regs.A - mem[regs.HL];

            if(decrease){
                regs.HL--;
            } else {
                regs.HL++;
            }

            regs.BC--;


            setFlag(FlagBitmaskZero, result == 0);
            setFlag(FlagBitmaskHalfCarry, (regs.A & 0xf0) < (result & 0xf0) );
        }
        while (repeat && regs.BC > 0);
    }

    setFlag(FlagBitmaskN,1);
}

// LDI
// opcode: 0xed 0xa0
// cycles: 16
// flags: H PV N
void CPU::ldi( bool decrease, bool repeat )
{
    if(regs.BC > 0)
    {
        do
        {
            mem[regs.DE] = mem[regs.HL]; // (de) = (hl)

            if (decrease)
            {
                regs.DE--;
                regs.HL--;
            } else
            {
                regs.DE++;
                regs.HL++;
            }

            regs.BC--;

        } while (repeat && regs.BC > 0);
    }

    setFlag(FlagBitmaskHalfCarry, 0);
    setFlag(FlagBitmaskN, 0);
}

void CPU::ldir()
{
    ldi(false,true);
}

