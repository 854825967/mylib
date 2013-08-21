#ifndef MULTISYS_H
#define MULTISYS_H
#include "WinSys.h"
#include "LinuxSys.h"
#include "MacSys.h"

#ifdef __cplusplus
inline void _AssertionFail(const char * strFile, int nLine, const char * pFunName) {
    //CSleep(1);
    ::fflush(stdout);
    ::fprintf(stderr, "\nAsssertion failed: file %s, line %d, function %s ", strFile, nLine, pFunName);
    ::fflush(stderr);
    ::abort();
}
#endif

#ifdef _DEBUG
#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#ifndef ASSERT

#ifdef __cplusplus
#define  ASSERT(p) ((p) ? (void)0 : (void)_AssertionFail(__FILE__, __LINE__, __FUNCTION__))
#endif //__cplusplus

#endif //ASSERT
#else
#define NEW new
#define  ASSERT(p) void(0);
#endif    //_DEBUG

#define BUFF_SIZE 4096

#endif //MULTISYS_H
