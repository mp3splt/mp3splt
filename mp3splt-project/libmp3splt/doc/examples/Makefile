all: clean compile run

compile: compile_test_library

clean: clean_test_library

run: run_library

run_v: run_test_library

clean_test_library:
	rm -rf test_library

compile_test_library:
	gcc $(CFLAGS) test_library.c -g -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -g -o test_library $(LDFLAGS) -lmp3splt

run_library: 
	./test_library

run_test_library:
	valgrind --leak-check=yes ./test_library
