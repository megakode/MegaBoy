#include <iostream>
#include <vector>
#include <string>
#include "cpu.cpp"

using namespace std;

int main()
{
    CPU cpu;
    HostMemory mem;

    mem.memory[0] = 0b01111110; // LD A,(HL))
    mem.memory[0x01ff] = 0x0a; // 
    cpu.regs.H = 0x01;
    cpu.regs.L = 0xff;
    cpu.mem = &mem;
    cpu.step();
    // a should be 10101010
    cout << "done" << endl;
}