#include "mmemory.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>  // TODO: remove it after debug

// ST is a queue that represents segments table.
typedef struct ST
{
	VA va;          // Segment VA.
	struct ST* n;   // Physical address of next segment's VA.
} ST;

// MEMORY represents virtual address space.
typedef struct
{
	size_t pa;		// Physical address of first block.
	size_t sz;	    // Number of blocks in memory.
    ST* fs;         // Points to the first segment in the table.
} MEMORY;

static MEMORY mmem; // Virtual address space instance.

// s_pa returns physical address of the segment s in mmem.
#define s_pa(s) (mmem.pa + (size_t)s->va)

// last_s updates last valuable segment(next is not null) of the table.
ST* last_s()
{
    ST* s = mmem.fs;
    if(s->n == NULL)
    {
        return s;
    }
    while(s->n->n != NULL)
    {
        s = s->n;
    }
    return s;
}

// s_end returns VA of last allocated block of the segment s.
size_t s_end (const ST* s)
{
    if(s->n == NULL)
    {
        return 0;
    }
    return (size_t)s->n->va - 1;
}

// free_spaceof returns amount of free space in the memory.
#define free_space() (mmem.sz - s_end(last_s()))

// s_len returns number of elements in segment s.
size_t s_len (const ST* s)
{
    size_t end = s_end(s);
    if(end == 0)
    {
        return 0;
    }
    return (end + 1) - (size_t)s->va;
}

// rqmem allocates sz bytes of memory for provided pointer.
size_t rqmem (const size_t sz)
{
    size_t addr = sbrk(sz);
    if (addr == -1)
    {
        return -1;
    }

    return addr;
}

// ptrs returns physical adress of the segment
// which ptr belongs to.
ST* ptrs (const VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)s_end(last_s())))
    {
        return NULL;
    }

    ST* s = mmem.fs;
    while (s->n != NULL)
    {
        if (ptr > (VA)s->va)
        {
            return s;
        }

        s = s->n;
    }

    return NULL;
}

int _malloc (VA* ptr, size_t szBlock)
{
	if (szBlock > free_space())
	{
		return -2;
	}

    size_t addr = rqmem(szBlock);
    if (addr == -1)
    {
        return 1;
    }
    *ptr = (VA)addr;

    ST* s = last_s();
    while(s->n != NULL)
    {
        s = s->n;
    }
    s->n = (ST*)rqmem(sizeof(ST));
    s->n->va = (VA)(szBlock + 1);

    return 0;
}

int _free (VA ptr)
{
    if ((ptr < 0) || (ptr > (VA)s_end(last_s())))
    {
        return -1;
    }

    ST* s = ptrs(ptr);
    if (s == NULL)
    {
        return 1;
    }
    
    ST* fr_s = s;

    size_t shift;
    while(s->n != NULL)
    {
        shift = s_len(s);
        s->n->va = s->va;
        for(size_t el = mmem.pa + (size_t)s->va; el < mmem.pa + s_end(s); el++)
        {
            *((VA)el) = 0;
        }
        s = s->n;
        for(size_t el = mmem.pa + (size_t)s->va; el < mmem.pa + s_end(s); el++)
        {
            *((VA)(el - shift)) = *((VA)el);
        }
    }

    ST* prev = mmem.fs;
    while(prev->n != fr_s)
    {
        prev = prev->n;
    }
    
    prev->n = fr_s->n;
    free(fr_s);

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > (VA)s_end(last_s())))
	{
		return -1;
	}
    
    ST* s = ptrs(ptr);
    if (szBuffer > s_len(s))
    {
        return -2;
    }
    if (s == NULL)
    {
        return 1;
    }

    for (int i = 0; i < szBuffer; i++)
    {
        *((VA*)(pBuffer + i)) = *((VA*)(s_pa(s) + i));
    }

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((ptr < 0) || (ptr > (VA)s_end(last_s())))   // TODO: Segmentation here
    {
		return -1;
	}

    ST* s = ptrs(ptr);
    if (szBuffer > s_len(s))
    {
        return -2;
    }
    if (s == NULL)
    {
        return 1;
    }

    for (int i = 0; i < szBuffer; i++)
    {
        *((VA*)(s_pa(s) + i)) = *((VA*)(pBuffer + i));
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
    mmem.pa = addr;
    
    mmem.fs = (ST*)rqmem(sizeof(ST*));
    mmem.fs->va = 0;
    mmem.sz = n * szPage + 1;
	
    return 0;
}

