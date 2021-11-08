#include <iostream>
#include <vector>
#include <string>

class HostMemory {

    public:

        uint8_t memory[UINT16_MAX] = { 
            0b01111000 // LD A,B
         };

        uint8_t data( uint16_t addr ){
            return memory[addr];
        };

};

enum FlagBitmask : uint8_t {
    FlagBitmaskSign = 0b10000000,
    FlagBitmaskZero = 0b01000000,
    FlagBitmaskHalfCarry = 0b00010000,
    FlagBitmaskPV = 0b00000100,
    FlagBitmaskN = 0b00000010,
    FlagBitmaskC = 0b00000001
};

  /*
        Register codes used in all OP codes
        (0) 000 B
        (1) 001 C
        (2) 010 D
        (3) 011 E
        (4) 100 H
        (5) 101 L
        (6) 110 (HL)
        (7) 111 A
        */

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

struct RC {
    static const uint8_t A;
};

class CPU {

    struct Pins {
        uint16_t addr;  // Address bus
        uint8_t data;   // Data bus
    } pins;

    public:

    HostMemory *mem = nullptr;

    // Interrupt enable Flip Flop #1 and #2 (see page 323 in Zilog Z80 Technical manual)
    bool IFF = 0;
    bool IFF2 = 0;

    struct GeneralRegisters {

        // Flags:
        //
        // [S | Z | . | H | . | P/V | N | C ]
        //        
        // S - Sign flag
        // Z - Zero flag
        // . - unused
        // H - Half-carry
        // . - unused
        // P/V - Parity/overflow
        // N Add/Subtract flag
        // C - Carry 

        uint8_t B = 0,C = 0;
        uint8_t D = 0,E = 0;
        uint8_t H = 0,L = 0;
        uint8_t padding; // So register indexes in opcodes matches indexing into this struct
        uint8_t A = 0; // Accumulator
        uint8_t F = 0; 
        
        // Returns the Z80 address contained in the HL register pair
        uint16_t addrInHL() {
            return static_cast<uint16_t>((H<<8) + L);
        };

        // Returns the Z80 address contained in the DE register pair
        uint16_t addrInDE() {
            return static_cast<uint16_t>((D<<8) + E);
        };

        // Returns the Z80 address contained in the BC register pair
        uint16_t addrInBC() {
            return static_cast<uint16_t>((B<<8) + C);
        };

    };

    struct SpecialRegisters {
        uint8_t I = 0;      // Interrupt Page Address Register
        uint8_t R = 0;      // Memory Refresh Register
        uint16_t IX = 0;    // Index Register X
        uint16_t IY = 0;    // Index Register Y
        uint16_t SP = 0;    // Stack Pointer
        uint16_t PC = 0;    // Program Counter
    } specialRegs;

    GeneralRegisters regs,auxRegs;
    uint8_t regArray[9];
/*
    inline void setRegisterValue( RegisterCode dstReg, uint8_t value ){
        uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + registerCode;
        *dstRegPtr = value;
    }

    inline void setRegisterValue( RegisterCode dstReg, RegisterCode srcReg ){
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcReg;
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = *srcRegPtr;
    }
*/
    // Fetch next instruction byte from memory and increase Program Counter by +1 (PC)
    inline uint8_t fetchInstruction(){
        return mem->data(specialRegs.PC++);
    };

    // Fetch two instruction bytes from memory as a 16 bit address and increase Program Counter by +2 (PC)
    inline uint16_t fetch16BitAddress(){
        uint8_t lowbyte = fetchInstruction();
        uint16_t hibyte = fetchInstruction();
        return (hibyte<<8) + lowbyte;
    };

    void loadRR( uint8_t dstReg , uint8_t srcReg ) {
        if(srcReg == RegisterCode::HLPtr){
            uint16_t srcMemAddr = regs.addrInHL(); // Get memory address from HL register pair
            uint8_t data = mem->data(srcMemAddr); // Get data from HL location
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = data;
        } if(dstReg == RegisterCode::HLPtr){
            uint16_t dstMemAddr = regs.addrInHL(); // Get memory address from HL register pair
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcReg;
            mem->memory[dstMemAddr] = *srcRegPtr;
        } else {
            uint8_t *srcRegPtr = reinterpret_cast<uint8_t*>(&regs) + srcReg;
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = *srcRegPtr;
        }
    }


    void loadRN( uint8_t dstReg, uint8_t value){
        if(dstReg == RegisterCode::HLPtr){
            uint16_t dstMemAddr = regs.addrInHL(); // Get memory address from HL register pair
            mem->memory[dstMemAddr] = value;
        } else {
            uint8_t *dstRegPtr = reinterpret_cast<uint8_t*>(&regs) + dstReg;
            *dstRegPtr = value;
        }
    }

    void step() {

        assert(mem!=nullptr);
        
        // M1: OP Code fetch
        uint8_t op = fetchInstruction();

        // LD r,R  - (01 rrr RRR)
        if( (op & 0b11000000) == 0b01000000 ){
            uint8_t dstRegCode = (op >> 3) & 0b111;
            uint8_t srcRegCode = op & 0b111;
            loadRR(dstRegCode,srcRegCode);
        } else 

        // LD r,n - (00rrr110 nnnnnnnn)
        if( (op & 0b11000111) == 0b00000110 ) {
            uint8_t reg = (op & 0b00111000) >> 3;
            uint8_t value = fetchInstruction();
            loadRN(reg,value);
        } else 

        if( op == 0xDD ){
            uint8_t op2 = fetchInstruction();

            // Load r,(IX + d) (11 011 101,01rrr101,dddddddd)
            if( (op2 & 0b11000111) == 0b01000101){
                uint8_t reg = (op2 & 0b00111000) >> 3;
                int8_t d = fetchInstruction();
                uint16_t addr = specialRegs.IX + d;
                loadRN(reg,mem->data(addr));
            } else 

            // Load (IX+d), r (11011101,01110rrr,dddddddd )
            if( (op2 & 0b11111000) == 0b01110000 ){
                uint8_t reg = (op2 & 0b111);
                int8_t d = fetchInstruction();
                uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
                mem->memory[specialRegs.IX + d] = *regPtr;
            } else
            
            // Load (IX + d),n (DD,36,d,n)
            if(op2 == 0x36){
                int8_t d = fetchInstruction();
                uint8_t value = fetchInstruction();
                mem->memory[specialRegs.IX + d] = value;
            } else 

            // Load IX,nn - M:4 T:14
            if(op2 == 0x21){
                specialRegs.IX = fetch16BitAddress();
            }

            // Load IX,(nn) - M:6 T:20
            if(op2 == 0x2a){
                uint16_t addr = fetch16BitAddress();
                uint8_t lobyte = mem->data(addr);
                uint8_t hibyte = mem->data(addr+1);
                specialRegs.IX = (hibyte<<8) + lobyte;
            }

            // Load (nn),IX - (0xdd,0x22,n,n)- M:6 T:20
            if(op2 == 0x22){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = (uint8_t)specialRegs.IX;
                mem->memory[(uint16_t)(addr+1)] = specialRegs.IX>>8;
            }

            // Load SP,IX - M:2 T:10
            if(op2 == 0xF9){
                specialRegs.SP = specialRegs.IX;
            }


            // Push IX - M:€ T:15
            if( op2 == 0xE5){
                mem->memory[specialRegs.SP-2] = (uint8_t)specialRegs.IX;
                mem->memory[specialRegs.SP-1] = specialRegs.IX >> 8;
                specialRegs.SP -= 2;
            }

            // POP IX - M:4 T:14
            if( op2 == 0xE1 ){
                specialRegs.IX = (mem->data(specialRegs.SP+1)<<8) + mem->data(specialRegs.SP);
                specialRegs.SP +=2;
            }

        
        } else 
        
        if ( op == 0xFD ){
            uint8_t op2 = fetchInstruction();

            // Load r,(IY + d) (11 111 101,01rrr110,dddddddd)
            if( (op2 & 11000111) == 01000110 ){
                uint8_t reg = (op2 & 0b0011100) >> 3;
                uint8_t value = fetchInstruction();
                uint16_t addr = specialRegs.IY + (int8_t)regs.D;
                loadRN(reg,mem->data(addr));
            } else

            // Load (IY + d),r (11111101, 01110rrr, dddddddd)
            if( (op2 & 0b11111000) == 0b01110000){
                uint8_t reg = (op2 & 0b111);
                int8_t d = fetchInstruction();
                uint8_t *regPtr = reinterpret_cast<uint8_t*>(&regs) + reg;
                mem->memory[specialRegs.IY + d] = *regPtr;
            }else
            // Load (IY + d),n (FD,36,d,n)
            if(op2 == 0x36){
                int8_t d = fetchInstruction();
                uint8_t value = fetchInstruction();
                mem->memory[specialRegs.IY + d] = value;
            }else 
            
            // Load IX,nn - M:4 T:14
            if(op2 == 0x21){
                specialRegs.IY = fetch16BitAddress();
            }

            // Load IY,(nn) - M:6 T:20
            if(op2 == 0x2a){
                uint16_t addr = fetch16BitAddress();
                uint8_t lobyte = mem->data(addr);
                uint8_t hibyte = mem->data(addr+1);
                specialRegs.IY = (hibyte<<8) + lobyte;
            } else

            // Load (nn),IY - (0xfd,0x22,n,n)- M:6 T:20
            if(op2 == 0x22){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = (uint8_t)specialRegs.IY;
                mem->memory[(uint16_t)(addr+1)] = specialRegs.IY>>8;
            }else 

            // Load SP,IY - M:2 T:10
            if(op2 == 0xF9){
                specialRegs.SP = specialRegs.IY;
            }

            // Push IY - M:4 T:15
            if( op2 == 0xE5){
                mem->memory[specialRegs.SP-2] = (uint8_t)specialRegs.IY;
                mem->memory[specialRegs.SP-1] = specialRegs.IY >> 8;
                specialRegs.SP -= 2;
            } else 

            // POP IY - M:4 T:14
            if( op2 == 0xE1 ){
                specialRegs.IY = (mem->data(specialRegs.SP+1)<<8) + mem->data(specialRegs.SP);
                specialRegs.SP +=2;
            }


        }else 

        // load (HL),n (0x36,n)
        if( op == 0x36 ){
            uint8_t value = fetchInstruction();
            mem->memory[regs.addrInHL()] = value;
        } else

        // Load A,(BC)
        if( op == 0x0A){
            regs.A = mem->memory[regs.addrInBC()];
        } else

        // Load A,(DE)
        if( op == 0x1A){
            regs.A = mem->memory[regs.addrInDE()];
        } else

        // Load A,(nn) (0x3a,n,n)
        if( op == 0x3A){
            uint16_t addr = fetch16BitAddress();
            regs.A = mem->memory[addr];
        } else

        // Load (BC),A
        if( op == 0x02){
            mem->memory[regs.addrInBC()] = regs.A;
        } else 

        // Load (DE),A
        if( op == 0x12){
            mem->memory[regs.addrInDE()] = regs.A;
        } else 

        // Load (nn),A
        if( op == 0x32){
            uint16_t addr = fetch16BitAddress();
            mem->memory[addr] = regs.A;
        } else 

        if( op == 0xED){
            uint8_t op2 = fetchInstruction();
            // Load A,I
            if(op2 == 0x57){
                regs.A = specialRegs.I;
                regs.F &= 0b00000001;
                // Bits affected:
                // S: 1 if I-reg is negative. 0 otherwise.
                // TODO: how is I negative? isn't it unsigned?
                // Z: 1 if I is zero. 0 otherwise
                if(specialRegs.I == 0){ regs.F |= FlagBitmaskZero; };
                // H : 0
                // P/V: contents of IFF2
                if(IFF2){ regs.F |= FlagBitmaskPV; };
                // N: 0
                // C: not affected
            } else 

            // Load A,R
            if(op2 == 0x5f){
                regs.A = specialRegs.R;
                regs.F &= 0b00000001;
                // Bits affected:
                // S: 1 if I-reg is negative. 0 otherwise.
                // TODO: how is I negative? isn't it unsigned?
                // Z: 1 if I is zero. 0 otherwise
                if(specialRegs.I == 0){ regs.F |= FlagBitmaskZero; };
                // H : 0
                // P/V: contents of IFF2
                if(IFF2){ regs.F |= FlagBitmaskPV; };
                // N: 0
                // C: not affected
            } else 

            // Load I,A
            if(op2==0x47){
                specialRegs.I = regs.A;
            } else 

            // Load R,A
            if(op2==0x4F){
                specialRegs.R = regs.A;
            } else 

            // Load BC,(nn) - M:6 T:20
            if(op2==0b01001011){
                uint16_t addr = fetch16BitAddress();
                regs.C = mem->memory[addr];
                regs.B = mem->memory[addr+1];
            } else 
            
            // Load DE,(nn) - M:6 T:20
            if(op2==0b01011011){
                uint16_t addr = fetch16BitAddress();
                regs.E = mem->memory[addr];
                regs.D = mem->memory[addr+1];
            } else

            // Load HL,(nn) - M:6 T:20
            if(op2==0b01101011){
                uint16_t addr = fetch16BitAddress();
                regs.L = mem->memory[addr];
                regs.H = mem->memory[addr+1];
            } else 
            
            // Load SP,(nn) - M:6 T:20
            if(op2==0b01111011){
                uint16_t addr = fetch16BitAddress();
                uint8_t lowbyte = mem->memory[addr];
                uint16_t hibyte = mem->memory[addr+1];
                specialRegs.SP = (hibyte<<8) + lowbyte;
            } else 

            // Load (nn),BC - M:6 T:20
            if(op2==0b01000011){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = regs.C;
                mem->memory[(uint16_t)(addr+1)] = regs.B;
            }else 

            // Load (nn),DE - M:6 T:20
            if(op2==0b01010011){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = regs.E;
                mem->memory[(uint16_t)(addr+1)] = regs.D;
            }else 

            // Load (nn),HL - M:6 T:20
            if(op2==0b01100011){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = regs.L;
                mem->memory[(uint16_t)(addr+1)] = regs.H;
            }

            // Load (nn),SP - M:6 T:20
            if(op2==0b01110011){
                uint16_t addr = fetch16BitAddress();
                mem->memory[addr] = (uint8_t)specialRegs.SP;
                mem->memory[(uint16_t)(addr+1)] = (specialRegs.SP>>8);
            }


        } else 

            // 0x11,0x21,0x31
            // Load BC,nn (00000001) - M:3 T:10
        if( op == 0x01 ){
            regs.C = fetchInstruction();
            regs.B = fetchInstruction(); 
        } else 
        
        // Load DE,nn (0b00010001) - M:3 T:10
        if( op == 0x11 ){
            regs.E = fetchInstruction();
            regs.D = fetchInstruction();
        } else 

        // Load HL,nn (0b00100001) - M:3 T:10
        if( op == 0x21 ){
            regs.L = fetchInstruction();
            regs.H = fetchInstruction();
        } else 

        if( op == 0x22 ){
            uint16_t addr = fetch16BitAddress();
            mem->memory[addr] = regs.L;
            mem->memory[addr+1] = regs.H;
        } else

        // Load SP,nn (0b00110001) - M:3 T:10
        if( op == 0x31 ){
            specialRegs.SP = fetch16BitAddress();
        } else 

        // Load HL,(nn) - M:5 T:16
        if( op == 0x2A ){
            uint16_t addr = fetch16BitAddress();
            regs.L = mem->memory[addr];
            regs.H = mem->memory[(uint16_t)(addr+1)];
        } else

        // Load SP,HL - M:1 T:6
        if( op == 0xF9){
            specialRegs.SP = regs.addrInHL();
        } else 

        // Push BC - M:3 T:11
        if( op == 0b11000101){
            mem->memory[specialRegs.SP-2] = regs.C;
            mem->memory[specialRegs.SP-1] = regs.B;
            specialRegs.SP -= 2;
        } else

        // Push DE - M:3 T:11
        if( op == 0b11010101){
            mem->memory[specialRegs.SP-2] = regs.E;
            mem->memory[specialRegs.SP-1] = regs.D;
            specialRegs.SP -= 2;
        } else 

        // Push HL - M:3 T:11
        if( op == 0b11100101){
            mem->memory[specialRegs.SP-2] = regs.L;
            mem->memory[specialRegs.SP-1] = regs.H;
            specialRegs.SP -= 2;
        }

        // Push AF - M:3 T:11
        if( op == 0b11110101){
            mem->memory[specialRegs.SP-2] = regs.F;
            mem->memory[specialRegs.SP-1] = regs.A;
            specialRegs.SP -= 2;
        }

        // Pop BC - M:3 T:10
        if( op == 0b11000001 ){
            regs.C = mem->data(specialRegs.SP);
            regs.B = mem->data(specialRegs.SP+1);
            specialRegs.SP +=2;
        }

        // Pop DE - M:3 T:10
        if( op == 0b11010001 ){
            regs.E = mem->data(specialRegs.SP);
            regs.D = mem->data(specialRegs.SP+1);
            specialRegs.SP +=2;
        }

        // Pop HL - M:3 T:10
        if( op == 0b11100001 ){
            regs.L = mem->data(specialRegs.SP);
            regs.H = mem->data(specialRegs.SP+1);
            specialRegs.SP +=2;
        }

        // Pop AF - M:3 T:10
        if( op == 0b11110001 ){
            regs.F = mem->data(specialRegs.SP);
            regs.A = mem->data(specialRegs.SP+1);
            specialRegs.SP +=2;
        }

        // M2: Memory read
        // M3: Memory write

    }

};