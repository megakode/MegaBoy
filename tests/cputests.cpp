#include <catch2/catch_all.hpp>
#include <fstream>

#include "../CPU/cpu.h"

TEST_CASE("fetchxxBitValue")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);

    cpu.regs.PC = 0;
    cpu.mem.Write(0x0,0xab);
    cpu.mem.Write(0x1,0x11);
    cpu.mem.Write(0x2,0x22);

    uint8_t value8 = cpu.fetch8BitValue();
    REQUIRE(value8 == 0xab);

    uint16_t value16 = cpu.fetch16BitValue();
    REQUIRE(value16 == 0x2211); // If this fails, it's probably because you are running this on a big-endian system.
}

TEST_CASE("LD r,n")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);

    cpu.regs.PC = 0;
    cpu.regs.HL = 0x100;

    cpu.mem.Write(0x0,0x06); // ld B,11
    cpu.mem.Write(0x1,0x11);
    cpu.mem.Write(0x2,0x0e); // ld C,12
    cpu.mem.Write(0x3,0x12);
    cpu.mem.Write(0x4,0x16); // ld D,11
    cpu.mem.Write(0x5,0x13);
    cpu.mem.Write(0x6,0x1e); // ld E,11
    cpu.mem.Write(0x7,0x14);
    cpu.mem.Write(0x8,0x36); // ld (hl),n
    cpu.mem.Write(0x9,0x15);

    cpu.mem.Write(0xa,0x26); // ld H,n
    cpu.mem.Write(0xb,0x16);
    cpu.mem.Write(0xc,0x2e); // ld L,n
    cpu.mem.Write(0xd,0x17);


    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();


    REQUIRE(cpu.regs.B == 0x11);
    REQUIRE(cpu.regs.C == 0x12);
    REQUIRE(cpu.regs.D == 0x13);
    REQUIRE(cpu.regs.E == 0x14);
    REQUIRE(cpu.mem.Read(0x100) == 0x15);
    REQUIRE(cpu.regs.H == 0x16);
    REQUIRE(cpu.regs.L == 0x17);

}

// TEST_CASE("SWAP r")
// {
//     Cartridge cart;
//     HostMemory mem{cart};
//     CPU cpu(mem);

//     cpu.regs.B = 0x0f;
//     cpu.do_bit_instruction(0x30,cpu.regs.B);
//     REQUIRE(cpu.regs.B == 0xf0);
// }

TEST_CASE("RRA")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu{mem};

    cpu.regs.AF = 0xcf10;
    cpu.RRA();
    REQUIRE(cpu.regs.AF == 0xe710);

}
/*
TEST_CASE("pop")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu{mem};

    cpu.regs.AF = 0x1234;
    cpu.regs.SP = 0x100;
    cpu.push_af();
    cpu.regs.AF = 0;
    cpu.pop16(cpu.regs.AF);

    REQUIRE(cpu.regs.AF == 0x1230);

    cpu.regs.BC = 0x4321;
    cpu.push_bc();
    cpu.regs.B = 0x1111;
    //cpu.pop16()
}
*/
/*
TEST_CASE("reg_from_regcode")
{
    HostMemory mem;
    CPU cpu(mem);

    cpu.regs.A = 0;
    uint8_t& reg_a = cpu.reg_from_regcode(RegisterCode::A);
    reg_a = 0xaa;
    REQUIRE(cpu.regs.A == 0xaa);

    cpu.regs.B = 0;
    uint8_t& reg_b = cpu.reg_from_regcode(RegisterCode::B);
    reg_b = 0xbb;
    REQUIRE(cpu.regs.B == 0xbb);

    cpu.regs.C = 0;
    uint8_t& reg_c = cpu.reg_from_regcode(RegisterCode::C);
    reg_c = 0xcc;
    REQUIRE(cpu.regs.C == 0xcc);

    cpu.regs.D = 0;
    uint8_t& reg_d = cpu.reg_from_regcode(RegisterCode::D);
    reg_d = 0xdd;
    REQUIRE(cpu.regs.D == 0xdd);

    cpu.regs.E = 0;
    uint8_t& reg_e = cpu.reg_from_regcode(RegisterCode::E);
    reg_e = 0xee;
    REQUIRE(cpu.regs.E == 0xee);

    cpu.regs.L = 0;
    uint8_t& reg_l = cpu.reg_from_regcode(RegisterCode::L);
    reg_l = 0x10;
    REQUIRE(cpu.regs.L == 0x10);

    cpu.regs.H = 0;
    uint8_t& reg_h = cpu.reg_from_regcode(RegisterCode::H);
    reg_h = 0x20;
    REQUIRE(cpu.regs.H == 0x20);

    cpu.regs.HL = 0x100;
    cpu.mem.Write(0x100,0x34);
    uint8_t& reg_phl = cpu.reg_from_regcode(RegisterCode::HLPtr);
    REQUIRE(reg_phl == 0x34);
}
*/
TEST_CASE("reg_name_from_regcode")
{
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::A) == "A");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::B) == "B");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::C) == "C");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::D) == "D");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::E) == "E");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::H) == "H");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::L) == "L");
    REQUIRE( CPU::reg_name_from_regcode(RegisterCode::HLPtr) == "(HL)");
}

TEST_CASE("is_condition_true")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);

    // 0 = NZ
    // 1 = Z
    // 2 = NC
    // 3 = C

    cpu.setFlag(FlagBitmaskZero,false);
    cpu.setFlag(FlagBitmaskC,true);
    REQUIRE(cpu.is_condition_true(0) == true);
    REQUIRE(cpu.is_condition_true(1) == false);
    REQUIRE(cpu.is_condition_true(2) == false);
    REQUIRE(cpu.is_condition_true(3) == true);

    cpu.setFlag(FlagBitmaskZero,true);
    cpu.setFlag(FlagBitmaskC,false);
    REQUIRE(cpu.is_condition_true(0) == false);
    REQUIRE(cpu.is_condition_true(1) == true);
    REQUIRE(cpu.is_condition_true(2) == true);
    REQUIRE(cpu.is_condition_true(3) == false);
}

TEST_CASE("Flags")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);

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

}

TEST_CASE("Parity"){

    // odd = false
    // even = true

    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);
    REQUIRE( cpu.has_parity(0b00000000) );
    REQUIRE( cpu.has_parity(0b10000010) );
    REQUIRE( cpu.has_parity(0b11110000) );
    REQUIRE( cpu.has_parity(0b10110001) );
    REQUIRE( !cpu.has_parity(0b10110000) );
    REQUIRE( !cpu.has_parity(0b10111100) );
    REQUIRE( cpu.has_parity(0b11111111) );
    
}
