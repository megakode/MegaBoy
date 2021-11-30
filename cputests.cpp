#include <catch2/catch_test_macros.hpp>
#include "cpu.h"


// TODO: check ZEXDOC - Z80 instruction set exerciser
// https://github.com/anotherlin/z80emu/blob/master/testfiles/zexdoc.z80

TEST_CASE("InitialRegisterStates")
{
    CPU cpu;
    REQUIRE(cpu.regs.A == 0);
    REQUIRE(cpu.regs.B == 0);
    REQUIRE(cpu.regs.C == 0);
    REQUIRE(cpu.regs.D == 0);
    REQUIRE(cpu.regs.E == 0);
    REQUIRE(cpu.regs.H == 0);
    REQUIRE(cpu.regs.L == 0);
    REQUIRE(cpu.regs.F == 0);

}

TEST_CASE("ldi")
{
    CPU cpu;

    cpu.regs.C = 3;
    cpu.ldi(false,false);
    REQUIRE(cpu.regs.C == 2);

    cpu.regs.C = 3;
    cpu.ldi(false,true);
    REQUIRE(cpu.regs.C == 0);

}

TEST_CASE("add16")
{
    CPU cpu;

    cpu.regs.BC = 30000;
    cpu.regs.HL = 30000;
    cpu.add16(cpu.regs.BC,cpu.regs.HL);
    REQUIRE(cpu.regs.BC == 60000);
}

TEST_CASE("RegCodes")
{
    CPU cpu;

    cpu.regs.A = 1;
    cpu.regs.B = 2;

    auto reg_a = cpu.reg_from_regcode(RegisterCode::A);
    REQUIRE(cpu.regs.A == 1);

    auto reg_b = cpu.reg_from_regcode(RegisterCode::B);
    REQUIRE(cpu.regs.B == 2);

    reg_b = 3;

    REQUIRE(reg_b == 3);

    cpu.regs.H = 0;
    cpu.regs.L = 10;
    cpu.mem[10] = 0xaa;
    REQUIRE(cpu.reg_from_regcode(RegisterCode::HLPtr) == 0xaa);
}

TEST_CASE("BitInstructionsIX"){

    CPU cpu;
    cpu.regs.B = 0b00001111;
    cpu.specialRegs.IX = 10;
    cpu.mem[0] = 1;
    cpu.do_bit_instruction( 0 , cpu.mem[0] , cpu.mem[0] );

    REQUIRE(cpu.mem[0] == cpu.regs.A);
}

TEST_CASE("LoadBCNN")
{
    CPU cpu;

    cpu.mem[0] = 0x01;
    cpu.mem[1] = 0x34;
    cpu.mem[2] = 0x12;
    cpu.step();
    REQUIRE(cpu.regs.BC == 0x1234);
}

TEST_CASE("LoadBCA")
{
    CPU cpu;

    cpu.mem[0] = 0x02;
    cpu.regs.B = 0x12;
    cpu.regs.C = 0x34;
    cpu.regs.A = 0x99;
    cpu.step();
    REQUIRE(cpu.mem[0x1234] == 0x99);
}

TEST_CASE("IncBC")
{
    CPU cpu;

    cpu.mem[0] = 0x03;
    cpu.regs.B = 1;
    cpu.regs.C = 0xff;
    cpu.step();
    REQUIRE(cpu.regs.BC == 0x0200);
}

TEST_CASE("IncB"){
    CPU cpu;

    cpu.regs.F = 0;
    cpu.regs.B = 0;
    cpu.inc_b();
    REQUIRE(cpu.regs.B == 1);
    REQUIRE((cpu.regs.F & FlagBitmaskSign) == 0);
    REQUIRE((cpu.regs.F & FlagBitmaskZero) == 0);
    REQUIRE((cpu.regs.F & FlagBitmaskHalfCarry) ==0);
    REQUIRE((cpu.regs.F & FlagBitmaskPV) == 0);
    REQUIRE((cpu.regs.F & FlagBitmaskN) == 0);
    REQUIRE((cpu.regs.F & FlagBitmaskC) == 0);
    
    // Test Sign flag
    cpu.regs.F = 0;
    cpu.regs.B = 0x80;
    cpu.inc_b();
    REQUIRE(cpu.regs.F & FlagBitmaskSign);

    // Test Z flag
    cpu.regs.F = 0;
    cpu.regs.B = 0xff;
    cpu.inc_b();
    REQUIRE(cpu.regs.F & FlagBitmaskZero);

    // Test HalfCarry
    cpu.regs.F = 0;
    cpu.regs.B = 0x0F;
    cpu.inc_b();
    REQUIRE(cpu.regs.F & FlagBitmaskHalfCarry);

    // Test Parity flag
    cpu.regs.F = 0;
    cpu.regs.B = 0x7f;
    cpu.inc_b();
    REQUIRE(cpu.regs.F & FlagBitmaskPV);

    // Test N flag
    cpu.regs.F = 0xff;
    cpu.regs.B = 0b011111111;
    cpu.inc_b();
    REQUIRE((cpu.regs.F & FlagBitmaskN) == 0);
    
}

TEST_CASE("DecB"){
    CPU cpu;

    cpu.regs.F = 0;
    cpu.regs.B = 0x10;
    cpu.dec_b();
    REQUIRE(cpu.regs.B == 0x0f);
    REQUIRE((cpu.regs.F & FlagBitmaskSign) == 0);
    REQUIRE((cpu.regs.F & FlagBitmaskZero) == 0);
    REQUIRE(cpu.regs.F & FlagBitmaskHalfCarry);
    REQUIRE((cpu.regs.F & FlagBitmaskPV) == 0);
    REQUIRE(cpu.regs.F & FlagBitmaskN);
    REQUIRE((cpu.regs.F & FlagBitmaskC) == 0);
}

TEST_CASE("Parity"){

    // odd = false
    // even = true

    CPU cpu;
    REQUIRE( cpu.has_parity(0b00000000) );
    REQUIRE( cpu.has_parity(0b10000010) );
    REQUIRE( cpu.has_parity(0b11110000) );
    REQUIRE( cpu.has_parity(0b10110001) );
    REQUIRE( cpu.has_parity(0b10110000) );
    REQUIRE( cpu.has_parity(0b10111100) );
    REQUIRE( cpu.has_parity(0b11111111) );
    
}
/*
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}*/