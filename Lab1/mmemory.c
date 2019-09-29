#include "mmemory.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>  // TODO: remove it after debug
#include <assert.h>

// segm_end returns VA of last allocated block of the segment x.
#define segm_end(x) (x->va + x->l)

// segment_pa returns physical address of the segment x in mmem.
#define segment_pa(x) (mmem.b + (size_t)x->va * sizeof(VA))

// ocup_space returns occupied space in memory mem.
#define ocup_space(mem)((size_t)mem.te->va + mem.te->l)

// free_spaceof returns amount of free space in the memory mem.
#define free_space(mem) (mem.s - ocup_space(mmem))

// ST is a stack that represents segments table.
typedef struct ST
{
	VA va;          // Segment VA. 
    size_t l;       // Length of the segment.
	struct ST* p;   // Physical address of prev segment's VA.
} ST;

// MEMORY represents virtual address space.
typedef struct
{
	VA b;		    // Points to the first block of memory.
	size_t s;	    // Number of blocks in memory.
    ST* te;         // Points to the ending of segments table stack.
} MEMORY;

static MEMORY mmem; // Virtual address space instance.

// request_memory allocates sz bytes of memory for provided pointer.
size_t request_memory (size_t sz)
{
    //int tptr = sbrk(0);
    size_t reqmem = sbrk(sz);
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
    if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
    {
        return NULL;
    }

    ST* segm = mmem.te;
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
	if (szBlock > free_space(mmem))
	{
		return -2;
	}

    ST* segm = mmem.te;

    size_t addr = request_memory(sizeof(ST*));
    if (addr == -1)
    {
        return 1;
    }
    mmem.te = (ST*)addr;

    addr = request_memory(szBlock * sizeof(VA));
    if (addr == -1)
    {
        return 1;
    }
    *ptr = (VA)addr;

    mmem.te->va = segm->va + segm->l;
    mmem.te->l = szBlock;
    mmem.te->p = segm;

    return 0;
}

int _free (VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
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
	if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
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
	if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
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

    size_t addr = request_memory(n * szPage);
    if (addr == -1)
    {
        return 1;
    }
    mmem.b = (VA)addr;
    
    mmem.te = (ST*)request_memory(sizeof(ST*));
    mmem.te->va = 0;
    mmem.te->l = 0;
    mmem.s = n * szPage;
    //assert(mmem.s > ocup_space(mmem));
	
    return 0;
}

