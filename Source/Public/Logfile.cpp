#include "TLogger.h"
#include "Tools.h"

namespace tlib {
    Logfile::Logfile() {

    }

    Logfile::Logfile(const char * filepath) {
        InitializeTLogger(filepath);
    }

    void Logfile::InitializeTLogger(const char * filepath) {
        ASSERT(!m_log_file.is_open());
        m_log_file.open(filepath, std::ios::app | std::ios::out);
    }

    void Logfile::In(const s64 lTick, const char * TLogContext, const char * head) {
        std::string timeStr = ::GetTimeString(lTick).c_str();
        if (head != NULL) {
            m_log_file << head << " | " << timeStr << " : " << TLogContext << "\n" << std::flush;
        } else {
            m_log_file << timeStr << " : " << TLogContext << "\n" << std::flush;
        }
    }
    
    void Logfile::Close() {
        ASSERT(m_log_file.is_open());
        m_log_file.flush();
        m_log_file.close();
        m_log_file.clear();
    }
}