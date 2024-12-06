//
// Created by aleksey on 3/27/24.
//
#include "mem_alloc.h"
#include <stdio.h>
#include <unistd.h>

void test2() {
    int pgSize = getpagesize();
    int memSize = pgSize * 2;
    printf("-----------------------------------------------\n");
    printf("Test 2: Allocating %d pages for a total size of %d\n", 2, memSize);
    int rc = init(memSize);
    if (rc == -1) {
        printf("Test 2: Error initializing\n");
        return;
    }

    int arraySize = 512;
    int allocSize = sizeof(int) * arraySize;
    int expectedSum = (arraySize / 2) * (arraySize - 1);

    void *mem = NULL;
    void* mems[memSize / allocSize];
    int memCounter = 0;
    do {
        mem = mem_alloc(allocSize);
        if (mem == NULL) {
            printf("Test 2: Allocation number %d failed\n", memCounter);
        } else {

            int *array = (int *) mem;
            for (int i = 0; i < arraySize; i++) {
                array[i] = i;
            }
            int sum = 0;
            for (int i = 0; i < arraySize; i++) {
                sum += array[i];
            }

            if (sum != expectedSum) {
                printf("Test 2: Array for allocation %d is corrupt: sum is %d, expecting %d\n", memCounter, sum,
                       expectedSum);
            }
            mems[memCounter++] = mem; // keep the pointer  so that we can free it later;
        }
    } while (mem != NULL);

    printf("Test 2: Allocated %d arrays\n", memCounter);


    for (int i = 0; i < memCounter; i++) {
        print_free_list();
        printf("Test 2: Freeing memory allocation %d\n", i);
        mem_free(mems[i]);
    }
    print_free_list();
}

int main(int argc, char *argv[]) {
    printf("MemAllocator Tester\n");
    test2();
}
