#include "mmemory.h"
#include <stdbool.h>
#include <stdlib.h>

typedef VA* SEGMENT;

typedef struct 
{
	bool is_present;	// Checks if segment is present in memory
	VA beginning;		// points to beginning of segment
	size_t length;		// Number of elements in table
} segment_table;

typedef struct
{
	int d; // Address shift.
	int s; // Segment index.
} segment_link;

segment_table s_table;  // Segment table instance

// get_ending returns address of last element
// in last segment of s_table instance.
VA get_ending ()
{
    return s_table.beginning + s_table.length * sizeof(VA);
}

int _malloc (VA ptr, size_t szBlock)
{
	if ((s_table.beginning > ptr) && (ptr > get_ending()) && (szBlock < 1))  // TODO
	{
		return -1;
	}

    return 0;
}

int _free (VA ptr)
{
    if ((s_table.beginning > ptr) && (ptr > get_ending()))
    {
        return -1;
    }

    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((s_table.beginning > ptr) && (ptr > get_ending()) && (szBuffer < 1)) // TODO
	{
		return -1;
	}

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((s_table.beginning > ptr) && (ptr > get_ending()) && (szBuffer < 1)) // TODO
	{
		return -1;
	}

    return 0;
}

int s_init (int n, int sz)
{
	if ((n < 1) || (sz < 1))
	{
		return -1;
	}

	s_table.beginning = malloc(n * sz * sizeof(VA));
	s_table.length = n;

	s_table.is_present = 1;
	return 0;
}

