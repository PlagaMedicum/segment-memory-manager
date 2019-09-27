#include "mmemory.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>  // TODO: remove it

// free_spaceof gets amount of free space in the memory x.
#define free_spaceof(x) (x.s - (size_t)(x.te->va + x.te->l) * sizeof(VA))
// TODO: Segmentation fault here...

// segm_end gets VA of last allocated block of the segment x.
#define segm_end(x) (x->va + x->l)

// segment_pa gets physical address of the segment x in vmem.
#define segment_pa(x) (vmem.b + (size_t)x->va * sizeof(VA))

// ST is a stack that represents segments table.
typedef struct ST
{
	VA va;                  // Segment VA. 
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

static MEMORY vmem;    // Virtual memory instance.

// request_memory allocates sz bytes of memory for provided pointer.
int request_memory (int sz)
{
    //int tptr = sbrk(0);
    int reqmem = sbrk(sz);
    if (reqmem == -1)
    {
        return -1;
    }

    return reqmem;
}

// find_swb returns physical adress of record in segment
// table with the segment containing block of ptr.
ST* find_swb (VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)segm_end(vmem.te)))
    {
        return NULL;
    }

    ST* segm = vmem.te;
    while (segm->p != NULL)
    {
        if (ptr > (VA)segm->va)
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

    size_t addr = request_memory(sizeof(ST*));
    if (addr == -1)
    {
        return 1;
    }
    vmem.te = (ST*)addr;

    addr = request_memory(szBlock * sizeof(VA));
    if (addr == -1)
    {
        return 1;
    }
    *ptr = (VA)addr;

    vmem.te->va = segm->va + segm->l;
    vmem.te->l = szBlock;
    vmem.te->p = segm;

    return 0;
}

int _free (VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)segm_end(vmem.te)))
    {
        return -1;
    }

    ST* segm = find_swb(ptr);
    if (segm == NULL)
    {
        return 1;
    }

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > (VA)segm_end(vmem.te)))
	{
		return -1;
	}
    
    ST* segm = find_swb(ptr);
    if (szBuffer > segm->l)
    {
        return -2;
    }
    if (segm == NULL)
    {
        return 1;
    }

    for (int i = 0; i < szBuffer; i++)
    {
        *((VA*)(pBuffer + i)) = *((VA*)(segment_pa(segm) + i));
    }

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > (VA)segm_end(vmem.te)))
    {
		return -1;
	}

    ST* segm = find_swb(ptr);
    if (szBuffer > segm->l)
    {
        return -2;
    }
    if (segm == NULL)
    {
        return 1;
    }

    for (int i = 0; i < szBuffer; i++)
    {
        *((VA*)(segment_pa(segm) + i)) = *((VA*)(pBuffer + i));
    }

    return 0;
}

int s_init (int n, int szPage)
{
	if ((n < 1) || (szPage < 1))
	{
		return -1;
	}

    size_t addr = request_memory(n * szPage * sizeof(VA*));
    if (addr == -1)
    {
        return 1;
    }
    vmem.b = (VA)addr;

    vmem.s = n * szPage;

	return 0;
}

