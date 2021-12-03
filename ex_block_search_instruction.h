//
// Exchange, block transfer and search instructions
// Created by sbeam on 03/12/2021.
//

#pragma once

// *****************************************************************
// Exchange
// *****************************************************************

// EX AF AF'
// opcode: 0x08
// cycles: 4
void CPU::ex_af(){
    std::swap(regs.A,auxRegs.A);
    std::swap(regs.F,auxRegs.F);
}

// EXX
// exchange BC, DE, HL registers with aux versions
// opcode: 0xd9
// cycles: 4
void CPU::exx()
{
    std::swap(regs.B,auxRegs.B);
    std::swap(regs.C,auxRegs.C);

    std::swap(regs.D,auxRegs.D);
    std::swap(regs.E,auxRegs.E);

    std::swap(regs.H,auxRegs.H);
    std::swap(regs.L,auxRegs.L);
}

// EX (SP),HL
// opcode: 0xe3
// cycles: 19
// flags: -
void CPU::EX_pSP_HL()
{
    std::swap( regs.L , mem[specialRegs.SP] );
    std::swap( regs.H , mem[specialRegs.SP+1] );
}

// EX (SP),IY
// opcode: 0xFD 0xE3
// cycles: 23
// flags: -
// Exchanges (SP) with IYL, and (SP+1) with IYH.
void CPU::EX_pSP_IY()
{
    std::swap( specialRegs.IYL , mem[specialRegs.SP] );
    std::swap( specialRegs.IYH , mem[specialRegs.SP+1] );
}

// EX (SP),IX
// opcode: 0xDD 0xE3
// cycles: 23
// flags: -
// Exchanges (SP) with IXL, and (SP+1) with IXH.
void CPU::EX_pSP_IX()
{
    std::swap( specialRegs.IXL , mem[specialRegs.SP] );
    std::swap( specialRegs.IXH , mem[specialRegs.SP+1] );
}


// EX DE,HL
// opcode: 0xEB
// cycles: 4
// flags: -
void CPU::ex_de_hl()
{
    std::swap( regs.D, regs.H);
    std::swap( regs.E, regs.L);
}

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

            setFlag(FlagBitmaskSign,result & 0x80);
            setFlag(FlagBitmaskZero, result == 0);
            setFlag(FlagBitmaskHalfCarry, (regs.A & 0xf0) < (result & 0xf0) );
        }
        while (repeat && regs.BC > 0);
    }

    setFlag(FlagBitmaskPV,regs.BC == 0);
    setFlag(FlagBitmaskN,1);
}

void CPU::cpir()
{
    cpi(false,true);
}

void CPU::cpd()
{
    cpi(true,false);
}

void CPU::cpdr()
{
    cpi(true,true);
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
    setFlag(FlagBitmaskPV, regs.BC != 0);
}

void CPU::ldir()
{
    ldi(false,true);
}

void CPU::ldd()
{
    ldi(true,false);
}

void CPU::lddr()
{
    ldi(true,true);
}
