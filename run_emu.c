#include <stdio.h>
#include <stdbool.h>
// need to create register variables, global if possible, maybe a struct?
int16_t V0 = 0;
int16_t V1 = 0;
int16_t V2 = 0;
int16_t V3 = 0;
int16_t V4 = 0;
int16_t V5 = 0;
int16_t V6 = 0;
int16_t V7 = 0;
int16_t V8 = 0;
int16_t V9 = 0;
int16_t VA = 0;
int16_t VB = 0;
int16_t VC = 0;
int16_t VD = 0;
int16_t VE = 0;
int16_t VF = 0;
int16_t I = 0;
int16_t PC = 0;
// Each register (16 total) will start with V and have 0-F second digit as a name.
    // VF should not be touched. Also need an 16 bit register I.
// 2 8-bit registers for sound and delay.
// 16 layer stack of 16-bit values
// 16-bit PC
// Need user input, 16 key hex keyboard (1234qwerasdfzxcv is common)
int16_t instruction(int16_t instr){
    // take an instruction?
}

void update_display(int16_t display){
    // update display?
}

int main(int args, char ** argc){
    bool Flag = true;
    int16_t instr = PC; // get first instruction
    int16_t front_mask = 0xF000;
    int16_t back_mask = 0xF;
    while(Flag){
        // Fetch instruction in PC
        // increment PC
        // Decode, probably with big switch based on first hex?
        int16_t nibble1 = front_mask & instr >> 12;
        switch(nibble1){
            case 0x0:
                int16_t masked_instr = instr & back_mask;
            case 0x1:
                int16_t masked_instr = instr & back_mask;
            case 0x2:
                int16_t masked_instr = instr & back_mask;
            case 0x3:
                int16_t masked_instr = instr & back_mask;
            case 0x4:
                int16_t masked_instr = instr & back_mask;
            case 0x5:
                int16_t masked_instr = instr & back_mask;
            case 0x6:
                int16_t masked_instr = instr & back_mask;
            case 0x7:
                int16_t masked_instr = instr & back_mask;
            case 0x8:
                int16_t masked_instr = instr & back_mask;
            case 0x9:
                int16_t masked_instr = instr & back_mask;
            case 0xA:
                int16_t masked_instr = instr & back_mask;
            case 0xB:
                int16_t masked_instr = instr & back_mask;
            case 0xC:
                int16_t masked_instr = instr & back_mask;
            case 0xD:
                int16_t masked_instr = instr & back_mask;
            case 0xE:
                int16_t masked_instr = instr & back_mask;
            case 0xF:
                int16_t masked_instr = instr & back_mask;
        }
        // execute emulator
    }
}