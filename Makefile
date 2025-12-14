# These are found by running pkg-config, you better have it on your system
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

SRC = ./src
EXE = WRText-v0.1

all: # Compiles the release version of the application
	gcc $(CFLAGS) -o $(EXE) src/*.c $(LIBS) -DVER=\"$(EXE)\"
debug: # Compiles the debug version of the application
	gcc $(CFLAGS) -o $(EXE) src/*.c $(LIBS) -DVER=\"$(EXE)\" -DDEBUG -g -Wall
	
auto-style:
	clang-format -i src/*.c -style=file
check-style:
	clang-format -i src/*.c -style=file --dry-run
update-docs:
	doxygen Doxyfile

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=vg.log --verbose ./WRText-v0.1


clean:
	rm -f ./$(EXE)