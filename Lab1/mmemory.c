#include "mmemory.h"
#include <unistd.h>
#include <stdio.h>  // TODO: remove it after debug

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
    size_t i;       // Segment index.
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

// rqmem allocates sz bytes of memory for provided pointer.
size_t rqmem (size_t sz)
{
    size_t reqmem = sbrk(sz);
    if (reqmem == -1)
    {
        return -1;
    }

    return reqmem;
}

// ptrs returns physical adress of the segment
// which ptr belongs to.
ST* ptrs (VA ptr)
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

    ST* prev = mmem.te;

    size_t addr = rqmem(sizeof(ST*));
    if (addr == -1)
    {
        return 1;
    }
    mmem.te = (ST*)addr;

    addr = rqmem(szBlock * sizeof(VA));
    if (addr == -1)
    {
        return 1;
    }
    *ptr = (VA)addr;
    
    mmem.te->i = prev->i + 1;
    mmem.te->va = prev->va + prev->l;
    mmem.te->l = szBlock;
    mmem.te->p = prev;

    return 0;
}

int _free (VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
    {
        return -1;
    }

    ST* s = ptrs(ptr);
    if (s == NULL)
    {
        return 1;
    }

    // TODO: Segment shifting and freeing
    
    for(size_t el = (size_t)mmem.b + (size_t)s->va + s->l; el < ocup_space(mmem); el++)
    {
        // need to shift va's
        *((VA)el - s->l) = *((VA)el);
        *((VA)el) = 0;
    }

    ST* next;
    if(mmem.te->i != s->i)

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > (VA)segm_end(mmem.te)))
	{
		return -1;
	}
    
    ST* segm = ptrs(ptr);
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

    ST* segm = ptrs(ptr);
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

    size_t addr = rqmem(n * szPage);
    if (addr == -1)
    {
        return 1;
    }
    mmem.b = (VA)addr;
    
    mmem.te = (ST*)rqmem(sizeof(ST*));
    mmem.te->i = 0;
    mmem.te->va = 0;
    mmem.te->l = 0;
    mmem.s = n * szPage;
	
    return 0;
}

