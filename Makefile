# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -std=c99

# Libraries to link
LIBS = -lncurses -lasound

# Executable Name
TARGET = ./build/bin/catalyst-editor

# Source Files
SRCS = ./src/ui.c ./src/midi.c ./src/main.c

# Object Files
OBJS = $(SRCS:./src/%.c=./build/obj/%.o)

# Default Target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile .c files to .o files
./build/obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Clean up the build
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets to prevent conflicts with files of the same name
.PHONY: all clean

