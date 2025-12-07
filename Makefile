# These are found by running pkg-config, you better have it on your system
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

SRC = ./src
EXE = WRText-v0.1

all:
	gcc $(CFLAGS) -o $(EXE) src/*.c $(LIBS)
auto-style:
	clang-format -i src/*.c -style=file
check-style:
	clang-format -i src/*.c -style=file --dry-run
update-docs:
	doxygen Doxyfile


clean:
	rm -f ./$(EXE)