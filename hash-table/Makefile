CC = clang
CFLAGS = -std=c18 -Wall -Wextra -Wpedantic

all: main

debug: CFLAGS += -O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
debug: main

release: CFLAGS += -O2 -g -march=native
release: main

main: main.c
	$(CC) $(CFLAGS) -o $@ $^

run: main
	./main
	
.PHONY: dist-clean
dist-clean:
	$(RM) -fv main
