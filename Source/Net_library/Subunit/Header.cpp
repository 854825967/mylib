#include "Header.h"
#include "Tools.h"
#include "TLogger.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    tlib::TLogger<LOG_FILE_COUNT>  g_Logger;

    void LogModuleInit() {
        std::string filePath = ::GetAppPath();
        g_Logger.OpenLogFile(LOG_TRACE, filePath.c_str(), "NetTrace");
        g_Logger.OpenLogFile(LOG_DEBUG, filePath.c_str(), "NetDebug");
        g_Logger.OpenLogFile(LOG_ERROR, filePath.c_str(), "NetError");
        g_Logger.Start();
    }

    void LogModuleStop() {
        g_Logger.Stop();
    }

    void Trace(const char * log) {
        g_Logger.Log(LOG_TRACE, log);
    }

    void Debug(const char * log) {
        g_Logger.Log(LOG_DEBUG, log);
    }

    void Error(const char * log) {
        g_Logger.Log(LOG_ERROR, log);
    }
#ifdef __cplusplus
};
#endif //__cplusplus
