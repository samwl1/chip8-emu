#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// need to create register variables, global if possible, maybe a struct?
int16_t registers[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t I = 0;
int16_t PC = 0;

// Each register (16 total) will start with V and have 0-F second digit as a name.
    // VF should not be touched. Also need an 16 bit register I.
// 2 8-bit registers for sound and delay.
// 16 layer stack of 16-bit values
// 16-bit PC
// Need user input, 16 key hex keyboard (1234qwerasdfzxcv is common)

void update_display(int16_t display){
    // update display?
}

int main(int args, char ** argc){
    bool Flag = true;
    int16_t* mem_ptr = calloc(2048, sizeof(int16_t));
    int16_t* stack_ptr = malloc(sizeof(int16_t)*16);
    int16_t instr = PC; // get first instruction
    int16_t op_code_mask = 0xF000;
    int16_t nibble_mask_2 = 0x0F00;
    int16_t nibble_mask_3 = 0x00F0;
    int16_t nibble_mask_4 = 0x000F;
    
    while(Flag){
        // Fetch instruction in PC
        // increment PC
        // Decode, probably with big switch based on first hex?
        int16_t nibble1 = op_code_mask & instr >> 12;
        
        int16_t nibble2 = nibble_mask_2 & instr >> 8;
        int16_t nibble3 = nibble_mask_3 & instr >> 4;
        int16_t nibble4 = nibble_mask_4 & instr;
        int16_t instr_data = 0x0FFF & instr;
        switch(nibble1){
            case 0x0:
                if(instr_data == 0x0E0){
                    update_display(0);
                }
                else if (instr_data == 0x0EE)
                {
                    PC = *stack_ptr;
                    stack_ptr = stack_ptr - 1;
                }
                else
                {
                    // Jump to machine code routine @ instr_data;
                }
                
                
            case 0x1:
                PC = instr_data;
            case 0x2:
                stack_ptr = stack_ptr + 1;
                *stack_ptr = PC;
                PC = instr_data;
            case 0x3:
                if(registers[nibble2] == ((nibble3 << 4) | nibble4)){
                    PC = PC + 2;
                } 
            case 0x4:
                if(registers[nibble2] != ((nibble3 << 4) | nibble4)){
                    PC = PC + 2;
                }          
            case 0x5:
                if(registers[nibble2] == registers[nibble3]){
                    PC = PC + 2;
                } 
            case 0x6:
                registers[nibble2] = ((nibble3 << 4) | nibble4);
            case 0x7:
                 registers[nibble2] = registers[nibble2] + ((nibble3 << 4) | nibble4);
            case 0x8:
                 
            case 0x9:
                if(registers[nibble2] != registers[nibble3]){
                    PC = PC + 2;
                }      
            case 0xA:
                I = instr_data;
            case 0xB:
                PC = instr_data + registers[0];
            case 0xC:
                registers[nibble2] = (rand() & 0xFFFF) + ((nibble3 << 4) | nibble4);
            case 0xD:
                 
            case 0xE:
                 
            case 0xF:
                 
        }
        // execute emulator
    }
}