#include <stdio.h>
#include "mmemory_test.h"
#include "mmemory.h"

#define PASS "PASS"
#define FAIL_UNEXP "FAIL! Unexpected error"
#define FAIL_WR_INP "FAIL! Wrong input parameters"

void _init_test ()
{
    printf("_init test:\n");
    
    int code = _init(1, 1);

    char* result;
    switch (code)
    { 
        case 0:
            result = PASS;
            break;
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    printf("-- %s\n", result);
}

void _malloc_test ()
{
    printf("_malloc test:\n");

    int code = _init(1, 1);

    char* result;
    switch (code)
    { 
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    if (result != NULL)
    {
        printf("-- (_init)%s\n", result);
        return;
    }

    char* ptr;
    code = _malloc(&ptr, 3);

    switch (code)
    { 
        case 0:
            result = PASS;
            break;
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    printf("-- %s\n", result);
}

/*
void _free_test ()
{
    printf("_free test:\n");
    
    int code = _free();
    
    char* result;
    switch (code)
    { 
        case 0:
            result = PASS;
            break;
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    printf("-- %s\n", result);
}

void _read_test ()
{
    printf("_read test:\n");
    
    int code = _read();
    
    char* result;
    switch (code)
    { 
        case 0:
            result = PASS;
            break;
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    printf("-- %s\n", result);

}

void _write_test ()
{
    printf("_write test:\n");
    
    int code = _write();
    
    char* result;
    switch (code)
    { 
        case 0:
            result = PASS;
            break;
        case 1:
            result = FAIL_UNEXP;
            break;
        case -1:
            result = FAIL_WR_INP;
    }
    printf("-- %s\n", result);
}
*/

int main (int argc, char** argv)
{
    _init_test();
    _malloc_test();
/*
    _free_test();
    _read_test();
    _write_test();
*/

	return 0;
}

