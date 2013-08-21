#include "MultiSys.h"
#include "LogSystem.h"

//#include "Pool.h"

class INet;

#ifdef __cplusplus
extern "C" {
#endif

#if defined WIN32 || defined WIN64
__declspec(dllexport)
#endif //#if defined WIN32 || defined WIN64

ILog * GET_LOG_FUN(const char * filename, const u32 duration = 3600, const u32 maxsize = 50) {
    LogSystem * p = NEW LogSystem(filename, duration, maxsize);
    return p;
}

#if defined WIN32 || defined WIN64
__declspec(dllexport)
#endif //#if defined WIN32 || defined WIN64
void RETURN_LOG_FUN(ILog * p) {
    delete p;
}

#ifdef __cplusplus
};
#endif
