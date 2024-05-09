//
// Created by sbeam on 1/14/22.
//

#include <catch2/catch_all.hpp>
#include <fstream>

#include "../CPU/cpu.h"
#include "../Timer.h"

TEST_CASE("timer")
{
    Cartridge cart;
    HostMemory mem{cart};
    CPU cpu(mem);
    Timer timer(mem);
    bool didOverflow = false;
    constexpr uint8_t TimerEnabled = 0b100;
    constexpr uint8_t TimerDisabled = 0b000;
    constexpr uint8_t TimerCPUDiv16 = 0b001;
    constexpr uint8_t TimerCPUDiv1024 = 0b000;

    timer.didOverflow = [&didOverflow]()
    {
        didOverflow = true;
    };

    // Timer increases at CPU clock / 1024
    timer.SetTimerControl(TimerEnabled + TimerCPUDiv1024);
    timer.SetDividerRegister();
    timer.SetModulo(0x11); // expect counter to be reset to this
    timer.SetCounter(0);

    std::cout << std::format("Counter is {}.\n", timer.GetCounter());
    for (int i = 0; i < 512; i++)
    {
        // REQUIRE(timer.GetCounter() == i);
        timer.Step(1);
        std::cout << std::format("i={} Counter={}.\n", i, timer.GetCounter());
    }
    REQUIRE(didOverflow == false);
    timer.SetDividerRegister();
    REQUIRE(timer.GetCounter() == 0x11);
    REQUIRE(didOverflow);
}
