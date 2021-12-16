# MegaBoy

My first try at writing an emulator. 

Work in progress, and will probably rewrite large chunks of it a couple of times :)

First step: doing a fast and accurate Z80 CPU emulator.

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

- https://gbdev.io/pandocs/
- https://gbdev.io/pandocs/CPU_Comparison_with_Z80.html
- https://github.com/AntonioND/giibiiadvance/ (GB/GBC/GBA emulator)
- https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf (cycle accurate docs)