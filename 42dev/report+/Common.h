#ifndef REPORT_PLUS_COMMON_H
#define REPORT_PLUS_COMMON_H

#include "42.h"
#include <stdint.h>

#define TRUE 1
#define FALSE 0

#ifndef BOOL
    typedef int BOOL;
#endif

// время с 00:00 01.01.1970 до 12:00 01.01.2000
#define SEC_2000_1970 (946728000)
long CombTransDec(long a, long b, long N);
typedef enum {
    scGsConnect = 1,
    scScConnect = 2
} connectType;
long findCombinationNum(long Iobj1, long Iobj2, connectType type);

#ifdef _CJSON_PLUS_
#include "cJSON.h"
void outputConsolFile(cJSON * array, const char * arrayName);
#endif

#endif // REPORT_PLUS_COMMON_H
