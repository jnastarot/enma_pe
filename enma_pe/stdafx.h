#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include <list>
#include <math.h>
#include <map>
#include <set>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <stdarg.h> 
#include <iostream>
#include <iostream>

#include <locale.h>

using namespace std;

#ifndef ALIGN_UP
    #define ALIGN_UP(a, b) ((((a) + (b) - 1) / (b)) * (b))
#endif

#ifndef offsetof
    #define offsetof(s,m) (uint32_t)((size_t)&(((s*)0)->m))
#endif

#ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define INT8_SIZE   1
#define INT16_SIZE  2
#define INT32_SIZE  4
#define INT64_SIZE  8
#define CHAR_SIZE   1
#define WCHAR_SIZE  2

#ifndef _In_
    #define _In_
#endif
#ifndef _Inout_
    #define _Inout_
#endif
#ifndef _Out_opt_
    #define _Out_opt_
#endif
#ifndef _In_opt_
    #define _In_opt_
#endif
#ifndef _Inout_opt_
    #define _Inout_opt_
#endif
#ifndef _Out_
    #define _Out_
#endif

#include "enma_pe.h"

