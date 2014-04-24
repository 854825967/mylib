#include "CoreHeader.h"
#include "Kernel.h"
using namespace core;

#if defined WIN32 || defined WIN64
#include "CDumper.h"
#include "Tools.h"
using namespace tlib;
class InitDumper {
public:
    InitDumper() {
        CDumper::GetInstance().SetDumpFileName("core.dmp");
    }
};
InitDumper initdump;
#endif //defined WIN32 || defined WIN64

s32 main() {
#if defined WIN32 || defined WIN64
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);// ³õÊ¼»¯Windows Socket 2.2
#endif //defined WIN32 || defined WIN64

    CoreHeaderInit();
    Kernel * pKernel = Kernel::GetKernel();
    pKernel->Start();
    pKernel->Loop();
    pKernel->Release();
    CoreHeaderRelease();
    return 0;
}
