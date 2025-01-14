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
    
    Byte C : 1; // Carry flag
    Byte Z : 1; // Zero flag
    Byte I : 1; // Intrupt disable flag
    Byte D : 1; // Decimal mode flag
    Byte B : 1; // Break flag
    Byte V : 1; // oVerflow flag
    Byte N : 1; // Negative flag

    void reset(Mem& memory) {
        PC = 0xFFFC;
        SP = 0x0100;

        A = X = Y = 0;
        C = Z = I = D = B = V = N = 0;

        memory.init();
    }

    Word fetchByte(u32& cycles, Mem& memory) {
        
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
    

    Word readWord(u32& cycles, Word address, Mem& memory) {
        Byte lowBytes = readByte(cycles, address, memory);
        Byte highBytes = readByte(cycles, address + 1, memory);

        return lowBytes | (highBytes << 8);
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
        INS_JSR = 0x20,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xA2;


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
                case INS_LDA_ABS: {
                    Word absoluteAddress = fetchWord(cycles, memory);
                    A = readByte(cycles, absoluteAddress, memory);
                } break;
                case INS_LDA_ABSX: {
                    Word absoluteAddress = fetchWord(cycles, memory);
                    Word absAddressX = absoluteAddress + X;
                    A = readByte(cycles, absAddressX, memory);
                    // check if the pages cross
                    if (absAddressX - absoluteAddress >= 0xFF) 
                        --cycles;
                } break;
                case INS_LDA_ABSY: {
                    Word absoluteAddress = fetchWord(cycles, memory);
                    Word absAddressY = absoluteAddress + Y;
                    A = readByte(cycles, absAddressY, memory);
                    // check if the pages cross
                    if (absAddressY - absoluteAddress >= 0xFF)
                        --cycles;
                } break;
                case INS_LDA_INDX: {
                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    zeroPageAddress += X;
                    --cycles;
                    Word effectiveAddress = readWord(cycles, zeroPageAddress, memory);
                    A = readByte(cycles, effectiveAddress, memory);
                } break;
                case INS_LDA_INDY: {
                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    zeroPageAddress += Y;
                    Word effectiveAddress = readWord(cycles, zeroPageAddress, memory);
                    A = readByte(cycles, effectiveAddress, memory); 
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
