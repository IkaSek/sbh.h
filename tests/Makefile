CC ?= cc
CFLAGS += -std=c99 -Wall -Wextra -g -O0
TESTS = $(wildcard *.c)
TESTS_O = $(patsubst %.c, %.exe, $(TESTS))


%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@

tests: $(TESTS_O)
clean: 
	rm -rf $(TESTS_O)

all: tests
.PHONY: tests
