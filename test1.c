//
// Created by aleksey on 3/27/24.
//
#include "mem_alloc.h"
#include <stdio.h>
#include <unistd.h>

void test1() {
    int pgSize = getpagesize();
    int memSize = pgSize * 2;
    printf("Test 1: Allocating %d pages for a total size of %d\n", 2, memSize);
    int rc = init(memSize);
    if (rc == -1) {
        printf("Test 1: Error initializing\n");
        return;
    }
    void *mem = NULL;
    while (mem == NULL) {
        mem = mem_alloc(memSize);
        if (mem == NULL) {
            printf("Test 1: Cannot allocate %d bytes\n", memSize);
            memSize--;
        }
    }
    printf("Test 1: Allocated %d bytes\n", memSize);

    int *array = (int*) mem;
    int arraySize = memSize / sizeof(int) - 1;
    for (int i = 0; i < arraySize; i++) {
        array[i] = i;
    }

    printf("Test 1: Set %d integers\n", arraySize);

    int sum = 0;
    for (int i = 0; i < arraySize; i++) {
        sum += array[i];
    }

    int expectedSum = (arraySize / 2.0) * (arraySize - 1);
    if (sum == expectedSum) {
        printf("Test 1: All values are OK\n");
    } else {
        printf("Test 1: Array is corrupt: sum is %d, expecting %d\n", sum, expectedSum);
    }

    print_free_list();

    printf("Test 1: Freeing memory\n");
    mem_free(mem);

    print_free_list();

}

int main(int argc, char *argv[]) {
    printf("MemAllocator Tester\n");
    test1();
}
