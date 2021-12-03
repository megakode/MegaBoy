#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "cpu.h"

using namespace std;

int main()
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
        std::cout << "Reading zexdoc.com";
        z80file.read ((char*)&cpu.mem[0], size );
        z80file.close();
    }



    std::cout.setf(std::ios::hex, std::ios::basefield);
#ifdef DEBUG_LOG
    std::cout << "DEBUG_LOG enabled" << std::endl;
#endif

    bool quit = false;
    cpu.reset();
    cpu.specialRegs.PC = 0;

    while(!quit){
        cpu.step();

        if(cpu.specialRegs.PC == 0){
            std::cout << "test failed";
        }

        /*
         * Den er stuck i et loop her:
         _LABEL_1B76_17:
	ld   e, (hl)
_LABEL_1B77_16:
	ld   a, e
	and  c
	jp   z, _LABEL_1B7D_15
	inc  d
_LABEL_1B7D_15:
	ld   a, c
	rlca
	ld   c, a
	cp   $01
	jp   nz, _LABEL_1B77_16
         */
    }
}