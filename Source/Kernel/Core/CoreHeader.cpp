#include "CoreHeader.h"
#include "Tools.h"
#include <string>
using namespace tlib;

namespace core {
    TLogger<LOG_CORE_COUNT> * g_pCoreLogger = NULL;
    ITimerEngine * g_pTimeEngine = NULL;
    INet * g_pNetEngine = NULL;
    IHttpEngine * g_pHttpEngine = NULL;
    ICallEngine<s8, EVENTCALLFUN, EVENTCALL_ARGS> * g_pEventCallEngine;

    
    bool inited = false;

    void CoreHeaderInit() {
        ASSERT(!inited);
        if (!inited) {
            g_pCoreLogger = NEW TLogger<LOG_CORE_COUNT>;
            std::string filePath = ::GetAppPath();
            g_pCoreLogger->OpenLogFile(LOG_CORE_TRACE, filePath.c_str(), "CoreTrace");
            g_pCoreLogger->OpenLogFile(LOG_CORE_DEBUG, filePath.c_str(), "CoreDebug");
            g_pCoreLogger->OpenLogFile(LOG_CORE_ERROR, filePath.c_str(), "CoreError");
            g_pCoreLogger->OpenLogFile(LOG_LOGIC, filePath.c_str(), "Logic");
            g_pCoreLogger->Start();

            g_pTimeEngine = NEW CTimerEngine;
            g_pEventCallEngine = NEW CCallEngine<s8, EVENTCALLFUN, EVENTCALL_ARGS>;
            g_pNetEngine = NEW CNet;
            g_pHttpEngine = NEW CHttpEngine;

            inited = true;
        }
    }

    void CoreHeaderRelease() {
        ASSERT(inited);
        if (inited) {
            g_pCoreLogger->Stop();
            delete g_pCoreLogger;
            g_pCoreLogger = NULL;
            
            delete g_pTimeEngine;
            g_pTimeEngine = NULL;
            
            delete g_pEventCallEngine;
            g_pEventCallEngine = NULL;

            delete g_pNetEngine;
            g_pNetEngine = NULL;

            delete g_pHttpEngine;
            g_pHttpEngine = NULL;

            inited = false;
        }
    }
}
