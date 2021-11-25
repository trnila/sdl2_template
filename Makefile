# compiler flags
# -MMD generates dependencies on header files, so .c will will be recompiled on header modification
CFLAGS += -g -Wall -Wextra -MMD
# linker flags
LDFLAGS += -lm -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# the name of our program
PROGRAM = game

# $^ - inputs (main.o image.o)
# $@ - outputs (main)
# output: inputs
$(PROGRAM): main.o sdl.o # add more object files here...
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

# run to delete all compiled files
# $ make clean
clean:
	rm -f *.o $(PROGRAM)

# include *.d file with dependencies on header files
-include *.d
