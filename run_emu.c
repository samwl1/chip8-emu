#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
// need to create register variables, global if possible, maybe a struct?
int16_t registers[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t I = 0;
int16_t PC = 0x200;
int8_t memory[4096];
int16_t stack[16];
int16_t stack_ptr = 0;
uint8_t gfx[64 * 32];
uint8_t keyboard[16];
uint_fast8_t draw_Flag;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;
uint8_t delay_timer = 0;
uint8_t sound = 0;
int16_t op_code_mask = 0xF000;
int16_t nibble_mask_2 = 0x0F00;
int16_t nibble_mask_3 = 0x00F0;
int16_t nibble_mask_4 = 0x000F;

// Each register (16 total) will start with V and have 0-F second digit as a name.
    // VF should not be touched. Also need an 16 bit register I.
// 2 8-bit registers for sound and delay.
// 16 layer stack of 16-bit values
// 16-bit PC
// Need user input, 16 key hex keyboard (1234qwerasdfzxcv is common)
// Key Mappings
char key_mappings[] = {'1', '2', '3', '4', 'q', 'w', 'e', 'r', 'a', 's', 'd', 'f', 'z', 'x', 'c', 'v'};

void draw()
{
	uint32_t pixels[64 * 32];
	unsigned int x, y;
		
	if (draw_Flag)
	{
		memset(pixels, 0, (64 * 32) * 4);
		for(x=0;x<64;x++)
		{
			for(y=0;y<32;y++)
			{
				if (gfx[(x) + ((y) * 64)] == 1)
				{
					pixels[(x) + ((y) * 64)] = UINT32_MAX;
				}
			}
		}
		
		SDL_UpdateTexture(screen, NULL, pixels, 64 * sizeof(uint32_t));
	
		SDL_Rect position;
		position.x = 0;
		position.y = 0;
		position.w = 64;
		position.h = 32;
		SDL_RenderCopy(renderer, screen, NULL, &position);
		SDL_RenderPresent(renderer);
	}
	draw_Flag = false;
}

int main(int args, char ** argc){
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);
    bool quit = false;
	memset(stack,0,16);
	memset(memory,0,4096);
	memset(gfx,0,2048);
    memset(keyboard,0,16);
    
    while(!quit){
        
    }
}

void operate(){
    int16_t instr = memory[PC]; // get first instruction
    PC += 2;
    uint16_t key_pressed;
    int16_t nibble1 = op_code_mask & instr >> 12;
    int16_t nibble2 = nibble_mask_2 & instr >> 8;
    int16_t nibble3 = nibble_mask_3 & instr >> 4;
    int16_t nibble4 = nibble_mask_4 & instr;
    int16_t instr_data = 0x0FFF & instr;
    printf("Opcode: %x \n", nibble1);
    printf("Program Counter: %x \n",PC);
    printf("I: %x \n",I);
    switch(nibble1){
        case 0x0:
            if(instr_data == 0x0E0){
                memset(gfx, 0, 2048);
                draw_Flag = true;
                break;
            }
            else if (instr_data == 0x0EE)
            {
                stack_ptr = stack_ptr - 1;
                PC = stack[stack_ptr];
            }
            else{
                printf("Opcode error 0xxx -> %x\n", instr_data);
            }
            
        case 0x1:
            PC = instr_data;
        case 0x2:
            stack_ptr = stack_ptr + 1;
            stack[stack_ptr] = PC;
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
            switch (nibble4)
            {
            case 0:
                registers[nibble2] = registers[nibble3];
                break;
            case 1:
                registers[nibble2] = registers[nibble2] | registers[nibble3];
                break;
            case 2:
                registers[nibble2] = registers[nibble2] & registers[nibble3];
                break;
            case 3:
                registers[nibble2] = registers[nibble2] ^ registers[nibble3];
                break;
            case 4:
                int16_t tmp = registers[nibble2] + registers[nibble3];
                if(tmp > 255){
                    registers[0xF] = 1;
                }
                else{
                    registers[0xF] = 0;
                }
                registers[nibble2] = tmp & 0x00FF;
                break;
            case 5:
                if(registers[nibble2] > registers[nibble3]){
                    registers[0xF] = 1;
                }
                else{
                    registers[0xF] = 0;
                }
                registers[nibble2] = registers[nibble2] - registers[nibble3];
                break;
            case 6:
                if(registers[nibble2] & 0x1 == 0x1){
                    registers[0xF] = 1;
                }
                else{
                    registers[0xF] = 0;
                }
                registers[nibble2] = registers[nibble2] / 2;
                break;
            case 7:
                if(registers[nibble3] > registers[nibble2]){
                    registers[0xF] = 1;
                }
                else{
                    registers[0xF] = 0;
                }
                registers[nibble3] = registers[nibble3] - registers[nibble2];
                break;
            case 0xE:
                int16_t tmp = registers[nibble2] & 0x8000;
                tmp = tmp >> 15;
                if(tmp == 0x1){
                    registers[0xF] = 1;
                }
                else{
                    registers[0xF] = 0;
                }
                registers[nibble2] = registers[nibble2] * 2;
                break;
            default:
            break;
            }
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
            uint16_t x = registers[nibble2];
            uint16_t y = registers[nibble3];
            uint16_t height = nibble4;
            uint8_t pixel;

            registers[0xF] = 0;
            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++) {
                    if((pixel & (0x80 >> xline)) != 0) {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1){
                            registers[0xF] = 1;                                   
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            draw_Flag = true;
            break;
        case 0xE:
            
            if(nibble3 == 9){
                if(key_mappings[nibble2] != 0){
                    PC += 2;
                }
            }
            else if(nibble3 == 0xA){
                if(key_mappings[nibble2] == 0){
                    PC += 2;
                }
            }
            else{
                printf("Opcode error 0xxx -> %x\n", instr_data);
            }
        case 0xF:
			switch((nibble3 << 4) | nibble4){
				case 0x7:
                    registers[nibble2] = delay_timer;
                    break;
            
				case 0xA:
                    key_pressed = 0;
                    for(uint16_t i = 0; i<16; i++){
                        if (keyboard[i])
                        {
                            key_pressed = 1;
                            registers[nibble2] = i;
                        }
                    }

                    if (key_pressed == 0){
                        PC -= 2;
                    }
                    break;

				case 0x15:
				    delay_timer = registers[nibble2];
				    break;

				case 0x0018:
				    sound = registers[nibble2];
				    break;

				case 0x1E:
                    I = I + registers[nibble2];
                    break;

				case 0x29:
                    I = registers[nibble2] * 5;
                    break;

				case 0x33:
					int16_t num = registers[nibble2];
					memory[I] = (num - (num % 100)) / 100;
					num -= memory[I] * 100;
					memory[I + 1] = (num - (num % 10)) / 10;
					num -= memory[I+1] * 10;
					memory[I + 2] = num;
				    break;

				case 0x55:

                    for (uint8_t i = 0; i <= nibble2; ++i){
                        memory[I+ i] = registers[i];	
                    }
                    break;

				case 0x65:
                    for (uint8_t i = 0; i <= nibble2; ++i){
                        registers[i] = memory[I+ i];	
                    }
                    break;

			}
			break;	

        default: printf("OPCODE ERROR -> %x \n",instr); 
        break;
                
    }
}

