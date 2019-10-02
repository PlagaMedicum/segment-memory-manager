#include "mmemory.h"
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
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
size_t s_pa (const ST* s)
{
    assert(s != NULL);

    return mmem.pa + (size_t)s->va;
}

// last_s updates last valuable segment(next is not null) of the table.
//
// fs->va = 0; fs->n = NULL                   --> fs
// fs->va = 0; fs->n != NULL; fs->n->n = NULL --> fs
//
ST* last_s()
{
    ST* s = mmem.fs;

    assert(s != NULL);

    if (s->n == NULL)
    {
        assert(s->va == 0);

        return s;
    }

    assert(s->n->va > s->va);

    while (s->n->n != NULL)
    {
        assert(s->n->va > s->va);

        s = s->n;
    }
    return s;
}

// s_end returns VA of last allocated block of the segment s.
//
// va = 0; n = NULL  --> s_end = 0
// va = 0; n->va = 1 --> s_end = 1 - 1 = 0
// va = 0; n->va = 3 --> s_end = 3 - 1 = 2
//
size_t s_end (const ST* s)
{
    assert(s != NULL);

    if (s->n == NULL)
    {
        assert(s->va == 0);

        return 0;
    }

    assert(s->n != NULL);
    assert(s->n->va > 0);

    return (size_t)s->n->va - 1;
}

// free_spaceof returns amount of free space in the memory.
#define free_space() (mmem.sz - s_end(last_s()))

// s_len returns number of elements in segment s.
size_t s_len (const ST* s)
{
    assert(s != NULL);

    size_t end = s_end(s);
    if (end == 0)
    {
        return 0;
    }
    return (end + 1) - (size_t)s->va;
}

// rqmem allocates sz bytes of memory for provided pointer.
size_t rqmem (const size_t sz)
{
    if (sz < 1)
    {
        return -1;
    }

    size_t addr = sbrk(sz);

    assert(addr != -1);

    return addr;
}

// ptrs returns physical adress of the segment
// which ptr belongs to.
ST* ptrs (const VA ptr)
{
    if ((ptr == NULL) || (ptr < 0) || (ptr > (VA)s_end(last_s())))
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
        return addr;
    }

    *ptr = (VA)addr;
    if (ptr == NULL)
    {
        return 1;
    }

    ST* s = last_s();

    assert(s != NULL);

    while (s->n != NULL)
    {
        s = s->n;
    }

    addr = rqmem(sizeof(ST));
    if (addr == -1)
    {
        return addr;
    }
    s->n = (ST*)addr;
    s->n->va = (VA)(szBlock + 1);

    return 0;
}

int _free (VA ptr)
{
    ST* s = ptrs(ptr);
    if (s == NULL)
    {
        return -1;
    }
    
    ST* freeing_s = s;

    size_t shift;
    while (s->n != NULL)
    {
        shift = s_len(s);

        s->n->va = s->va;

        for (size_t el = mmem.pa + (size_t)s->va; el < mmem.pa + s_end(s); el++)
        {
            assert((VA)el != NULL);

            *((VA)el) = 0;
        }

        s = s->n;

        for (size_t el = mmem.pa + (size_t)s->va; el < mmem.pa + s_end(s); el++)
        {
            assert((VA)(el - shift) != NULL);

            *((VA)(el - shift)) = *((VA)el);
        }
    }

    assert(mmem.fs != NULL);

    ST* prev = mmem.fs;
    while (prev->n != freeing_s)
    {
        prev = prev->n;
        
        assert(prev != NULL);
    }
    
    prev->n = freeing_s->n;
    free(freeing_s);

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
    ST* s = ptrs(ptr);
    if (s == NULL)
    {
        return -1;
    }

    if (szBuffer > s_len(s))
    {
        return -2;
    }

    VA buf_el;
    VA s_el;
    for (int i = 0; i < szBuffer; i++)
    {
        buf_el = (VA)(pBuffer + i);
        s_el = (VA)(s_pa(s) + i);

        assert(s_el != NULL);

        *buf_el = *s_el;
    }

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
    ST* s = ptrs(ptr);
    if (s == NULL)
    {
        return -1;
    }

    if (szBuffer > s_len(s))
    {
        return -2;
    }

    VA buf_el;
    VA s_el;
    for (int i = 0; i < szBuffer; i++)
    {
        buf_el = (VA)(pBuffer + i);
        s_el = (VA)(s_pa(s) + i);

        assert(buf_el != NULL);

        *s_el = *buf_el;
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
        return addr;
    }
    mmem.pa = addr;
    
    addr = rqmem(sizeof(ST));
    if (addr == -1)
    {
        return addr;
    }
    mmem.fs = (ST*)addr;
    mmem.fs->va = 0;

    assert(mmem.fs->n == NULL);

    mmem.sz = n * szPage + 1;
	
    return 0;
}

