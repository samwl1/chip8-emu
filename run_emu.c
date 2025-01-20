#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

uint16_t registers[16];
uint16_t instr;
uint16_t I = 0;
uint16_t PC = 0x200;
uint8_t memory[4096];
uint16_t stack[16];
uint16_t stack_ptr;
uint8_t gfx[64 * 32];
uint8_t keyboard[16];
uint_fast8_t draw_Flag;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *screen;
uint8_t delay_timer = 0;
uint8_t sound = 0;
uint16_t op_code_mask = 0xF000;
uint16_t nibble_mask_2 = 0x0F00;
uint16_t nibble_mask_3 = 0x00F0;
uint16_t nibble_mask_4 = 0x000F;
uint8_t chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void draw(void);
void operate(void);
void CleanUp_SDL(void);
uint32_t load_ROM_file(char* file);
void initChip8(void);
// Key Mappings

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
		// If you change SDL_RenderSetLogicalSize, change this accordingly.
		position.w = 64;
		position.h = 32;
		SDL_RenderCopy(renderer, screen, NULL, &position);
		SDL_RenderPresent(renderer);
	}
	draw_Flag = false;
}

int main (int argc, char ** argv)
{
    // Stolen display code.
	uint32_t quit=0;
	
	if (argc < 2)
	{
		printf("Usage: ./chip8 <rom> \n");
		return 0;
	}

	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 0;
	}
	SDL_Event event;

	window = SDL_CreateWindow(("CHIP-8:  %s",argv[1]),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,320,0);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, 64, 32);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);	

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);

	initChip8();
	if (load_ROM_file(argv[1]) == 0)
	{
		CleanUp_SDL();
		return 0;
	}
	int32_t speed=5;
	
	while(!quit)
	{
		printf("Speed: %d \n",speed);
		while(SDL_PollEvent(&event))
		{
				switch(event.type)
				{
					case SDL_QUIT:
					quit = 1;
					break;

					case SDL_KEYDOWN:

					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:quit = 1;break;
						case SDLK_F1:
							initChip8();
							if (load_ROM_file(argv[1]) == 0)
							{
								CleanUp_SDL();
								return 0;
							}
						break;
						case SDLK_F2:speed -= 1;break;
						case SDLK_F3:speed += 1;break;
						case SDLK_x:keyboard[0] = 1;break;
						case SDLK_1:keyboard[1] = 1;break;
						case SDLK_2:keyboard[2] = 1;break;
						case SDLK_3:keyboard[3] = 1;break;
						case SDLK_q:keyboard[4] = 1;break;
						case SDLK_w:keyboard[5] = 1;break;
						case SDLK_e:keyboard[6] = 1;break;
						case SDLK_a:keyboard[7] = 1;break;
						case SDLK_s:keyboard[8] = 1;break;
						case SDLK_d:keyboard[9] = 1;break;
						case SDLK_z:keyboard[0xA] = 1;break;
						case SDLK_c:keyboard[0xB] = 1;break;
						case SDLK_4:keyboard[0xC] = 1;break;
						case SDLK_r:keyboard[0xD] = 1;break;
						case SDLK_f:keyboard[0xE] = 1;break;
						case SDLK_v:keyboard[0xF] = 1;break;
					}
					break;
					
					case SDL_KEYUP:
					
					switch (event.key.keysym.sym)
					{
						case SDLK_x:keyboard[0] = 0;break;
						case SDLK_1:keyboard[1] = 0;break;
						case SDLK_2:keyboard[2] = 0;break;
						case SDLK_3:keyboard[3] = 0;break;
						case SDLK_q:keyboard[4] = 0;break;
						case SDLK_w:keyboard[5] = 0;break;
						case SDLK_e:keyboard[6] = 0;break;
						case SDLK_a:keyboard[7] = 0;break;
						case SDLK_s:keyboard[8] = 0;break;
						case SDLK_d:keyboard[9] = 0;break;
						case SDLK_z:keyboard[0xA] = 0;break;
						case SDLK_c:keyboard[0xB] = 0;break;
						case SDLK_4:keyboard[0xC] = 0;break;
						case SDLK_r:keyboard[0xD] = 0;break;
						case SDLK_f:keyboard[0xE] = 0;break;
						case SDLK_v:keyboard[0xF] = 0;break;
					}
					break;
				}
				break;
			}
			
		if(speed<0)
		{
			speed = 0;
		}
		else
		{
			SDL_Delay(speed);
		}
		if (delay_timer > 0) --delay_timer;
			
		operate();
		draw();

	}
	
	CleanUp_SDL();
	
	return 0;
}

uint32_t load_ROM_file(char* file){
	FILE * fp = fopen(file,"rb");
	
	if(fp == NULL)
	{
		fprintf(stderr,"Can't open the file rom \n");
		return 0;
	}	
	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp); 
	fseek(fp, 0 ,SEEK_SET);

	fread(memory+0x200,sizeof(uint16_t),size,fp);
	
	return 1;
}

//Initialize everything
void initChip8()
{	
	delay_timer= 0;
	sound= 0;
	instr = 0;
	PC = 0x200;
	I = 0;
	stack_ptr = 0;
	memset(stack,0,16);
	memset(memory,0,4096);
	memset(registers,0,16);
	memset(gfx,0,2048);
	memset(keyboard,0,16);
	// Load fonts
	memcpy(memory,chip8_fontset,80*sizeof(int8_t));
}

void operate(void){
    instr = memory[PC] << 8 | memory[PC + 1];
	PC +=2;
    uint16_t key_pressed;
	uint16_t X= (instr & 0x0F00) >> 8;
	uint16_t Y = (instr & 0x00F0) >> 4;
	uint16_t nnn = (instr & 0x0FFF);
	uint16_t kk = (instr & 0x00FF);
	uint16_t n = (instr & 0x000F);
    printf("Opcode: %x \n", instr);
    printf("Program Counter: %x \n",PC);
    printf("I: %x \n",I);
    switch(instr & 0xF000){

        case 0x0000:
            if(nnn == 0x00E0){
                memset(gfx, 0, 2048);
                draw_Flag = true;
            }
            else if (nnn == 0x00EE)
            {
                stack_ptr = stack_ptr - 1;
                PC = stack[stack_ptr];
            }
            else{
                printf("Opcode error 0xxx -> %x\n", nnn);
            }
            break;
            
        case 0x1000:
            PC = nnn;
            break;
        case 0x2000:
            stack[stack_ptr] = PC;
            stack_ptr += 1;
            PC = nnn;
            break;
        case 0x3000:
            if(registers[X] == kk){
                PC += 2;
            }
            break;
        case 0x4000:
            if(registers[X] != kk){
                PC += 2;
            }
            break;          
        case 0x5000:
            if(registers[X] == registers[Y]){
                PC += 2;
            }
            break; 
        case 0x6000:
            registers[X] = kk;
            break;
        case 0x7000:
            registers[X] += kk;
            break;
        case 0x8000:
            switch (n){
                case 0x0000:
                    registers[X] = registers[Y];
                    break;
                case 0x0001:
                    registers[X] |= registers[Y];
                    break;
                case 0x0002:
                    registers[X] &= registers[Y];
                    break;
                case 0x0003:
                    registers[X] ^= registers[Y];
                    break;
                case 0x0004:{
                    int i = (int)(registers[X]) + (int)(registers[Y]);
                    if(i > 255){
                        registers[0xF] = 1;
                    }
                    else{
                        registers[0xF] = 0;
                    }
                    registers[X] = i & 0xFF;
                }
                break;
                case 0x0005:
                    if(registers[X] > registers[Y]){
                        registers[0xF] = 1;
                    }
                    else{
                        registers[0xF] = 0;
                    }
                    registers[X] -= registers[Y];
                    break;
                case 0x0006:
                    registers[0xF] = registers[X] & 0x0001;
                    registers[X] = registers[X]/2;
                    break;
                case 0x0007:
                    if(registers[Y] > registers[X]){
                        registers[0xF] = 1;
                    }
                    else{
                        registers[0xF] = 0;
                    }
                    registers[X] = registers[Y] - registers[X];
                    break;
                case 0x000E:
                    registers[0xF] = registers[X] >> 7;
                    registers[X] = registers[X] * 2;
                    break;
                default: printf("Opcode error 8xxx -> %x\n",instr );
            }
            break;
        case 0x9000:
            if(registers[X] != registers[Y]){
                PC += 2;
            }
            break;      
        case 0xA000:
            I = nnn;
            break;
        case 0xB000:
            PC = nnn + registers[0];
            break;
        case 0xC000:
            registers[X] = (rand() % 0x100) & kk;
            break;
        case 0xD000:;
            uint16_t x = registers[X];
            uint16_t y = registers[Y];
            uint16_t height = n;
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
        case 0xE000:
            switch(kk){
                case 0x009E:
                    if(keyboard[registers[X]] != 0){
                        PC += 2;
                    }
                break;
                case 0x00A1:
                    if(keyboard[registers[X]] == 0){
                        PC += 2;
                    }
                break;
            }
            break;
        case 0xF000:
			switch(kk){
				case 0x0007:
                    registers[X] = delay_timer;
                    break;
            
				case 0x000A:
                    key_pressed = 0;
                    for(uint16_t i = 0; i<16; i++){
                        if (keyboard[i])
                        {
                            key_pressed = 1;
                            registers[X] = i;
                        }
                    }

                    if (key_pressed == 0){
                        PC -= 2;
                    }
                    break;
                break;

				case 0x0015:
				    delay_timer = registers[X];
				    break;

				case 0x0018:
				    sound = registers[X];
				    break;

				case 0x001E:
                    I = I + registers[X];
                    break;

				case 0x0029:
                    I = registers[X] * 5;
                    break;

				case 0x0033:{
					int num = registers[X];
					memory[I] = (num - (num % 100)) / 100;
					num -= memory[I] * 100;
					memory[I + 1] = (num - (num % 10)) / 10;
					num -= memory[I+1] * 10;
					memory[I + 2] = num;
				    break;
                }

				case 0x0055:

                    for (uint8_t i = 0; i <= X; ++i){
                        memory[I+ i] = registers[i];	
                    }
                    break;

				case 0x0065:
                    for (uint8_t i = 0; i <= X; ++i){
                        registers[i] = memory[I+ i];	
                    }
                    break;

			}
		break;	
        default: printf("OPCODE ERROR -> %x \n",instr);  
        break;               
    }
}

void CleanUp_SDL(void)
{
	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
