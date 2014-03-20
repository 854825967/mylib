#include "CoreHeader.h"
#include "Tools.h"
#include <string>
using namespace tlib;
TLogger<CORELOG_COUNT> * g_pCoreLogger = NULL;

namespace core {

    void CoreHeaderInit() {
        static bool inited = false;
        if (!inited) {
            g_pCoreLogger = NEW TLogger<CORELOG_COUNT>;
            std::string filePath = ::GetAppPath();
            g_pCoreLogger->OpenLogFile(CORELOG_TRACE, filePath.c_str(), "CoreTrace");
            g_pCoreLogger->OpenLogFile(CORELOG_DEBUG, filePath.c_str(), "CoreDebug");
            g_pCoreLogger->OpenLogFile(CORELOG_ERROR, filePath.c_str(), "CoreError");
            g_pCoreLogger->Start();
        }
    }

    void CoreHeaderRelease() {
        static bool released = false;
        if (!released) {
            g_pCoreLogger->Stop();
            delete g_pCoreLogger;
            g_pCoreLogger = NULL;
        }
    }
}
