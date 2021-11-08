#include "gtest/gtest.h"
#include "cpu.cpp"

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

TEST(CPUTests,LoadHL)
{
    CPU cpu;
    HostMemory mem;

    mem.memory[0] = 0b01111110; // LD A,(HL))
    mem.memory[0x01ff] = 0x0a; // 
    cpu.regs.H = 0x01;
    cpu.regs.L = 0xff;
    cpu.mem = &mem;
    cpu.step();
    GTEST_ASSERT_EQ(cpu.regs.A,0x0a);
}

TEST(CPUTests,LoadRR)
{
    CPU cpu;
    HostMemory mem;

    mem.memory[0] = 0b01111000; // LD A,B
    cpu.regs.A = 1;
    cpu.regs.B = 2;
    cpu.mem = &mem;
    cpu.step();
    GTEST_ASSERT_EQ(cpu.regs.A,2);
}

TEST(CPUTests,Sign)
{
    uint8_t t = -1;
    int8_t uncasted = t;
    int8_t casted = (int8_t)t;
    std::cout << "uncasted" << uncasted << std::endl;
    std::cout << "casted" << casted << std::endl;
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}