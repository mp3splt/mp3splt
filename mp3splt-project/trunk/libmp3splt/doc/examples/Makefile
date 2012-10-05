all: clean compile

compile: compile_all

clean: clean_all

clean_all:
	rm -rf minimal

compile_all:
	gcc $(CFLAGS) minimal.c -g -Wall -g -o minimal -lmp3splt $(LDFLAGS)

