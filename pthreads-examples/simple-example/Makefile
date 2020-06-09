CC = clang
CFLAGS = -Wall -pedantic -g -O0 -std=c99

simple-example: main.c thread_context.h
	$(CC) $(CFLAGS) main.c -o simple-example

%.o : %.c
    $(CC) $(CFLAGS) $< -c
