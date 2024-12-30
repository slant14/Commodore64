#include <stdio.h>
#include <stdlib.h>


using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte data[MAX_MEM];

    void init() {
        for (u32 i = 0; i < MAX_MEM; i++) {
            data[i] = 0;
        }
    }

    // Read one byte
    Byte operator[](u32 address) const {
        
        // assert here the address is < MAX_MEM
        return data[address];
    }

    // Write one byte
    Byte& operator[](u32 address) {
        return data[address];
    }

    void writeWord(u32 cycles, Word value, u32 address) {
        data[address]       = (value & 0xFF);
        data[address + 1]   = (value >> 8);
        cycles -= 2;
    }

};

struct CPU {
    Word PC; // program counter
    Word SP; // stack pointer
             
    Byte A, X, Y; // Register
    
    Byte C : 1; // Status flag
    Byte Z : 1; // Status flag
    Byte I : 1; // Status flag
    Byte D : 1; // Status flag
    Byte B : 1; // Status flag
    Byte V : 1; // Status flag
    Byte N : 1; // Status flag

    void reset(Mem& memory) {
        PC = 0xFFFC;
        SP = 0x0100;

        A = X = Y = 0;
        C = Z = I = D = B = V = N = 0;

        memory.init();
    }

    Byte fetchByte(u32& cycles, Mem& memory) {
        
        Byte data = memory[PC]; // Byte data = memory[PC--];
        ++PC;                   // uncomment the upper line and delete this line
        --cycles;
        return data;
    }
    
    Word fetchWord(u32& cycles, Mem& memory) {
        Word data = memory[PC++];
        data |= (memory[PC++] << 8);
        cycles -= 2;
        return data;
    }

    Byte readByte(u32& cycles, Byte address, Mem& memory) {
        Byte data = memory[address];
        --cycles;
        return data;
    }

    // Instruction opcodes
    static constexpr Byte 
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0x20;

    void LDASetStatusFlags() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }


    void execute(u32 cycles, Mem& memory) {

        while (cycles > 0) {
            Byte instruction = fetchByte(cycles, memory);
            
            switch (instruction) {
                case INS_LDA_IM: {
                    Byte value = fetchByte(cycles, memory);
                    A = value;
                    LDASetStatusFlags();
                } break;
                case INS_LDA_ZP: {
                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    A = readByte(cycles, zeroPageAddress, memory);
                    LDASetStatusFlags();
                } break;
                case INS_LDA_ZPX: {
                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    zeroPageAddress += X;
                    --cycles;
                    A = readByte(cycles, zeroPageAddress, memory);
                    LDASetStatusFlags();
                } break;
                case INS_JSR: {
                    Word subRutineAddress = fetchWord(cycles, memory);
                    memory.writeWord(cycles, PC - 1, SP);
                    PC = subRutineAddress;
                    --cycles;
                } break;
                default: {
                    printf("Unknown instruction : %d\n", instruction);
                } break;
            }
        }
    }

};

int main() {
    
    Mem mem;
    CPU cpu;
    
    cpu.reset(mem);
    
    // Test program
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x69;

    cpu.execute(9, mem);

    printf ("%x\n", mem[0xFFFC]);
    printf ("%x\n", mem[0xFFFD]);
    return 0;
}
