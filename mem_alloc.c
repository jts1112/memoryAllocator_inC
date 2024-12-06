#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "mem_alloc.h"

#define DEBUG 0 // 1 for Debug print statements. Else no debug statements
#define TESTS 1 // 1 for printing tests results. else no tests being printed.

node_t *head = NULL; // global variable to keep track of the head of the free list.


/// @brief Initialize the heap 
/// @param heap_size  the size of the heap to be dealt with
/// @return returns -1 if error, 0 if called after success, return 1 if success
int init(int heap_size) {

    if (heap_size <= 0)
    {
        fprintf(stderr,"Error Heap Size < 0\n");
        return -1;
    }

    if (head != NULL) // already had successful allocation
    {
        return 0;
    }
    
    // check to see if heap_size is a multiple of pagesize;
    int pagesize = getpagesize();
    printf("page size is %d\n",pagesize);
    int dif = heap_size % pagesize;
    if (dif != 0)
    {
        heap_size += dif;
    }
    
    
    head = mmap(NULL,heap_size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,-1,0);

    // check if the mmap was successful
    if (head == MAP_FAILED)
    {
        fprintf(stderr,"nmap call failed\n");
        return -1;
    }
    
    head->size = heap_size - sizeof(node_t);
    head->next = NULL;
    return 1;
}


// TODO
/// @brief Allocattes memory in the heap and suitable large chunk into 2 sub chunks
/// @param size size of bits to be allocated
/// @return a pointer to the allocated memory
void* mem_alloc(int size) {
    
     if (size <= 0) // size needs to be a valid number of bits to allocate
    {
        return NULL;
    }

    // get head of the free list.
    node_t *temphead = head;
    if(DEBUG) printf("Address of initial head %x\n",head);
    int iterCount = 0;

    // find a suitable block size that is large enough for sizeof(header) and requested size.
    while (temphead != NULL)
    {
        if (DEBUG == 1){printf("Free block of size %d\n", temphead->size);}

        if (temphead->size >= sizeof(header_t) + size)
        {
            if (DEBUG == 1){printf("Found suitable block size of %d for requested size of %d\n",temphead->size,size);}
            
            // // need to split the node and return a pointer to it.
            if(DEBUG) printf("Address of temphead %x\n",temphead);
            int tempHeadSize = temphead->size;
            node_t* tempHeadNext_ptr = temphead->next;
            header_t *allocatedBlock = (header_t*)(temphead);

            if(DEBUG) printf("Adress of allocated block %x\n",allocatedBlock);
            allocatedBlock->magic = 123456;
            allocatedBlock->size = size;

            // part of the split block that isnt being allocated.
            node_t *freeBlock = (node_t*)(allocatedBlock + (sizeof(header_t) + size) / 8);
            if(DEBUG) printf("Address of freeblock %x\n",freeBlock);
            freeBlock->size = tempHeadSize - size - sizeof(header_t);
            freeBlock->next = tempHeadNext_ptr;

            // if was first node need to update head
            if (iterCount == 0)
            {
                if(DEBUG) printf("setting head to the free block\n");
                head = freeBlock;
            }
            
            if(DEBUG) printf("header start adress is %x\n",allocatedBlock);
            if(DEBUG) printf("returned header adress is %x\n",(void*) allocatedBlock + (sizeof(header_t)/8));

            return (void*) allocatedBlock + sizeof(header_t);
        }
        iterCount++;
        temphead = temphead->next;
    }
    
    // no available memory was able to be found so can return null
    if (DEBUG)
    {
        printf("Unable to find available memory suitable for size %d\n",size+(int)sizeof(header_t));
    }
    
    return NULL ;
}

// TODO
/// @brief free's a given chunk of memory given a pointer to it and coalessces 
/// nearby chunks into larger chunks
/// @param pointer_to_free 
void mem_free(void * pointer_to_free) {
    if (pointer_to_free == NULL)
    {
        printf("Pointer to free is null\n");
        return;
    }
    
    // find the block to free.
    header_t* blockToFree = (header_t*) ((char*) pointer_to_free - sizeof(header_t));
    if(DEBUG) printf("Pointer to free is %x\n",pointer_to_free);
    if(DEBUG) printf("block to free adress is %x\n",blockToFree);
    // memory integrity check to see if stuff has been overwritten
    if (blockToFree->magic != 123456)
    {
        printf("[Error] Memory has been overwritten for this block\n");
        return;
    }
    
    if(DEBUG) printf("Block %x to free size: %d\n",blockToFree,blockToFree->size);

    // convert block to  free into a free list node to attatch to the free list.
    int blocksize = blockToFree->size;
    node_t *addedNode = (node_t*)blockToFree;

    // note added size is the total space the node is taking up including the block size and the header size.
    addedNode->size = sizeof(header_t) + blocksize;
    addedNode->next = NULL;
 
    // Idea on finding adjacent nodes
    // walk through the free list and save stuff as we walk through
    node_t *tempHead = head;
    node_t * previous = NULL; // for case 0 when at the tail.
    node_t * addedBefore = NULL; // pointer to hold the adress if added node before free list node
    node_t * addedAfter = NULL; // pointer to hold the adress if added node after free list node
    node_t * savedPrevious = NULL; // pinter to previous for added before case.
    while (tempHead != NULL)
    {
        // case 1 check if added before has occured
        if (((void*) addedNode + addedNode->size == tempHead))
        {
            addedBefore = tempHead;
            savedPrevious = previous;
        }
        
        // case 2 check if added after has occured
        if ((void*)tempHead + tempHead->size == addedNode)
        {
            addedAfter = tempHead;
        }

        // else go to the next head no adacent nodes found.
        previous = tempHead;
        tempHead = tempHead->next;
    }

    // Case 0 if both added after and added before are null then no adacent found add added to end of list
   if (addedAfter == NULL && addedBefore == NULL)
   {
        if(DEBUG) printf("Case 0: Adding addedNode to end of FreeList\n");
        previous->next = addedNode;
        return;
   }
   
   // Case 1 if Both added after and added before are not null then sandwich case.
   if (addedAfter != NULL && addedBefore != NULL)
   {
        if(DEBUG) printf("Case 1: Sandwich case\n");
        // added node After scenario first
        addedAfter->size += addedNode->size;
        addedAfter->next = addedNode->next;

        // added node Before scenario after but now addedAfter is the addedNode.
        if (savedPrevious == NULL)
        { // added after is the head of the free list
            if(DEBUG) printf("added node has become the head\n");
            addedAfter->size += addedBefore->size;
            head = addedAfter;
            head->next = addedAfter->next;
            return;

        } else {  //  TODO need more testing added after is not the head of the free list
            if(DEBUG) printf("added node is not becoming the head\n");
            addedAfter->size += addedBefore->size;
            savedPrevious->next = addedAfter;
            return;
        }
   }

   // Case 2 if added after isnt null then merge addedAfter with addedNode.
   if (addedAfter != NULL)
   {
        if(DEBUG) printf("Case 2: Added node is after a free list node\n");
        addedAfter->size += addedNode->size; // TODO implemented just working on case 1 for now.
        addedAfter->next = addedNode->next;
        return;
   }

   // Case 3 if added before isnt null then     
   if (addedBefore != NULL)
   {
        if(DEBUG) printf("Case 3: Added node is before a free list node\n");
        if (savedPrevious == NULL)
        { // added after is the head of the free list
            if(DEBUG) printf("added node has become the head\n");
            addedNode->next = head->next;
            addedNode->size += head->size;
            head = addedNode;
            return;

        } else { // added after is not the head of the free list
            if(DEBUG) printf("added node is not becoming the head\n");
            addedNode->next = addedBefore->next;
            addedNode->size += addedBefore->size;
            savedPrevious->next = addedNode;
            return;
        }
   }
   if (DEBUG) printf("Something Happened!!!!\n");
   
   
   return;
}


/**
 * Simple method that will print the the free list in the order of 
 * which it is traversed.
*/
void print_free_list(){
    node_t* tempHead = head;
    while (tempHead != NULL)
    {
        printf("[Adresss: %x Size: %d Next: %x]\n",tempHead,tempHead->size,tempHead->next);
        tempHead = tempHead->next;
    }
}
