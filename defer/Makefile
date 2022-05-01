SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
BIN = main

# MMD will generate a .d file for each .c module containing its Makefile rules (including dependencies)
# These .d files are used with the `-include` bellow
CFLAGS  = -Wall -Wextra -Wpedantic -std=c18 -MMD
LDFLAGS = -Wall -Wextra -Wpedantic -std=c18

.PHONY: all release debug run distclean clean

all: debug

release: CFLAGS  += -O3
release: LDFLAGS += -O3
release: $(BIN)
	strip $(BIN)

ifeq ($(CC),clang)
debug: CFLAGS  += -fsanitize=address
debug: LDFLAGS += -fsanitize=address
endif

ifeq ($(CC),gcc)
debug: CFLAGS  += -fsanitize=address
debug: LDFLAGS += -fsanitize=address
endif

debug: CFLAGS  += -g -O0 -DDEBUG
debug: LDFLAGS += -g -O0
debug: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEP)

distclean: clean
	rm -f $(BIN)

clean:
	find . \
		-type f -name '*.d' -delete -o \
		-type f -name '*.o' -delete
