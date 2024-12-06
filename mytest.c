#include "mem_alloc.h"
#include <stdio.h>
#include <unistd.h>


/// @brief Basic running test function saying if something passed or failed based on expected value etc.
/// @param test_number test number to be printed.
/// @param expected_result the expected result
/// @param actual_result  the actual result
void run_test(int test_number, int expected_result, int actual_result) {
    printf("----\n");
    printf("Test %d: %s\n", test_number, expected_result == actual_result ? "Passed" : "Failed");
    print_free_list();
    printf("---\n");
}


int main(int argc, char const *argv[])
{
     // Test 1: Initialize with negative heap size
    run_test(1, -1, init(-1));

    // Test 2: Initialize with zero heap size
    run_test(2, -1, init(0));

    // Test 3: Initialize with valid heap size
    run_test(3, 1, init(4096));

    // Test 4: Initialize again after successful initialization
    run_test(4, 0, init(4096));

    // Test 5: Allocate memory with negative size
    run_test(5, 0, mem_alloc(-1));

    // Test 6: Allocate memory with zero size
    run_test(6, 0, mem_alloc(0));

    // Test 7: Allocate memory with valid size, shouldnt return null
    void* ptr0 = mem_alloc(16);
    run_test(7, 1, ptr0 != NULL);
    mem_free(ptr0);

    // Test 8: Free memory with NULL pointer
    printf("----\n");
    printf("Test 8: Manual\n");
    printf("[Free List Before]\n");
    print_free_list();
    mem_free(NULL);
    printf("[Free List After]\n");
    print_free_list();
    printf("Assert print \"pointer to free is null\" and free list is unchanged\n");
    printf("---\n");

    // Test 9: Free memory with invalid pointer. Professor said its okay for it to segfault, since that is not anticipated.
    // printf("----\n");
    // printf("Test 9: Manual\n");
    // printf("[Free List Before]\n");
    // print_free_list();
    // mem_free((void*)0x12345678);
    // printf("[Free List After]\n");
    // print_free_list();
    // printf("Assert print \"pointer to free is null\" and free list is unchanged\n");
    // printf("---\n");

    // Test 10: Allocate and free memory, then allocate again
    void* ptr1 = mem_alloc(16);
    run_test(10, 1, ptr1 != NULL);
    mem_free(ptr1);
    ptr1 = mem_alloc(16);
    run_test(10, 1, ptr1 != NULL);
    mem_free(ptr1);

    // Test 11: Allocate and free until alloc returns null. since cant keep track of all pointers.
    int i;
    for (i = 0; i < 1000; i++) {
        void* ptr = mem_alloc(16 * (i + 1));
        if (ptr == NULL) {
            break;
        }
        mem_free(ptr);
    }
    run_test(11, 1, i < 1000);

    // Test 12: Allocate memory, free half, then allocate again
    void* ptr2 = mem_alloc(32);
    run_test(12, 1, ptr2 != NULL);
    mem_free(ptr2);
    ptr2 = mem_alloc(16);
    run_test(12, 1, ptr2 != NULL);
    mem_free(ptr2);

    // Test 13: Allocate memory, free all, then allocate again
    void* ptr3 = mem_alloc(64);
    run_test(13, 1, ptr3 != NULL);
    mem_free(ptr3);
    ptr3 = mem_alloc(64);
    run_test(13, 1, ptr3 != NULL);
    mem_free(ptr3);


    // Test 14.2: Allocate memory with large size. [NOTE] need to leave room for the free list header and the allocated memory header_t
    ptr3 = mem_alloc(4096 - sizeof(node_t)- sizeof(header_t));
    run_test(14, 1, ptr3 != NULL);
    mem_free(ptr3);

    // Test 14.2: Allocate memory just above largest possible size. should return null.
    ptr3 = mem_alloc(4096 - sizeof(node_t)- sizeof(header_t) + 1);
    run_test(14, 1, ptr3 == NULL);

    // Test 15: Allocate memory, free it, then allocate a smaller size
    void* ptr4 = mem_alloc(128);
    run_test(15, 1, ptr4 != NULL);
    mem_free(ptr4);
    ptr4 = mem_alloc(64);
    run_test(15, 1, ptr4 != NULL);
    mem_free(ptr4);

    // Test 16: Allocate memory, free it, then allocate a larger size
    void* ptr5 = mem_alloc(32);
    run_test(16, 1, ptr5 != NULL);
    mem_free(ptr5);
    ptr5 = mem_alloc(128);
    run_test(16, 1, ptr5 != NULL);
    mem_free(ptr5);

    // Test 17: Allocate memory, free it, then allocate the same size
    void* ptr6 = mem_alloc(256);
    run_test(17, 1, ptr6 != NULL);
    mem_free(ptr6);
    ptr6 = mem_alloc(256); // todo added
    run_test(17, 1, ptr6 != NULL); // added ptr instead of mem_alloc(256)
    mem_free(ptr6);

    // Test 18: Allocate memory, free it, then allocate a larger size than the free block
    void* ptr7 = mem_alloc(64);
    run_test(18, 1, ptr7 != NULL);
    mem_free(ptr7);
    ptr7 = mem_alloc(128);
    run_test(18, 1, ptr7 != NULL);
    mem_free(ptr7);

    // Test 19: Allocate memory, free it, then allocate a smaller size than the free block
    void* ptr8 = mem_alloc(128);
    run_test(19, 1, ptr8 != NULL);
    mem_free(ptr8);
    ptr8 = mem_alloc(64);
    run_test(19, 1, ptr8 != NULL);
    mem_free(ptr8);

    // Test 20: Allocate blocks of memory and free in order forcing coalescing cases of before, after and sandwich case.
    printf("\n\nTest 20 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp3->temp1->temp2\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    node_t* temp1 = (void*)mem_alloc(sizeof(node_t));
    node_t* temp2 = (void*)mem_alloc(sizeof(node_t));
    node_t* temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp3);
    mem_free(temp1);
    mem_free(temp2);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");



    printf("\n\nTest 21 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp1->temp2->temp3\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    temp1 = (void*)mem_alloc(sizeof(node_t));
    temp2 = (void*)mem_alloc(sizeof(node_t));
    temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp1);
    mem_free(temp2);
    mem_free(temp3);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");



    printf("\n\nTest 22 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp2->temp1->temp3\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    temp1 = (void*)mem_alloc(sizeof(node_t));
    temp2 = (void*)mem_alloc(sizeof(node_t));
    temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp2);
    mem_free(temp1);
    mem_free(temp3);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");



    printf("\n\nTest 23 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp2->temp3->temp1\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    temp1 = (void*)mem_alloc(sizeof(node_t));
    temp2 = (void*)mem_alloc(sizeof(node_t));
    temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp2);
    mem_free(temp3);
    mem_free(temp1);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");



    printf("\n\nTest 24 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp1->temp3->temp2\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    temp1 = (void*)mem_alloc(sizeof(node_t));
    temp2 = (void*)mem_alloc(sizeof(node_t));
    temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp1);
    mem_free(temp3);
    mem_free(temp2);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");



    printf("\n\nTest 25 :Scenario [temp1][temp2][temp3][free(head)]\n");
    printf("Free Order  temp3->temp2->temp1\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");

    temp1 = (void*)mem_alloc(sizeof(node_t));
    temp2 = (void*)mem_alloc(sizeof(node_t));
    temp3 = (void*)mem_alloc(sizeof(node_t));
    mem_free(temp3);
    mem_free(temp2);
    mem_free(temp1);

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");


    printf("\n\n Test 26: Allocating large and small and freeing in alternating orders\n");
    printf("----\n");
    printf("Initial FreeList (No allocations)\n");
    print_free_list();
    printf("----\n");    
    for (int i = 0; i < 100; i++)
    {
        node_t* tempptr = (void*)mem_alloc(4096 - sizeof(node_t)- sizeof(header_t) - sizeof(node_t) - sizeof(node_t)); // allocate large enough chunk to fit with space for one more allocation of size node_t
        node_t* tempptr1 = (void*) mem_alloc(sizeof(node_t));
        if (i%2 == 0)
        {
            mem_free(tempptr);
            mem_free(tempptr1);
        } else{
            mem_free(tempptr1);
            mem_free(tempptr);
        }
    }

    printf("Final FreeList (Should be The same)\n");
    print_free_list();
    printf("[All Memory should be free'd and the list should be how it initialy was.]\n");
    printf("----\n");
    

    return 0;
}
