#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "mymalloc.h"

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8
void dump_heap();
unsigned int round_up_size(unsigned int data_size)
{
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

struct Block {
	int occupied; //decides if block is occupied
	int size; //size of block- this include header
	struct Block *prev; //pointer to the previous block
	struct Block *next; //pointer to the next block
};

static struct Block *head = NULL; //head of the L list
static struct Block *tail = NULL; //tail of the L list

void dump_heap()
	{
		struct Block *curr;
		printf("brk: %p\n", sbrk(0));
		printf("head ->");
		
		for (curr = head; curr != NULL; curr = curr-> next)
		{
			printf("[%d:%d:%d]->", curr-> occupied, (char*) curr - (char*) head, curr-> size);
			fflush(stdout);
			assert((char*) curr >= (char*) head && (char*) curr + curr->size <= (char*)sbrk(0));
			//^checks that block is within bounds of the heap
			if(curr-> next != NULL)
			{
				assert(curr->next->prev == curr);
				//^if its not the last block, checks that head & tail links are correct
				assert((char*) curr + curr-> size == (char*) curr -> next);
				//^checks that the block size is correctly set
			}
		}
		printf("null\n");
	}

void *my_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;

	size = round_up_size(size);
	 //= dump_heap();
	// ------- Don't remove anything above this line. -------
	// Here's where you'll put your code!
struct Block *curr = head; //head of L list
struct Block *best = NULL; 

while (curr != NULL) //check if it is occupied
{								//1 = if that memory was already allocated and has stuff in it
	if (curr -> occupied == 1) //0 = there is free space in memory
	{ 
		curr = curr -> next;
		
	}
	else if (curr -> occupied == 0)
	{
		if((curr-> size - sizeof(struct Block)) > size) //if its open check if it has enough memory
		{
			if(best == NULL)
			{ 
				//if(curr->size > sizeof(struct Block) + size)
					best = curr;
				
			}
			else if ((curr->size - sizeof(struct Block)) - size < (best->size - sizeof(struct Block)) - size) //check if it is better than previous best
			{
				best = curr;
				printf("we here");
			}
		} else if (curr->size == size + sizeof(struct Block)) {
			return (void*) curr + sizeof(struct Block);
		}	
		curr = curr-> next;
	}
	else
	{
		printf("Occupied shouldonly be printing 1 or 0 oh god lol");
		return NULL;
	}
}	//end of curr != Null while loop

if(best != NULL)
{ //if not null and enough room for 2 blocks, split em
	if(best->size > 2*sizeof(struct Block) + size)
	{//link both blocks within the list
		struct Block *freeblock = best + sizeof(struct Block) + size;
		struct Block *block = best;
		
		freeblock -> size = best -> size - (sizeof(struct Block) + size);
		freeblock -> prev = block;
		freeblock -> next = best-> next;
		best -> next -> prev = freeblock;
		freeblock -> occupied = 0;
		
		block-> size = sizeof(struct Block) + size;
		block -> prev = best-> prev;
		best ->prev->next = block;
		block -> next = freeblock;
		block -> occupied = 1;
		return block + sizeof(struct Block);
		
	}
	else {
		return best + sizeof(struct Block);
	}
}

//if there is no empty space, gotta grow the heap...

struct Block *block = sbrk(sizeof(struct Block));
if (block == (void*) -1 )
{
	return NULL;
}
block -> occupied = 1;
block -> size = sizeof(struct Block) + size;
if (head == NULL)
{
	head = block;
	tail = head;
	head -> prev = NULL;
	head -> next = NULL;
}
else 
{//extends heap in else
	tail -> next = block;
	block -> prev = tail;
	tail = block;
	tail -> next = NULL;
}




return sbrk(size);
}





void my_free(void* ptr)
{
	if(ptr == NULL)
		return;

	
	//ok lets start freeing stuff 
	struct Block *block = ptr - sizeof(struct Block);
	struct Block *left = block -> prev;
	struct Block *right = block -> next;
	
	//start coalescing if previous block is free
	if (left != NULL && left -> occupied == 0)
	{
		int boolean = 0;
			if (block == tail)
			{
				boolean = 1;
			}
		left -> size = left -> size + block -> size;
		//^combines the 2 blocks into the leftmost block
		
		left -> next = right;
		 
		if (right != NULL)
		{	//relink stuff 
			right -> prev = left;
		}
		block = left; //reset reference 
		
		if(boolean == 1)
		{
			tail = block; //set tail reference if needed
		}
	
	} //end of left being valid and not occupied if statement
	
	if (right != NULL && right -> occupied == 0)
	{
		block -> size = block -> size + right -> size;
		//^ combines into most left block
		block-> next = right-> next;
		if(block -> next != NULL && block -> next -> prev != NULL)
		{
			block -> next -> prev = block;
		}
	} //end of right not being null
	block -> occupied = 0;
	
	if (tail != NULL && tail -> occupied == 0)
		//decrement if the tail is empty
	{
		if(tail -> prev != NULL)
		{
			tail = tail -> prev;
		}
		if(block == head && (head -> next == NULL))
		{
			head-> size = 0;
		}
		sbrk(-(tail->size));
	}
}
	
	
	
	
	
	
