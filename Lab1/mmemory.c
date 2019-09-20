#include "mmemory.h"
#include <stdbool.h>

struct segment_table
{
	bool is_present;	// Checks if segment is present in memory
	VA beginning;		// points to beginning of segment
	int length;		// Number of elements in table

	VA next;		// next segment
};

struct segment_link
{
	int d; // Address shift.
	int s; // Segment index.
};

SEGMENT_TABLE s_table;

int _malloc (VA* ptr, size_t szBlock)
{
	if ((szBlock < 1))
	{
		return -1;
	}

    return 0;
}

int _free (VA ptr)
{
    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((szBuffer < 1))
	{
		return -1;
	}

    return 0;
}

int _write (VA ptr, void* pBuffer, size_t szBuffer)
{
	if ((szBuffer < 1))
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

	s_table.beginning = (SEGMENT)malloc(n * sz * sizeof(VA));
	s_table.length = n;

	s_table.is_present = 1;
	return 0;
}

