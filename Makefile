CC=gcc
FLAGS=-Wall -g
OUT=a.out

FILES_C=src/main.c src/arguments.c src/file.c src/graph.c src/array.c
FILES_H=src/arguments.h src/file.h src/graph.h src/array.h

$(OUT): $(FILES_C) $(FILES_H)
	$(CC) $(FLAGS) $(FILES_C) -o $(OUT)

