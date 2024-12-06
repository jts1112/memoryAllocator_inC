/*
Also including Notes from reading
*/

/*
When ALLOCATING
Splitting - (When requests are smaller than free chunk)Allocator will find a space that 
satisfies the request and splits it into two chunks. First chunk it will return to the 
caller. second chunk will remain on the list.

When FREEING
Coalescing - (When smaller chunks are close by) Allocator will combine the chunks to 
create a larger chunk so larger peices of memory can be allocated.
*/


/*
Header Block (stores extra needed information before chunk of memory)
- int size : sores the size of the chunk
- int magic : provides integrity checking
*/
//TODO define header structure
typedef struct header{
    int size;
    int magic;
}header_t;

typedef struct node{
    int size;
    struct node *next;
}node_t;


int init(int heap_size);

void print_free_list();

void* mem_alloc(int size);

void mem_free(void * pointer_to_free);


