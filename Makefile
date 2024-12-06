CC=gcc
CFLAGS=-c -fPIC
LDFLAGS=-shared -o libmemalloc.so
LIBS=-L. -lmemalloc

all: libmemalloc.so test1 mytest

libmemalloc.so: mem_alloc.o
	$(CC) $(LDFLAGS) $^

test1: test1.o
	$(CC) -o $@ $^ $(LIBS)

mytest: mytest.o
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o libmemalloc.so test1 mytest

## make run-test1 will run the test1 that you provided to test out the memory allocator program
run-test1: test1
	LD_LIBRARY_PATH=. ./test1

## make run-mytset will run the tests that I have made for my program and what I used to test the correctness of my program
run-mytest: mytest
	LD_LIBRARY_PATH=. ./mytest

## make run-all will run all the tests used to test my program including ones provided by you.
run-all: test1 mytest
	LD_LIBRARY_PATH=. ./test1
	LD_LIBRARY_PATH=. ./mytest
