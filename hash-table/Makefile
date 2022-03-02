all: main

main: main.c
	$(CC) -std=c18 -Wall -Wextra -Wpedantic -O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer -o $@ $^

run: main
	./main
	
.PHONY: dist-clean
dist-clean:
	$(RM) -fv main
