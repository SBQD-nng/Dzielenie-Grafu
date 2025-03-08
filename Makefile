CC=gcc
FLAGS=-Wall -g
OUT=a.out

FILES_C=src/main.c
FILES_H=

$(OUT): $(FILES_C) $(FILES_H)
	$(CC) $(FLAGS) $(FILES_C) -o $(OUT)

