CC=gcc
FLAGS=-Wall -g
OUT=a.out

FILES_C=src/main.c src/arguments.c
FILES_H=src/arguments.h

$(OUT): $(FILES_C) $(FILES_H)
	$(CC) $(FLAGS) $(FILES_C) -o $(OUT)

