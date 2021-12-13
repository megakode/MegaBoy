#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <filesystem>

#include "cpu.h"
#include "jump_tests.h"
#include "Timer.h"

TEST_CASE("Timer")
{

}

TEST_CASE("cp 0x02")
{
    CPU cpu;

    cpu.regs.F = 0xff;
    cpu.regs.A = 0x09;

    cpu.sub(0x02,false,true);

    REQUIRE(!cpu.getFlag(FlagBitmaskSign));
    REQUIRE(!cpu.getFlag(FlagBitmaskZero));
    REQUIRE(!cpu.getFlag(FlagBitmaskHalfCarry));
    REQUIRE(!cpu.getFlag(FlagBitmaskPV));
    REQUIRE(cpu.getFlag(FlagBitmaskN));
    REQUIRE(!cpu.getFlag(FlagBitmaskC));


}

TEST_CASE("z80 exerciser")
{
    CPU cpu;

    //std::filesystem::path filename = "zexdoc.com";
    std::filesystem::path filename = "zexdoc_sdsc.sms";

    auto path = std::filesystem::absolute(filename);
    auto size = std::filesystem::file_size(path);

    std::cout << "path: " << path << std::endl;
    if(exists(path)){
        std::cout << "file exists! :)";
    } else {
        std::cout << "file DOES NOT exist!";
    }
    std::ifstream z80file (path, std::ios::in | std::ios::binary);
    if (!z80file) {
        std::cout << "could not read zexdoc.com";
    } else
    {
        std::cout << "Reading " << filename;
        z80file.read ((char*)&cpu.mem[0], size );
        z80file.close();
    }

    cpu.specialRegs.PC = 0x0;

    while(1){
        cpu.step();
    }

}

TEST_CASE("RLCA")
{
    CPU cpu;

    cpu.regs.A = 0b10001000;
    cpu.rlca();
    REQUIRE(cpu.getFlag(FlagBitmaskC));
    REQUIRE(cpu.regs.A == 0b00010001);
}

TEST_CASE("reg_from_regcode")
{
    CPU cpu;

    cpu.regs.A = 0;
    uint8_t& reg_a = cpu.reg_from_regcode(RegisterCode::A);
    reg_a = 0xaa;
    REQUIRE(cpu.regs.A == 0xaa);

    cpu.regs.B = 0;
    uint8_t& reg_b = cpu.reg_from_regcode(RegisterCode::B);
    reg_b = 0xbb;
    REQUIRE(cpu.regs.B == 0xbb);

    cpu.regs.HL = 0x100;
    cpu.mem[0x100] = 0x34;
    uint8_t& reg_phl = cpu.reg_from_regcode(RegisterCode::HLPtr);
    REQUIRE(reg_phl == 0x34);
}

TEST_CASE("And"){
    CPU cpu;

    cpu.regs.A = 12;
    cpu.and_a_with_value(12);
    REQUIRE(!cpu.getFlag(FlagBitmaskZero));

    cpu.regs.A = 1;
    cpu.and_a_with_value(2);
    REQUIRE(cpu.getFlag(FlagBitmaskZero));

}

TEST_CASE("Flags")
{
    CPU cpu;

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskZero,true);
    REQUIRE(cpu.regs.F & FlagBitmaskZero);
    REQUIRE(cpu.getFlag(FlagBitmaskZero));

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskHalfCarry,true);
    REQUIRE(cpu.regs.F & FlagBitmaskHalfCarry);
    REQUIRE(cpu.getFlag(FlagBitmaskHalfCarry));

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskN,true);
    REQUIRE(cpu.regs.F & FlagBitmaskN);
    REQUIRE(cpu.getFlag(FlagBitmaskN));

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskC,true);
    REQUIRE(cpu.regs.F & FlagBitmaskC);
    REQUIRE(cpu.getFlag(FlagBitmaskC));

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskPV,true);
    REQUIRE(cpu.regs.F & FlagBitmaskPV);
    REQUIRE(cpu.getFlag(FlagBitmaskPV));

    cpu.regs.F = 0;
    cpu.setFlag(FlagBitmaskSign,true);
    REQUIRE(cpu.regs.F & FlagBitmaskSign);
    REQUIRE(cpu.getFlag(FlagBitmaskSign));
}

TEST_CASE("Compare")
{
    CPU cpu;

    // CP 1 (subtract and only set flags for comparison)
    cpu.regs.F  = 0;
    cpu.regs.A = 0;
    cpu.sub(1,false,true);
    REQUIRE( !(cpu.regs.F & FlagBitmaskZero) );

    cpu.regs.F  = 0;
    cpu.regs.A = 1;
    cpu.sub(1,false,true);
    REQUIRE( (cpu.regs.F & FlagBitmaskZero) );
}

TEST_CASE("pushpop")
{
    CPU cpu;

    cpu.regs.AF = 0x1234;
    cpu.push_af();
    cpu.regs.AF = 0;
    cpu.pop16(cpu.regs.AF);
    REQUIRE(cpu.regs.AF == 0x1234);

    cpu.regs.BC = 0x1234;
    cpu.push_bc();
    cpu.regs.BC = 0;
    cpu.pop16(cpu.regs.BC);
    REQUIRE(cpu.regs.BC == 0x1234);
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
    cpu.INC_B();
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
    cpu.INC_B();
    REQUIRE(cpu.regs.F & FlagBitmaskSign);

    // Test Z flag
    cpu.regs.F = 0;
    cpu.regs.B = 0xff;
    cpu.INC_B();
    REQUIRE(cpu.regs.F & FlagBitmaskZero);

    // Test HalfCarry
    cpu.regs.F = 0;
    cpu.regs.B = 0x0F;
    cpu.INC_B();
    REQUIRE(cpu.regs.F & FlagBitmaskHalfCarry);

    // Test Parity flag
    cpu.regs.F = 0;
    cpu.regs.B = 0x7f;
    cpu.INC_B();
    REQUIRE(cpu.regs.F & FlagBitmaskPV);

    // Test N flag
    cpu.regs.F = 0xff;
    cpu.regs.B = 0b011111111;
    cpu.INC_B();
    REQUIRE((cpu.regs.F & FlagBitmaskN) == 0);
    
}

TEST_CASE("DecB"){
    CPU cpu;

    cpu.regs.F = 0;
    cpu.regs.B = 0x10;
    cpu.DEC_B();
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