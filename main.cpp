#include "6502h.h"

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
