#include "MultiSys.h"
#include "CNet.h"

//#include "Pool.h"

class INet;

//#ifdef __cplusplus
//extern "C" {
//#endif

#if defined WIN32 || defined WIN64
__declspec(dllexport)
#endif //#if defined WIN32 || defined WIN64

INet * GET_NET_FUN(const u8 threadcount, const u16 sockcount, const u16 waittime) {
    CNet * p = NEW CNet(threadcount, sockcount, waittime);
    return p;
}

#if defined WIN32 || defined WIN64
__declspec(dllexport)
#endif //#if defined WIN32 || defined WIN64
void RETURN_NET_FUN(INet * p) {
    delete p;
}

//#ifdef __cplusplus
//};
//#endif
