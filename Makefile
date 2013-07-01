CC = gcc
CFLAGS = -Wall -pedantic -ansi
BIN = cstat.exe
OBJ = err.o cp1250_ctype.o file.o hash_table.o stat.o parser.o main.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) -lm $^ -o $@

