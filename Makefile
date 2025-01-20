OBJS = run_emu.c
# OBJS = chip8.c
# HEADERS = chip8.h
CC = gcc
C_FLAGS = -w -pedantic
L_FLAGS = -lSDL2
OBJ_NAME = chip8

all: $(OBJS)
	$(CC) $(OBJS) $(HEADERS) $(C_FLAGS) $(L_FLAGS) 
# -o $(OBJ_NAME)