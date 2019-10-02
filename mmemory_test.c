#include <stdio.h>
#include <string.h>
#include "mmemory_test.h"
#include "mmemory.h"

#define PASS "PASS"
#define FAIL_UNEXP "FAIL! Unexpected error"
#define FAIL_WR_INP "FAIL! Wrong input parameters"
#define FAIL_BOUND "FAIL! Access beyoud the segment"

// code_to_str converts provided code to string.
char* code_to_str (const int code)
{
    switch (code)
    { 
        case 0:
            return PASS;
        case 1:
            return FAIL_UNEXP;
        case -1:
            return FAIL_WR_INP;
        case -2:
            return FAIL_BOUND;
    }
    return NULL;
}

void _init_test ()
{
    printf("\n_init test:\n");
    
    int code[] = {_init(1, 1), _init(3, 3)};
    for (int i = 0; i < sizeof(code)/sizeof(int); i++)
    {
        printf("-- %s\n", code_to_str(code[i]));
    }
}

void _malloc_test ()
{
    printf("\n_malloc test:\n");
    
    int len = 3;

    int code = _init(len, 1);
    if (code != 0)
    {
        printf("-- (_init)%s\n", code_to_str(code));
        return;
    }

    char* ptr;
    code = _malloc(&ptr, len);
    if (ptr == NULL)
    {
        printf("-- (_malloc)FAIL! Pointer is NULL.");
        return;
    }
    printf("-- %s\n", code_to_str(code));
}

void _write_test ()
{
    printf("\n_write test:\n");
    
    const char* buf = "Hi";
    const int len = strlen(buf) + 1;
    const size_t va = 1;
    
    int code = _init(len, 1);
    if (code != 0)
    {
        printf("-- (_init)%s\n", code_to_str(code));
        return;
    }

    char* ptr;
    code = _malloc(&ptr, len);
    if (code != 0)
    {
        printf("-- (_malloc)%s\n", code_to_str(code));
        return;
    }
    if (ptr == NULL)
    {
        printf("-- (_malloc)FAIL! Pointer is NULL.");
        return;
    }
    
    code = _write((VA) va, &buf, len);
    printf("-- %s\n", code_to_str(code));
}

void _read_test ()
{
    printf("\n_read test:\n");
    
    const char* buf = "Hi";
    const int len = strlen(buf) + 1;
    const size_t va = 1;

    int code = _init(len, 1);
    if (code != 0)
    {
        printf("-- (_init)%s\n", code_to_str(code));
        return;
    }

    char* ptr;
    code = _malloc(&ptr, len);
    if (code != 0)
    {
        printf("-- (_malloc)%s\n", code_to_str(code));
        return;
    }
    if (ptr == NULL)
    {
        printf("-- (_malloc)FAIL! Pointer is NULL.");
        return;
    }

    code = _write((VA) va, &buf, len);
    if (code != 0)
    {
        printf("-- (_write)%s\n", code_to_str(code));
        return;
    }

    char* rbuf;
    code = _read((VA) va, &rbuf, len);
    if (code != 0)
    {
        printf("-- (_read)%s\n", code_to_str(code));
        return;
    }
    if (strcmp(buf, rbuf))
    {
        printf("-- (_read)FAIL!\n\
                \r\tExpected: %s\n\
                \r\tGot:      %s\n", 
                buf, rbuf);
    }

    printf("-- %s\n", PASS);
}

void _free_test ()
{
    printf("\n_free test:\n");

    const char* buf = "Hi";
    const int len = strlen(buf) + 1;
    const size_t va = 1;

    int code = _init(len, 1);
    if (code != 0)
    {
        printf("-- (_init)%s\n", code_to_str(code));
        return;
    }
 
    char* ptr;
    code = _malloc(&ptr, len);
    if (code != 0)
    {
        printf("-- (_malloc)%s\n", code_to_str(code));
        return;
    }
    if (ptr == NULL)
    {
        printf("-- (_malloc)FAIL! Pointer is NULL.");
        return;
    }

    code = _write((VA) va, &buf, len);
    if (code != 0)
    {
        printf("-- (_write)%s\n", code_to_str(code));
        return;
    }

    code = _free((VA) va);
    if (code != 0)
    {
        printf("-- (_free)%s\n", code_to_str(code));
        return;
    }
    
    char * rbuf;
    code = _read((VA) va, &rbuf, len);
    if (code != 0)
    {
        printf("-- (_read)%s\n", code_to_str(code));
        return;
    }
    if (buf == rbuf)
    {
        printf("-- (_read)FAIL! Reading from freed memory succeeded.");
        return;
    }

    printf("-- %s\n", PASS);
}

int main (int argc, char** argv)
{
    _init_test();
    _malloc_test();
    _write_test();
    _read_test();
    _free_test();

	return 0;
}

