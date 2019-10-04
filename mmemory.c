#include "mmemory.h"
#include <stdlib.h>
#include <assert.h>

// ST is a queue that represents segments table.
typedef struct _segment_table
{
	VA va;          // Segment VA.
	struct _segment_table* n;   // Physical address of next segment's VA.
} ST;

// MEMORY represents virtual address space.
typedef struct
{
	size_t pa;		// Physical address of first block.
	size_t sz;	    // Number of blocks in memory.
    ST* fs;         // Points to the first segment in the table.
} MEMORY;

static MEMORY mmem; // Virtual address space instance.

// s_pa returns physical address of segment s.
size_t s_pa (const ST* s)
{
    assert(s != NULL);

    return mmem.pa + (size_t)s->va;
}

// last_s returns last valuable segment(next is not null) of the table.
// Note: Cannot make it static because o _free.
//
// Examples:
// fs->va == 0, fs->n == NULL                   --> fs
// fs->va == 0, fs->n != NULL, fs->n->n == NULL --> fs
ST* last_s()
{
    ST* s = mmem.fs;

    assert(s != NULL);

    if (s->n == NULL)
    {
        assert(s->va == 0);

        return s;
    }

    if (s->n->va <= s->va)
    {
        printf("\nsnva = %ld sva = %ld\n", (size_t)s->n->va, (size_t)s->va);
        assert(s->n->va > s->va);
    }

    while (s->n->n != NULL)
    {
        assert(s->n->va > s->va);

        s = s->n;
    }
    return s;
}

// s_end returns VA of last allocated block of the segment s.
// s->n == NULL --> s_end = 0;
// s->n != NULL --> s_end = svan - 1,
// svan -- starting va of next segment.
//
// Examples:
// va == 0, n == NULL  --> s_end = 0
// va == 0, n->va == 1 --> s_end = 1 - 1 = 0
// va == 0, n->va == 3 --> s_end = 3 - 1 = 2
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

// s_len returns number of elements(bytes) in segment s.
// s_len = svan - svac,
// svan -- starting va of next segment,
// svac -- starting va of current segment.
//
// Examples:
// va == 0, n->va == 3 --> s_len = 3 - 0 = 3
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
        return RC_ERR_INPUT;
    }

    size_t addr = (size_t)malloc(sz);

    assert(addr != -1);

    return addr;
}

// ptrs writes physical address of the segment,
// which ptr belongs to, in s.
int ptrs (const VA ptr, ST** s)
{
    if (ptr == NULL)
    {
        return RC_ERR_INPUT;
    }
    if ((ptr < 0) || (ptr > (VA)s_end(last_s())))
    {
        return RC_ERR_SF;
    }

    ST* temp_s = mmem.fs;
    while (temp_s->n != NULL)
    {
        if (ptr < (VA)temp_s->n->va)
        {
            *s = temp_s;
            return RC_SUCCESS;
        }

        temp_s = temp_s->n;
    }

    return RC_ERR_U;
}

int _malloc (VA* ptr, size_t szBlock)
{
	if (szBlock > free_space())
	{
		return RC_ERR_SF;
	}

    size_t addr = rqmem(szBlock);

    *ptr = (VA)addr;
    if (ptr == NULL)
    {
        return RC_ERR_U;
    }

    ST* s = last_s();

    assert(s != NULL);

    while (s->n != NULL)
    {
        s = s->n;
    }

    addr = rqmem(sizeof(ST));
    s->n = (ST*)addr;
    s->n->va = (VA)(szBlock + 1);

    return RC_SUCCESS;
}

int _free (VA ptr)
{
    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    assert (s->n != NULL);

    size_t shift = s_len(s);
    while (s->n->n != NULL)
    {
        s = s->n;

        // Shifting va of the next segment.
        s->n->va = s->n->va - shift;

        // Shift back all elements of the segment and zero the old place.
        for (size_t el = mmem.pa + (size_t)s->va; el <= (size_t)(mmem.pa + s_end(s)); el++)
        {
            assert((VA)(el) != NULL);
            assert((VA)(el - shift) != NULL);

            *((VA)(el - shift)) = *((VA)el);
            *((VA)el) = 0;
        }
    }

    assert(s->n != NULL);

    // Shifting the ending of allocated address space.
    s->n->va = s->n->va - shift;

    assert(mmem.fs != NULL);

    // Udating connections in the addresses queue.
    ST* prev = mmem.fs;
    while ((prev != s) && (prev->n != s))
    {
        assert(prev != NULL);

        prev = prev->n;
    }
    if (prev == s)
    {
        prev->n = prev->n->n;
    }
    else
    {
        prev->n = s->n;
    }
    free(s);

    return RC_SUCCESS;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    if (szBuffer > s_len(s))
    {
        return RC_ERR_SF;
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

    return RC_SUCCESS;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    if (szBuffer > s_len(s))
    {
        return RC_ERR_SF;
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

    return RC_SUCCESS;
}

int s_init (int n, int szPage)
{
	if ((n < 1) || (szPage < 1))
	{
		return RC_ERR_INPUT;
	}

    size_t addr = rqmem(n * szPage);
    mmem.pa = addr;
    
    addr = rqmem(sizeof(ST));
    mmem.fs = (ST*)addr;
    mmem.fs->va = 0;

    assert(mmem.fs->n == NULL);

    mmem.sz = n * szPage;
	
    return RC_SUCCESS;
}

