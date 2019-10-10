#include <time.h>

#define _T_START clock_t t_start = clock(), t_stop
#define _T_STOP t_stop = clock()
#define _T_DIFF (double)(t_stop - t_start) / CLOCKS_PER_SEC

