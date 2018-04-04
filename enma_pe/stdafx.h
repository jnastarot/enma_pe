#pragma once

#define WIN32_LEAN_AND_MEAN   


#include <Windows.h>
//#include <winternl.h>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <time.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <iostream>
#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

#include <cstdint>

using namespace std;
#define ALIGN_UP(a, b) (((a + b - 1) / b) * b)


#include "enma_pe.h"


