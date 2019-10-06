#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "mmemory_test.h"

#define PASS "PASS"
#define FAIL_UNEXP "FAIL! Unexpected error"
#define FAIL_WR_INP "FAIL! Wrong input parameters"
#define FAIL_SF "FAIL! Access beyoud the segment"

#define _T_START clock_t t_start = clock(), t_stop;
#define _T_STOP t_stop = clock();
#define _T_DIFF (double)(t_stop - t_start) / CLOCKS_PER_SEC

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

// TODO: Rewrite tests with prepared structures for virtual address space.
// TODO: Prepare load tests. Analyze problems of model, describe it and
// choose required variables for creating graphics.

void test_init ()
{
    printf("\n_init test:\n");
    
    _T_START

    int code[] = {_init(1, 1), _init(3, 3)};
    for (int i = 0; i < sizeof(code)/sizeof(int); i++)
    {
        _T_STOP
        printf("-- %s(%fsec)\n", code_to_str(code[i]), _T_DIFF);
    }
}

void test_malloc ()
{
    printf("\n_malloc test:\n");
     
    _T_START
    
    

    int code = _init(18, 1);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_init)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char* ptr[3];
    int codes[] = {_malloc(&ptr[0], 3), _malloc(&ptr[1], 6), _malloc(&ptr[2], 9)};
    for (int i = 0; i < sizeof(codes)/sizeof(int); i++)
    {
        if (ptr[i] == NULL)
        {
            _T_STOP
            printf("-- (_malloc)FAIL! Pointer is NULL(%fsec)\n", _T_DIFF);
            return;
        }
        _T_STOP
        printf("-- %s(%fsec)\n", code_to_str(code), _T_DIFF);
    }
}

void test_write ()
{
    printf("\n_write test:\n");
     
    _T_START

    const char* buf = "Hi";
    const int len = strlen(buf) + 1;
    const size_t va = 1;
    
    int code = _init(len, 1);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_init)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char* ptr;
    code = _malloc(&ptr, len);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_malloc)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    if (ptr == NULL)
    {
        _T_STOP
        printf("-- (_malloc)FAIL! Pointer is NULL(%fsec)\n", _T_DIFF);
        return;
    }
    
    code = _write((VA) va, &buf, len);
    _T_STOP
    printf("-- %s(%fsec)\n", code_to_str(code), _T_DIFF);
}

void test_read ()
{
    printf("\n_read test:\n");
     
    _T_START

    char buf[] = "Hi";

    assert(*(buf + 3) == '\0');

    const int len = strlen(buf) + 1;
    const size_t va = 1;

    int code = _init(len, 1);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_init)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char* ptr;
    code = _malloc(&ptr, len);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_malloc)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    
    if (ptr == NULL)
    {
        _T_STOP
        printf("-- (_malloc)FAIL! Pointer is NULL(%fsec)\n", _T_DIFF);
        return;
    }

    code = _write((VA) va, &buf, len);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_write)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char rbuf[3];
    code = _read((VA) va, &rbuf, len);
    
    assert(*(rbuf + 3) == '\0');

    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_read)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    
    if (strcmp(buf, rbuf))
    {
        _T_STOP
        printf("-- (_read)FAIL!(%fsec)\n\
                \r\tExpected: %s\n\
                \r\tGot:      %s\n", 
                _T_DIFF, buf, rbuf);
    }

    _T_STOP
    printf("-- %s(%fsec)\n", PASS, _T_DIFF);
}

void test_free ()
{
    printf("\n_free test:\n");
 
    _T_START

    const char* buf = "Hi";
    const int len = strlen(buf) + 1;
    const size_t va = 1;

    int code = _init(len, 1);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_init)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
 
    char* ptr;
    code = _malloc(&ptr, len);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_malloc)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    if (ptr == NULL)
    {
        _T_STOP
        printf("-- (_malloc)FAIL! Pointer is NULL(%fsec)\n", _T_DIFF);
        return;
    }

    code = _write((VA) va, &buf, len);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_write)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    code = _free((VA) va);
    if (code != RC_SUCCESS)
    {
        _T_STOP
        printf("-- (_free)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }

    char * rbuf;
    code = _read((VA) va, &rbuf, len);
    if ((code != RC_SUCCESS) && (code != RC_ERR_SF))
    {
        _T_STOP
        printf("-- (_read)%s(%fsec)\n", code_to_str(code), _T_DIFF);
        return;
    }
    if (buf == rbuf)
    {
        _T_STOP
        printf("-- (_read)FAIL! Reading from freed memory succeeded(%fsec)\n",
                _T_DIFF);
        return;
    }

    _T_STOP
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

