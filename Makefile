CC := gcc

all: main.o solver.o
	gcc main.o solver.o -o sudoku
main.o: main.c
solver.o: solver.c

clean:
	@$(RM) *.o sudoku
