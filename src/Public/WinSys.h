#if defined WIN32 || defined WIN64

#ifndef WINSYS_H
#define WINSYS_H

#include <stdio.h>
#ifndef _WINSOCK2API_  
#include <WinSock2.h>  
#endif  
#ifndef _WINDOWS_  
#include <Windows.h>  
#endif  
#include <shlwapi.h>
#include <Mswsock.h>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <process.h>
#include <tchar.h>
#include <stdlib.h>
#include <direct.h>

#pragma comment( lib, "ws2_32.lib" )
#pragma comment(lib, "shlwapi.lib")

#define CSleep(n) Sleep(n)
#define SafeSprintf _snprintf_s
#define ThreadID DWORD
#define THREAD_FUN DWORD WINAPI
typedef HANDLE CHandle;

#define ECHO(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); \
    printf("%s\n", _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

#define ECHO_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN); \
    printf("[info] %s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

#define ECHO_WARN(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN); \
    printf("[warn]%s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

#define ECHO_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED); \
    printf("[error]%s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char s8;
typedef short s16;
typedef int s32;

#ifdef WIN32 
typedef unsigned long long u64;
typedef long long s64;
#else WIN64
typedef unsigned long u64;
typedef long s64;
#endif


#endif //WINSYS_H

#endif //#if defined WIN32 || defined WIN64
