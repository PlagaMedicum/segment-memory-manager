#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "benchmark.h"
#include "unit_tests.h"

#define SUCCESS "0(SUCCESS)"
#define FAIL_UNEXP "1(Unexpected error)"
#define FAIL_WR_INP "-1(Wrong input parameters)"
#define FAIL_SF "-2(Access beyoud the segment)"

// init_mmem creates new memory and segment table instances
// with specified parameters and calls make_mmem to update mmem.
// sz -- number of bytes in memory;
// buf -- string buffer contains data for initialising memory;
// szBuf -- size of buffer;
// count -- number of segments in the segment table;
// ... -- lengths of segments in the table.
MEMORY* init_mmem (size_t sz, const char* buf, size_t szBuf, size_t count, ...)
{
    MEMORY* mem = malloc(sizeof(MEMORY));
    mem->sz = sz;
    mem->pa = (size_t)malloc(mem->sz);

    // Copying data from buf
    for (int i = 0; i < szBuf; i++)
    {
        *((char*)(mem->pa + i)) = buf[i];
    }

    // Creating segment table
    va_list lengths;
    ST* st = malloc(sizeof(ST));
    VA va = 0;
    st->va = va;
    st->n = NULL;
    mem->fs = st;

    // Adding segments in the table
    va_start(lengths, count);

    ST* cur_s;
    cur_s = st;
    ST* next_s;
    for (int i = 0; i < count; i++)
    {
        next_s = malloc(sizeof(ST));
        va = (VA)(va + va_arg(lengths, size_t));
        next_s->va = va;
        next_s->n = NULL;

        cur_s->n = next_s;
        cur_s = cur_s->n;
    }

    va_end(lengths);
    
    make_mmem(mem);

    return mem;
}

// code_to_str converts provided code to string.
char* code_to_str (const int code)
{
    switch (code)
    {
        case RC_SUCCESS:
            return SUCCESS;
        case RC_ERR_U:
            return FAIL_UNEXP;
        case RC_ERR_INPUT:
            return FAIL_WR_INP;
        case RC_ERR_SF:
            return FAIL_SF;
    }
    return NULL;
}

// TC is a structure for test cases
typedef struct
{
    char* name;     // Name of the test case.
    int rc;         // Returned code.
    int exp_rc;     // Expected return code.
    int n;          // Number of pages.
    int szPage;     // Size of pages.
    size_t szBlock; // Size of segment.
    VA ptr;         // Segment address.
    char* pBuffer;  // Data buffer.
    size_t szBuffer;// Size of the buffer.
} TC;

void test_init ()
{
    printf("\n_init test:\n");
    
    TC tc[] = {
        {.name = "1 element", .n = 1, .szPage = 1},
        {.name = "10 elements", .n = 10, .szPage = 1},
        {.name = "100 elements", .n = 100, .szPage = 1},
        {.name = "n * szPage == 0", .exp_rc = RC_ERR_INPUT, .n = 0, .szPage = 1},
        {.name = "n * szPage < 0", .exp_rc = RC_ERR_INPUT, .n = -1, .szPage = 1},
    };


    for (int i = 0; i < sizeof(tc)/sizeof(TC); i++)
    {
        _T_START;
        tc[i].rc = _init(tc[i].n, tc[i].szPage);
        _T_STOP;
        if (tc[i].rc == tc[i].exp_rc)
        {
            printf("-- %s\n\
                  \r   PASS(%fsec)\n", 
                  tc[i].name, _T_DIFF);
            continue;
        }
        printf("-- %s\n\
              \r   FAIL!(%fsec)\n\
              \r   Return code is: %s\n", 
              tc[i].name, _T_DIFF, code_to_str(tc[i].rc));
    }
}

void test_malloc ()
{
    printf("\n_malloc test:\n");
 
    TC tc[] = {
        {.name = "1 byte", .szBlock = 1},
        {.name = "10 byte", .szBlock = 10},
        {.name = "100 byte", .szBlock = 100},
        {.name = "0 size", .exp_rc = RC_ERR_INPUT, .szBlock = 0},
        {.name = "Size out of range", .exp_rc = RC_ERR_SF, .szBlock = 999}
    };

    init_mmem(111, NULL, 0, 0);
    
    for (int i = 0; i < sizeof(tc)/sizeof(TC); i++)
    {
        _T_START;
        tc[i].rc = _malloc(&tc[i].ptr, tc[i].szBlock);
        _T_STOP;
        if ((tc[i].rc == RC_SUCCESS) && (tc[i].ptr == NULL))
        {
            tc[i].rc = RC_ERR_U;
        }   
        if (tc[i].rc == tc[i].exp_rc)
        {
            printf("-- %s\n\
                  \r   PASS(%fsec)\n", 
                  tc[i].name, _T_DIFF);
            continue;
        }
        printf("-- %s\n\
              \r   FAIL!(%fsec)\n\
              \r   Return code is: %s\n", 
              tc[i].name, _T_DIFF, code_to_str(tc[i].rc));
    }
}

void test_write ()
{
    printf("\n_write test:\n");
    
    TC tc[] = {
        {.name = "\"Hi\"", .ptr = (VA)0, .pBuffer = (char*)"Hi", .szBuffer = 3},
        {.name = "NULL buffer", .exp_rc = RC_ERR_INPUT, .ptr = (VA)0, .pBuffer = (char*)NULL, .szBuffer = 1},
        {.name = "0 size buffer", .exp_rc = RC_ERR_INPUT, .ptr = (VA)0, .pBuffer = (char*)1, .szBuffer = 0},
        {.name = "Negative ptr", .exp_rc = RC_ERR_SF, .ptr = (VA)-1, .pBuffer = (char*)1, .szBuffer = 1},
        {.name = "Ptr out of range", .exp_rc = RC_ERR_SF, .ptr = (VA)999, .pBuffer = (char*)1, .szBuffer = 1}
    };

    init_mmem(3, NULL, 0, 1, 3);

    for (int i = 0; i < sizeof(tc)/sizeof(TC); i++)
    {
        _T_START;
        tc[i].rc = _write(tc[i].ptr, tc[i].pBuffer, tc[i].szBuffer);
        _T_STOP;
        if (tc[i].rc == tc[i].exp_rc)
        {
            printf("-- %s\n\
                  \r   PASS(%fsec)\n", 
                  tc[i].name, _T_DIFF);
            continue;
        }
        printf("-- %s\n\
              \r   FAIL!(%fsec)\n\
              \r   Return code is: %s\n", 
              tc[i].name, _T_DIFF, code_to_str(tc[i].rc));
    }
}

void test_read ()
{
    printf("\n_read test:\n");
    
    TC tc[] = {
        {.name = "\"Hi\"", .ptr = (VA)0, .szBuffer = 3},
        {.name = "0 size buffer", .exp_rc = RC_ERR_INPUT, .ptr = (VA)0, .szBuffer = 0},
        {.name = "Negative ptr", .exp_rc = RC_ERR_SF, .ptr = (VA)-1, .szBuffer = 1},
        {.name = "Ptr out of range", .exp_rc = RC_ERR_SF, .ptr = (VA)999, .szBuffer = 1}
    };

    char buf[] = "Hi";
    const size_t len = strlen(buf) + 1;

    init_mmem(len, buf, sizeof(buf), 1, len + 1);
    
    for (int i = 0; i < sizeof(tc)/sizeof(TC); i++)
    {
        tc[i].pBuffer = malloc(tc[i].szBuffer);

        _T_START;
        tc[i].rc = _read(tc[i].ptr, tc[i].pBuffer, tc[i].szBuffer);
        _T_STOP;
        if ((tc[i].rc == RC_SUCCESS) && (strcmp(buf, tc[i].pBuffer)))
        {
            tc[i].rc = RC_ERR_U;
        }
        if (tc[i].rc == tc[i].exp_rc)
        {
            printf("-- %s\n\
                  \r   PASS(%fsec)\n", 
                  tc[i].name, _T_DIFF);
            continue;
        }
        printf("-- %s\n\
              \r   FAIL!(%fsec)\n\
              \r   Return code is: %s\n", 
              tc[i].name, _T_DIFF, code_to_str(tc[i].rc));
    }
}

void test_free ()
{
    printf("\n_free test:\n");
    
    TC tc[] = {
        {.name = "\"Hi\"", .ptr = (VA)0},
        {.name = "Negative ptr", .exp_rc = RC_ERR_SF, .ptr = (VA)-1},
        {.name = "Ptr out of range", .exp_rc = RC_ERR_SF, .ptr = (VA)999}
    };

    char buf[] = "Hi";
    const size_t len = strlen(buf) + 1;

    MEMORY* mem = init_mmem(len, buf, sizeof(buf), 3, len + 1, 2, 3);
   
    char* rbuf = malloc(sizeof(buf));
    for (int i = 0; i < sizeof(tc)/sizeof(TC); i++)
    {
        _T_START;
        tc[i].rc = _free(tc[i].ptr);
        _T_STOP;

        for (int i = 0; i < sizeof(buf); i++)
        {
            rbuf[i] = *((char*)(mem->pa + i));
        }
        if ((tc[i].rc == RC_SUCCESS) && (buf == rbuf))
        {
            tc[i].rc = RC_ERR_U;
        }

        if (tc[i].rc == tc[i].exp_rc)
        {
            printf("-- %s\n\
                  \r   PASS(%fsec)\n", 
                  tc[i].name, _T_DIFF);
            continue;
        }
        printf("-- %s\n\
              \r   FAIL!(%fsec)\n\
              \r   Return code is: %s\n", 
              tc[i].name, _T_DIFF, code_to_str(tc[i].rc));
    }
 
    free(rbuf);
}

int main (int argc, char** argv)
{
    test_init();
    test_malloc();
    test_write();
    test_read();
    test_free();

    return 0;
}

