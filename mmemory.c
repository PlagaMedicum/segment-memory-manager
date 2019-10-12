#include "mmemory.h"
#include "tests/unit_tests.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static MEMORY* mmem; // Virtual address space instance.

// free_st frees segment table.
void free_mmem ()
{
    if (mmem == NULL)
    {
        return;
    }
    if (mmem->fs == NULL)
    {
        return;
    }

    ST* cur = mmem->fs;
    ST* prev;
    while (cur->n != NULL)
    {
        prev = cur;
        cur = cur->n;

        free(prev);
    }
    free(cur);

    free(mmem);
}

void make_mmem (MEMORY* new_mem)
{
    free_mmem();
    mmem = new_mem;
}

// s_end returns address of last block of segment s.
#define s_end(s) (s->va + s->l)

// find_space returns free segment for provided length
ST* find_space (size_t s_len)
{
    assert(mmem->fs != NULL);
    
    if(mmem->fs->n == NULL)
    {
        return mmem->fs;
    }

    ST* s = mmem->fs;

    while (s->n != NULL)
    {
        if (s->n->va - s_end(s) >= s_len)
        {
            return s;
        }

        s = s->n;
    }

    if (mmem->sz - (size_t)s->va >= s_len)
    {
        return s;
    }

    return NULL;
}

int _malloc (VA* ptr, size_t szBlock)
{
    if ((szBlock < 1) || (szBlock > mmem->sz))
    {
        return RC_ERR_INPUT;
    }

    ST* s = find_space(szBlock);
    if (s == NULL)
    {
        return RC_ERR_U;
    }

    if ((s == mmem->fs) && (s->l == 0))
    {
        s->l = szBlock;

        *ptr = s->va;

        return RC_SUCCESS;
    }

    ST* next = s->n;
    s->n = calloc(1, sizeof(ST));
    s->n->va = s_end(s) + 1;
    s->n->l = szBlock;
    s->n->n = next;

    *ptr = s->va;
    
    return RC_SUCCESS;
}

// ptrs wrtes in s physical address of the segment,
// which ptr belongs to.
int ptrs (const VA ptr, ST** s)
{
    if ((ptr < 0) || (ptr > (VA)mmem->sz))
    {
        return RC_ERR_SF;
    }

    ST* temp_s = mmem->fs;
    while (temp_s->n != NULL)
    {
        if (ptr <= s_end(temp_s))
        {
            *s = temp_s;
            return RC_SUCCESS;
        }

        temp_s = temp_s->n;
    }

    return RC_ERR_U;
}

int _free (VA ptr)
{
    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    ST* prev = mmem->fs;
    if (prev == s)
    {
        free(s);

        return RC_SUCCESS;
    }

    while (prev->n != s)
    {
        prev = prev->n;
    }
    prev->n = s->n;

    free(s);

    return RC_SUCCESS;
}

// s_pa returns physical address of segment s.
size_t s_pa (const ST* s)
{
    assert(s != NULL);

    return mmem->pa + (size_t)s->va;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
    if ((pBuffer == NULL) || (szBuffer < 1))
    {
        return RC_ERR_INPUT;
    }

    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    if (szBuffer > s->l)
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
        assert(buf_el != NULL);

        *buf_el = *s_el;
    }

    return RC_SUCCESS;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
    if ((pBuffer == NULL) || (szBuffer < 1))
    {
        return RC_ERR_INPUT;
    }

    ST* s;
    int code = ptrs(ptr, &s);
    if (code != RC_SUCCESS)
    {
        return code;
    }

    if (szBuffer > s->l)
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
        assert(buf_el != NULL);

        *s_el = *buf_el;
    }

    return RC_SUCCESS;
}

int _init (int n, int szPage)
{
	if ((n < 1) || (szPage < 1))
	{
		return RC_ERR_INPUT;
	}

    free_mmem();

    mmem = calloc(1, sizeof(MEMORY));
    mmem->sz = n * szPage;
    mmem->pa = (size_t)calloc(mmem->sz, 1);
    mmem->fs = calloc(1, sizeof(ST));

    return RC_SUCCESS;
}

