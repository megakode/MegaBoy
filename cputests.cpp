#include "gtest/gtest.h"
#include "cpu.h"

TEST(CPUTests, InitialRegisterStates)
{
    CPU cpu;
    GTEST_ASSERT_EQ(cpu.regs.A, 0);
    GTEST_ASSERT_EQ(cpu.regs.B, 0);
    GTEST_ASSERT_EQ(cpu.regs.C, 0);
    GTEST_ASSERT_EQ(cpu.regs.D, 0);
    GTEST_ASSERT_EQ(cpu.regs.E, 0);
    GTEST_ASSERT_EQ(cpu.regs.H, 0);
    GTEST_ASSERT_EQ(cpu.regs.L, 0);
    GTEST_ASSERT_EQ(cpu.regs.F, 0);

}

TEST(CPUTests,LoadBCNN)
{
    CPU cpu;

    cpu.mem[0] = 0x01;
    cpu.mem[1] = 0x34;
    cpu.mem[2] = 0x12;
    cpu.step();
    GTEST_ASSERT_EQ(cpu.regs.value_in_bc(),0x1234);
}

TEST(CPUTests,LoadBCA)
{
    CPU cpu;

    cpu.mem[0] = 0x02;
    cpu.regs.B = 0x12;
    cpu.regs.C = 0x34;
    cpu.regs.A = 0x99;
    cpu.step();
    GTEST_ASSERT_EQ(cpu.mem[0x1234],0x99);
}

TEST(CPUTests,IncBC)
{
    CPU cpu;

    cpu.mem[0] = 0x03;
    cpu.regs.B = 1;
    cpu.regs.C = 0xff;
    cpu.step();
    GTEST_ASSERT_EQ(cpu.regs.value_in_bc(),0x0200);
}

TEST(CPUTests,IncB){
    CPU cpu;

    cpu.regs.F = 0;
    cpu.regs.B = 0;
    cpu.inc_b();
    GTEST_ASSERT_EQ(cpu.regs.B,1);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskSign,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskZero,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskHalfCarry,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskPV,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskN,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskC,0);
    
    // Test Sign flag
    cpu.regs.F = 0;
    cpu.regs.B = 0x80;
    cpu.inc_b();
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskSign);

    // Test Z flag
    cpu.regs.F = 0;
    cpu.regs.B = 0xff;
    cpu.inc_b();
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskZero);

    // Test HalfCarry
    cpu.regs.F = 0;
    cpu.regs.B = 0x0F;
    cpu.inc_b();
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskHalfCarry);

    // Test Parity flag
    cpu.regs.F = 0;
    cpu.regs.B = 0x7f;
    cpu.inc_b();
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskPV);

    // Test N flag
    cpu.regs.F = 0xff;
    cpu.regs.B = 0b011111111;
    cpu.inc_b();
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskN,0);
    
}

TEST(CPUTests,DecB){
    CPU cpu;

    cpu.regs.F = 0;
    cpu.regs.B = 0x10;
    cpu.dec_b();
    GTEST_ASSERT_EQ(cpu.regs.B,0x0f);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskSign,0);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskZero,0);
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskHalfCarry);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskPV,0);
    GTEST_ASSERT_TRUE(cpu.regs.F & FlagBitmaskN);
    GTEST_ASSERT_EQ(cpu.regs.F & FlagBitmaskC,0);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}