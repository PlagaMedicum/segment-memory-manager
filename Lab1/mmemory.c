#include "mmemory.h"
#include <stdbool.h>
#include <stdlib.h>

// free_spaceof gets amount of free space in the memory.
#define free_spaceof(x) (x.s - (x.te->va + x.te->l) * sizeof(VA))

// eo_alloc gets VA of last allocated block of the segment x.
#define eo_alloc(x) (x->va + x->l)

// ST is a stack that represents segments table.
typedef struct
{
	size_t va;              // Segment VA. 
                            // TODO: Maybe need to change size_t -> VA.
    size_t l;               // Length of the segment.
	struct ST* p;           // Physical address of prev segment's VA.
} ST;

// MEMORY represents virtual memory
typedef struct 
{
	VA b;		    // Points to the first block of memory.
	size_t s;	    // Number of blocks in memory.
    ST* te;         // Points to the ending of segments table stack.
} MEMORY;

MEMORY vmem;    // Virtual memory instance.

// find_swb returns physical adress of record in segment
// table with the segment containing block of ptr.
ST* find_swb (VA ptr)
{
    if ((ptr < 0) || (ptr > eo_alloc(vmem.te)))
    {
        return NULL;
    }

    ST* segm = vmem.te;
    while (segm->p != NULL)
    {
        if (ptr > segm->va)
        {
            return segm;
        }

        segm = segm->p;
    }

    return NULL;
}

int _malloc (VA* ptr, size_t szBlock)
{
	if (szBlock > free_spaceof(vmem))
	{
		return -2;
	}

    ST* segm = vmem.te;
    vmem.te = malloc(sizeof(ST));
    vmem.te->va = segm->va + segm->l;
    vmem.te->l = szBlock;
    vmem.te->p = segm;

    ptr = malloc(szBlock * sizeof(VA));

    return 0;
}

int _free (VA ptr)
{
    if ((ptr < 0) || (ptr > eo_alloc(vmem.te)))
    {
        return -1;
    }

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > eo_alloc(vmem.te)))
	{
		return -1;
	}
    
    ST* segm = find_swb(ptr);

    if ((segm == NULL) || (szBuffer > segm->l))
    {
        return -2;
    }

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > eo_alloc(vmem.te)))
    {
		return -1;
	}

    ST* segm = find_swb(ptr);

    if ((segm == NULL) || (szBuffer > segm->l))
    {
        return -2;
    }

    return 0;
}

int s_init (int n, int szPage)
{
	if ((n < 1) || (szPage < 1))
	{
		return -1;
	}

	vmem.b = malloc(n * szPage * sizeof(VA*));
    vmem.s = n * szPage;

	return 0;
}

