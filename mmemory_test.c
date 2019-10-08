#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "mmemory_test.h"

#define PASS "PASS"
#define FAIL_UNEXP "FAIL! Unexpected error"
#define FAIL_WR_INP "FAIL! Wrong input parameters"
#define FAIL_SF "FAIL! Access beyoud the segment"

#define _T_START clock_t t_start = clock(), t_stop
#define _T_STOP t_stop = clock()
#define _T_DIFF (double)(t_stop - t_start) / CLOCKS_PER_SEC

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
            return PASS;
        case RC_ERR_U:
            return FAIL_UNEXP;
        case RC_ERR_INPUT:
            return FAIL_WR_INP;
        case RC_ERR_SF:
            return FAIL_SF;
    }
    return NULL;
}

// TODO: Prepare load tests. Analyze problems of model, describe it and
// choose required variables for creating graphics.

void test_init ()
{
    printf("\n_init test:\n");
    
    _T_START;

    int code[] = {_init(1, 1), _init(10, 1), _init(100, 1)};
    for (int i = 0; i < sizeof(code)/sizeof(int); i++)
    {
        _T_STOP;
        printf("-- %s(%fsec)\n", code_to_str(code[i]), _T_DIFF);
    }
}

void test_malloc ()
{
    printf("\n_malloc test:\n");
     
    init_mmem(111, NULL, 0, 0);
    
    _T_START;

    char* ptr[3];
    int codes[] = {_malloc(&ptr[0], 1), _malloc(&ptr[1], 10), _malloc(&ptr[2], 100)};
    for (int i = 0; i < sizeof(codes)/sizeof(int); i++)
    {
        if (ptr[i] == NULL)
        {
            _T_STOP;
            printf("-- (_malloc)FAIL! Pointer is NULL(%fsec)\n", _T_DIFF);
            return;
        }
        _T_STOP;
        printf("-- %s(%fsec)\n", code_to_str(codes[i]), _T_DIFF);
    }
}

void test_write ()
{
    printf("\n_write test:\n");
     
    char buf[] = "Hi";
    const size_t len = strlen(buf) + 1;

    init_mmem(len, NULL, 0, 1, len + 1);

    _T_START;

    const size_t va = 0;
    int code = _write((VA) va, &buf, len);
    _T_STOP;
    printf("-- %s(%fsec)\n", code_to_str(code), _T_DIFF);
}

void test_read ()
{
    printf("\n_read test:\n");

    char buf[] = "Hi";
    const size_t len = strlen(buf) + 1;

    init_mmem(len, buf, sizeof(buf), 1, len + 1);
    
    _T_START;

    const size_t va = 0;

    char rbuf[3];
    int code = _read((VA) va, &rbuf, len);
    
    assert(*(rbuf + 3) == '\0');

    if (code != RC_SUCCESS)
    {
        _T_STOP;
        printf("-- (_read)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    
    if (strcmp(buf, rbuf))
    {
        _T_STOP;
        printf("-- (_read)FAIL!(%fsec)\n\
                \r\tExpected: %s\n\
                \r\tGot:      %s\n", 
                _T_DIFF, buf, rbuf);
    }

    _T_STOP;
    printf("-- %s(%fsec)\n", PASS, _T_DIFF);
}

void test_free ()
{
    printf("\n_free test:\n");

    char buf[] = "Hi";
    const size_t len = strlen(buf) + 1;

    MEMORY* mem = init_mmem(len, buf, sizeof(buf), 1, len + 1);
   
    _T_START;
 
    const size_t va = 0;
    int code = _free((VA) va);
    if (code != RC_SUCCESS)
    {
        _T_STOP;
        printf("-- (_free)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char* rbuf = malloc(sizeof(buf));
    for (int i = 0; i < sizeof(buf); i++)
    {
        rbuf[i] = *((char*)(mem->pa + i));
    }
    if (buf == rbuf)
    {
        _T_STOP;
        printf("-- (_read)FAIL! Data in the memory does'nt changed(%fsec)\n",
                _T_DIFF);
        free(rbuf);
        return;
    }
    free(rbuf);

    _T_STOP;
    printf("-- %s(%fsec)\n", PASS, _T_DIFF);
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

