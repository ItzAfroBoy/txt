CC=gcc
CFLAGS=-Wall -I. -std=c99
BIN=/usr/local/bin
.PHONY: clean install purge

txt: txt.c
	@$(CC) $^ -o $@ $(CFLAGS)

install:
	@sudo cp txt $(BIN)

purge: clean
	@sudo rm $(BIN)/txt

clean:
	@rm txt
