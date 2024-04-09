.PHONY: test

CFLAGS=-I ../bstree-src -Wall -g #-DSENTINEL
CC=g++ -g -Wall
test: test-bstree
	
	./test-bstree
	valgrind ./test-bstree

test-bstree: test-bstree.o ../bstree-src/bstree.o

../bstree-src/bstree.o:
	$(MAKE) -C ../bstree-src bstree.o

clean:
	rm -f test-bstree *.o
