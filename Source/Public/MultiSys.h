#ifndef MULTISYS_H
#define MULTISYS_H
#include "WinSys.h"
#include "LinuxSys.h"
#include "MacSys.h"
#include "IOSSys.h"

#ifdef __cplusplus
extern "C" {
#endif
    void _AssertionFail(const char * strFile, int nLine, const char * pFunName);
    typedef void (*VOID_FUN_TYPE)(void);
    void _void_fun(void);
#ifdef __cplusplus
};
#endif


#if defined _DEBUG
//#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define NEW new
#define MALLOC malloc
#define FREE free
#ifndef ASSERT
#define  ASSERT(p) ((p) ? (void)0 : (void)_AssertionFail(__FILE__, __LINE__, __FUNCTION__))
#endif //ASSERT
#else
#define NEW new
#define MALLOC malloc
#define FREE free
#define  ASSERT(p)
#endif //_DEBUG

#define BUFF_SIZE 4096

#ifndef OUT
#define OUT
#endif //OUT

#ifndef IN
#define IN
#endif //IN

#endif //MULTISYS_H
