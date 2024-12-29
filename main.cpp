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

    Byte operator[](u32 address) const {
        
        // assert here the address is < MAX_MEM
        return data[address];
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

    Byte fetchByte(u32 cycles, Mem& memory) {
        
        Byte data = memory[PC]; // Byte data = memory[PC--];
        ++PC;                   // uncomment the upper line and delete this line
        --cycles;
        return data;
    }

    void execute(u32 cycles, Mem& memory) {

        while (cycles > 0) {
            Byte instruction = fetchByte(cycles, memory);
        }
    }

};

int main() {
    
    Mem mem;
    CPU cpu;
    cpu.reset(mem);
    cpu.execute(2, mem);

    return 0;
}
