#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void solve(uint8_t puzzle[9][9]);

int complete(uint8_t puzzle[9][9])
{
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (!puzzle[i][j])
				return 0;
		}
	}
	return 1;
}

int subset(uint8_t puzzle[9][9], uint8_t solution[9][9])
{
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (puzzle[i][j] && puzzle[i][j] != solution[i][j])
				return 0;
		}
	}
	return 1;
}

int valid(uint8_t puzzle[9][9])
{
	uint8_t hits[9];
	int i, j, r, c;

	for (r = 0; r < 9; r++) {
		memset(hits, 0, 9);
		for (c = 0; c < 9; c++) {
			uint8_t digit = puzzle[r][c];
			if (digit) {
				digit--;
				if (hits[digit]) {
					return 0;
				}
				hits[digit] = 1;
			}
		}
	}

	for (c = 0; c < 9; c++) {
		memset(hits, 0, 9);
		for (r = 0; r < 9; r++) {
			uint8_t digit = puzzle[r][c];
			if (digit) {
				digit--;
				if (hits[digit]) {
					return 0;
				}
				hits[digit] = 1;
			}
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			memset(hits, 0, 9);
			for (r = 0; r < 3; r++) {
				for (c = 0; c < 3; c++) {
					uint8_t digit = puzzle[3*i+r][3*j+c];
					if (digit) {
						digit--;
						if (hits[digit]) {
							return 0;
						}
						hits[digit] = 1;
					}
				}
			}
		}
	}
	return 1;
}

void print(uint8_t puzzle[9][9])
{
	int i, j;
	for (i = 0; i < 9; i++) {
		if (i && i % 3 == 0) {
			for (j = 0; j < 9; j++) {
				if (j) {
					putchar('-');
					if (j % 3 == 0) {
						putchar('+');
						putchar('-');
					}
				}
				putchar('-');
			}
			putchar('\n');
		}

		for (j = 0; j < 9; j++) {
			uint8_t num = puzzle[i][j];
			if (j) {
				putchar(' ');
				if (j % 3 == 0) {
					putchar('|');
					putchar(' ');
				}
			}

			if (num)
				putchar(num + '0');
			else
				putchar('-');
		}
		putchar('\n');
	}
}

int load(char *filename, uint8_t puzzle[9][9])
{
	int i, j;
	FILE *infile;

	if ( (infile = fopen(filename, "r")) == NULL ) {
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
		return 1;
	}

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			int c = getc(infile);
			uint8_t digit;
			switch (c) {
			case ' ':
				digit = 0;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				digit = c - '0';
				break;
			case EOF:
				fprintf(stderr, "%s: Unexpected end of file\n", filename);
				return 1;
			default:
				fprintf(stderr, "%s: Invalid digit at (%d,%d): %c\n", filename, i, j, c);
				return 1;
			}
			puzzle[i][j] = digit;
		}
		int c = getc(infile);
		if (c != '\n') {
			fprintf(stderr, "%s: Expected newline character\n", filename);
			return 1;
		}
	}
	return 0;
}

int main(int argc, char * argv[])
{
	static uint8_t master[9][9];
	static uint8_t solution[9][9];

	uint8_t scratchpad[9][9];
	clock_t start, end;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <puzzle> <solution>\n", argv[0]);
		return 1;
	}

	if (load(argv[1], master) || load(argv[2], solution)) {
		return 1;
	}

	if (!valid(master)) {
		fprintf(stderr, "Invalid puzzle\n");
		return 1;
	}

	if (!complete(solution)) {
		fprintf(stderr, "Incomplete solution\n");
		return 1;
	}

	if (!valid(solution)) {
		fprintf(stderr, "Invalid solution\n");
		return 1;
	}

	if (!subset(master, solution)) {
		fprintf(stderr, "Solution does not match puzzle\n");
		return 1;
	}

	memcpy(scratchpad, master, 9*9);
	puts("Puzzle:");
	print(master);

	start = clock();
	solve(scratchpad);
	end = clock();

	printf("\nRan in %ld clock cycles:\n", end - start);
	print(scratchpad);

	if (!subset(master, scratchpad)) {
		fprintf(stderr, "Error: solve() function modified initial squares\n");
		return 1;
	}

	if (!complete(scratchpad)) {
		int correct = 0;
		int filled = 0;
		int total = 0;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (master[i][j] == 0) {
					total++;
					if (scratchpad[i][j]) {
						filled++;
						if (scratchpad[i][j] == solution[i][j]) {
							correct++;
						} else {
							printf("Incorrect square: (%d,%d)\n", i, j);
						}
					}
				}
			}
		}
		printf("Filled %d out of %d squares\n", filled, total);
		printf("Got %d out of %d correct\n", correct, filled);
	} else {
		puts("Congratulations! You successfully solved the puzzle");
	}

	puts("\nCorrect solution:");
	print(solution);

	return 0;
}
