#include <cstdint>
#include <string>
#include "../HostMemory.h"
#include "arithmetic_instructions_disassembler.h"

struct InstructionBytes {    
    uint8_t data[3];
};

struct DisassemblyLine {
    uint16_t PC = 0;
    uint8_t cycles;
    InstructionBytes instructionBytes;
    std::string text;
};


class Disassembler
{

    public:

        Disassembler() = default;

        /// @brief Disassemble memory at location and add disassembled line to internal list
        /// @param address Location in host memory to disassemble. Depending on the instruction at the given location 1-4 bytes will be disassembled.
        /// @param memref Reference to the host GB memory.
        /// @return Number of bytes the instruction took up.
        DisassemblyLine DisassembleAddress( uint16_t address , const HostMemory& memref );

        struct Instruction {
        /// Number of CPU cycles the instruction takes to execute
        //  uint8_t cycles = 0;
        /// pointer to the function executing the instruction
            DisassemblyLine (Disassembler::*code)(InstructionBytes) = nullptr;
        };

        // Arithmetic instructions
        DisassemblyLine CP_r(const InstructionBytes &bytes);
        DisassemblyLine ADD_A_r(const InstructionBytes &bytes);

    private:

    enum RegisterCode : uint8_t {
        B = 0,
        C,
        D,
        E,
        H,
        L,
        HLPtr,
        A
    };

    static inline std::string reg_name_from_regcode( uint8_t regcode )
    {
        switch (regcode) {
            case RegisterCode::A: return "A"; break;
            case RegisterCode::B: return "B"; break;
            case RegisterCode::C: return "C"; break;
            case RegisterCode::D: return "D"; break;
            case RegisterCode::E: return "E"; break;
            case RegisterCode::H: return "H"; break;
            case RegisterCode::L: return "L"; break;
            case RegisterCode::HLPtr: return "(HL)"; break;
            default: return "Regcode not found!"; break;
        }
    }


        std::vector<Instruction> instructions;

};