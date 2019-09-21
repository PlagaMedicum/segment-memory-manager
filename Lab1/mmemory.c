#include "mmemory.h"
#include <stdbool.h>
#include <stdlib.h>

// S_TABLE is a stack that represents segments table
typedef struct
{
	size_t va;              // Segment VA.
	struct S_TABLE* p;      // Physical address of prev segment's VA.
	// bool is_p;              // Checks if segment is present in memory.
} S_TABLE;

// MEMORY represents virtual memory
typedef struct 
{
	VA b;		    // Points to the first block of memory.
	size_t s;	    // Number of bytes in memory.
    S_TABLE* te;    // Points to the ending of segments table stack.
} MEMORY;

MEMORY vmem;         // Virtual memory instance.

// get_free returns amount of free space in the memory.
size_t get_free ()
{
    return vmem.s - vmem.te->va * sizeof(VA);
}

int _malloc (VA* ptr, size_t szBlock)
{
	if (szBlock > get_free())
	{
		return -1;
	}

    S_TABLE* tmp_t = vmem.te;
    vmem.te = malloc(sizeof(S_TABLE));
    vmem.te->va = tmp_t->va + szBlock;
    vmem.te->p = tmp_t;

    ptr = malloc(szBlock * sizeof(VA));

    return 0;
}

int _free (VA ptr)
{
    if ((1))    // TODO
    {
        return -1;
    }

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((1))    // TODO
	{
		return -1;
	}

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((1))    // TODO
	{
		return -1;
	}

    return 0;
}

int s_init (int n, int szPage)
{
	if ((n < 1) || (szPage < 1))
	{
		return -1;
	}

	vmem.b = malloc(n * szPage * sizeof(VA));
    vmem.s = n * szPage;

    S_TABLE s_table;
    s_table.va = 0;
    vmem.tb = &s_table;

	return 0;
}

