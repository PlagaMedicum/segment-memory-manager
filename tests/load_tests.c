#include <stdio.h>
#include "benchmark.h"
#include "../mmemory.h"
#include "load_tests.h"

// TODO: Fix init with calloc and fix SF in ?free?

// test_fragmentation tests the slowdown while freeing
// first segment in memory, increasing number and size of
// segments to reallocate whith defragmentation.
void test_fragmentation ()
{
    printf("\nfragmentation test:\n\
            \rSIZE\t|NUMBER\t|TIME\n");

    VA va;
    for (size_t s_size = 1; s_size <= 101; s_size += 10)
    {
        for (size_t s_number = 1; s_number <=101; s_number += 10)
        {
            _init(s_size, s_number);
            for (int i = 0; i < s_number; i++)
            {
                _malloc(&va, s_size);
            }

            _T_START;
            _free((VA)0);
            _T_STOP;

            printf("%ld\t|%ld\t|%f\n", s_size, s_number, _T_DIFF);
        }
    }
}

int main (int argc, char** argv)
{
    test_fragmentation();

    return 0;
}
