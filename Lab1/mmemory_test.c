#include <stdio.h>
#include "mmemory_test.h"
#include "mmemory.h"

#define PASS "PASS"
#define FAIL_UNEXP "FAIL! Unexpected error"
#define FAIL_WR_INP "FAIL! Wrong input parameters"

void code_to_str (int code, char** str)
{
    switch (code)
    { 
        case 0:
            *str = PASS;
            break;
        case 1:
            *str = FAIL_UNEXP;
            break;
        case -1:
            *str = FAIL_WR_INP;
    }
}

void _init_test ()
{
    printf("_init test:\n");
    
    int code[2] = {_init(1, 1), _init(3, 3)};
    char* res;
    for (int i = 0; i < sizeof(code)/sizeof(int); i++)
    {
        code_to_str(code[i], &res);
        printf("-- %s\n", res);
    }
}

void _malloc_test ()
{
    printf("_malloc test:\n");

    int code = _init(1, 1);
    char* res;
    if (code != 0)
    {
        code_to_str(code, &res);
        printf("-- (_init)%s\n", res);
        return;
    }

    char* ptr;
    code = _malloc(&ptr, 3);
    code_to_str(code, &res);
    printf("-- %s\n", res);
}

void _write_test ()
{
    printf("_write test:\n");
    
    // TODO: malloc
    int code = _init(1, 1);
    char* res;
    if (code != 0)
    {
        code_to_str(code, &res);
        printf("-- (_init)%s\n", res);
        return;
    }


    char* buf = "Anarchy in UK!";
    code = _write((VA)1, &buf, sizeof(char) * sizeof(buf));
    code_to_str(code, &res);
    printf("-- %s\n", res);
}

void _read_test ()
{
    printf("_read test:\n");
    
    // TODO: malloc, write
    int code = _init(1, 1);
    char* res;
    if (code != 0)
    {
        code_to_str(code, &res);
        printf("-- (_init)%s\n", res);
        return;
    }

    char* buf;
    code = _read((VA)1, &buf, sizeof(char) * 3);
    code_to_str(code, &res);
    printf("-- %s\n", res);

}

void _free_test ()
{
    printf("_free test:\n");

    // TODO: malloc, write
    int code = _init(1, 1);
    char* res;
    if (code != 0)
    {
        code_to_str(code, &res);
        printf("-- (_init)%s\n", res);
        return;
    }

    code = _free((VA)1);
    code_to_str(code, &res);
    printf("-- %s\n", res);
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

