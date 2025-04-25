CC=gcc

FLAGS=-Wall -g -lm
#FLAGS=-Wall -fsanitize=address -g -lm
#FLAGS=-Wall -O3 -march=native -lm

OUT=a.out

FILES_C=src/main.c src/arguments.c src/file.c src/graph.c src/array.c src/simple_cut.c src/list.c src/cut.c src/karger_cut.c src/errors.c
FILES_H=src/arguments.h src/file.h src/graph.h src/array.h src/simple_cut.h src/list.h src/cut.h src/karger_cut.h src/errors.h

$(OUT): $(FILES_C) $(FILES_H)
	$(CC) $(FLAGS) $(FILES_C) -o $(OUT)

