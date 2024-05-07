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

    timer.didOverflow = [&didOverflow]()
    {
        didOverflow = true;
    };

    // Timer increases at CPU clock / 16
    timer.SetTimerControl(TimerEnabled + TimerCPUDiv16);
    timer.SetDividerRegister();
    timer.SetModulo(0x11); // expect counter to be reset to this
    timer.SetCounter(0);

    for (int i = 0; i < 254; i++)
    {
        REQUIRE(timer.GetCounter() == i);
        timer.Step(4);
        std::cout << "i=" << i << "counter=" << timer.GetCounter() << std::endl;
    }
    REQUIRE(didOverflow == false);
    timer.Step(16);
    REQUIRE(timer.GetCounter() == 0x11);
    REQUIRE(didOverflow);
}
