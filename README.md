# MegaBoy

My first try at writing an emulator. 

Work in progress, and will probably rewrite large chunks of it a couple of times :)

**TODO**

 - IRQ handling: trigger IRQs from Timer, LCD, Input, etc.

**References:**

Bare metal programming for rPi
https://forums.raspberrypi.com/viewtopic.php?f=72&t=72260

Immediate mode UI - good for embedding 
https://github.com/Immediate-Mode-UI/Nuklear

Embox RTOS
https://github.com/embox/embox

Gameboy docs

https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
https://github.com/retrio/gb-test-roms/tree/master/cpu_instrs

- https://gbdev.io/pandocs/
- https://gbdev.io/pandocs/CPU_Comparison_with_Z80.html
- https://github.com/AntonioND/giibiiadvance/ (GB/GBC/GBA emulator)
- https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf (cycle accurate docs)

Other GB emulators:

- https://github.com/LIJI32/SameBoy/blob/master/Core/sm83_cpu.c

Test Roms:

- https://github.com/mattcurrie/dmg-acid2
- https://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM