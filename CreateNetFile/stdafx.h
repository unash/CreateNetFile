// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
// #pragma once

#include "targetver.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>

#if defined(__APPLE__)
#define PLATFORM_MACOSX
#elif defined(_WIN32) || define(_WIN64)
#define PLATFORM_WINNT
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif

#if defined(PLATFORM_MACOSX)

#include <unistd.h>
typedef char _TCHAR;
#define MainX(argc,argv) main(argc,argv)
#define strcpy_x(source, size, target) strcpy(source,target)

#elif defined(PLATFORM_WINNT)

#define MainX(argc,argv) _tmain(argc,argv)
#define strcpy_x(source, size, target) strcpy_s(source,size,target)
#include <tchar.h>

#elif defined(PLATFORM_LINUX)

#include <unistd.h>
#include <cstdlib>
typedef char _TCHAR;
#define MainX(argc,argv) main(argc,argv)
#define strcpy_x(source, size, target) strcpy(source,target)

#endif

using namespace std;
