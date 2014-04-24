#ifndef TLOGGER_H
#define TLOGGER_H
#include "MultiSys.h"
#include "TQueue.h"
#include "Tools.h"
#include <fstream>
#include <string>
using namespace std;

#define TICK_OF_DAY (24 * 60 * 60 * 1000)

namespace tlib {
#define LOG_OPT_LOCK(b, lock) \
    if (b) { \
    lock->Lock(); \
    }

#define LOG_OPT_FREELOCK(b, lock) \
    if (b) { \
    lock->UnLock(); \
    }

    class Logfile {
    public:
        Logfile() {
            
        }
        
        Logfile(const char * filepath) {
            InitializeTLogger(filepath);
        }
        
        void InitializeTLogger(const char * filepath) {
            ASSERT(!m_log_file.is_open());
            m_log_file.open(filepath, std::ios::app | std::ios::out);
            ASSERT(m_log_file.is_open());
        }
        
        void In(const s64 lTick, const char * pContext, const char * head) {
            std::string timeStr = ::GetTimeString(lTick).c_str();
            ASSERT(m_log_file.is_open());
            if (head != NULL) {
                m_log_file << head << " | " << timeStr << " : " << pContext << "\n" << std::flush;
            } else {
                m_log_file << timeStr << " : " << pContext << "\n" << std::flush;
            }
        }
        
        void Close() {
            if (m_log_file.is_open()) {
                m_log_file.flush();
                m_log_file.close();
                m_log_file.clear();
            }
        }

    private:
        std::ofstream m_log_file;
    };


    template<const s32 buffMax>
    class TLogContext {
    public:
        s8 type;
        s64 timetick;
        char context[buffMax];
        s16 len;

        TLogContext() {
            memset(this, 0, sizeof(*this));
        }

        TLogContext<buffMax> & operator = (const TLogContext<buffMax> & target) {
            type = target.type;
            len = target.len;
            timetick = target.timetick;
            memcpy(context, target.context, len + 1);
            return *this;
        }
    };

    enum {
        WORK_THREAD_WORK = 0,
        WORK_THREAD_STOPING = 1,
        WORK_THREAD_STOPED = 2
    };

    template<const s32 fileCount, s32 buffMax = 2048, bool loglock = false>
    class TLogger {
    public:
        TLogger() {
            m_state = WORK_THREAD_STOPED;
            memset(m_loghead_str, 0, sizeof(m_loghead_str));
            m_pLock = NULL;
            if (loglock) {
                m_pLock = NEW CLockUnit;
            }
        }

        ~TLogger() {
            Stop();
            for (s32 i=0; i<fileCount; i++) {
                m_logfile[i].Close();
            }
        }
        void Start() {
            m_state = WORK_THREAD_WORK;
#if defined WIN32 || defined WIN64
            HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(TLogger<fileCount, buffMax, loglock>::LogThread), (LPVOID)this, 0, NULL);
            if (hThread == NULL) {
                DWORD dw = GetLastError();
                ECHO_ERROR("Start log thread error, last error code %d", dw);
                ASSERT(false);
            } else {
                CloseHandle(hThread);
                ECHO_TRACE("Log thread started");
            }
#endif //defined WIN32 || defined WIN64
            
            
#if defined __IPHONE_OS_VERSION_MAX_ALLOWED || defined IS_IPAD
            int errCode=0;
            do {
                pthread_attr_t tAttr;
                errCode=pthread_attr_init(&tAttr);
                CC_BREAK_IF(errCode!=0);
                errCode = pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
                if(errCode != 0) {
                    pthread_attr_destroy(&tAttr);
                    break;
                }
                pthread_t t;
                errCode = pthread_create(&t, &tAttr, (void * (*)(void *))(TLogger<fileCount, buffMax, loglock>::LogThread), this);
            } while (0);
#endif //#if defined __IPHONE_OS_VERSION_MAX_ALLOWED || defined IS_IPAD
            return;
            
        }

        void Stop() {
            if (WORK_THREAD_WORK == m_state) {
                m_state = WORK_THREAD_STOPING;
                while (m_state != WORK_THREAD_STOPED) {
                    CSleep(1);
                }
            }
        }

        void OpenLogFile(const s32 fileIndex, const char * filepath, const char * loghead = NULL) {
            ASSERT(fileIndex < fileCount);
            m_filepath[fileIndex] = filepath;
            string filename = m_filepath[fileIndex] + "/" + loghead + "_" + ::GetCurrentTimeString("%4d-%d-%d") + ".log";
            m_logfile[fileIndex].InitializeTLogger(filename.c_str());
            if (loghead != NULL) {
                SafeSprintf(m_loghead_str[fileIndex], sizeof(m_loghead_str[fileIndex]), "%s", loghead);
            }
        }

        void Log(const s32 fileIndex, const char * context) {
            if (m_state != WORK_THREAD_WORK) {
                return;
            }
            LOG_OPT_LOCK(loglock, m_pLock);
            m_queue.addByArgs(fileIndex, context);
            LOG_OPT_FREELOCK(loglock, m_pLock);
        }

    private:
        static THREAD_FUN LogThread(LPVOID * p) {
            TLogger<fileCount, buffMax, loglock> * pThis = (TLogger<fileCount, buffMax, loglock> *)p;
            TLogContext<buffMax> log;
            s64 lTimeTick = ::GetTimeTickOfDayBeginning();
            while (true) {
                if (pThis->m_queue.read(log)) {
                    if (log.timetick - lTimeTick >= TICK_OF_DAY) {
                        for (s32 i=0; i<fileCount; i++) {
                            pThis->m_logfile[i].Close();
                            std::string filename = pThis->m_filepath[i] + "/" + pThis->m_loghead_str[i] + "_" + ::GetCurrentTimeString("%4d-%d-%d") + ".log";
                            pThis->m_logfile[i].InitializeTLogger(filename.c_str());
                        }
                        lTimeTick = ::GetTimeTickOfDayBeginning();
                    }

                    (pThis->m_logfile[log.type]).In(log.timetick, log.context, pThis->m_loghead_str[log.type]);
                } else if (pThis->m_state == WORK_THREAD_STOPING && pThis->m_queue.IsEmpty()) {
                    pThis->m_state = WORK_THREAD_STOPED;
                    ECHO_TRACE("Log thread has stopped");
                    return 0;
                } else {
                    CSleep(1);
                }
            }
        }

        typedef void (*funtype)(TLogContext<buffMax> &, const s8, const char *);
        static void LogInQueue(TLogContext<buffMax> & log, const s8 type, const char * context) {
            ASSERT(type < fileCount);
            log.type = type;
            s32 nLen = strlen(context);
            if (nLen >= buffMax) {
                memcpy(log.context, context, buffMax - 1);
                log.context[buffMax - 1] = 0;
                log.len = buffMax - 1;
            } else {
                memcpy(log.context, context, nLen);
                log.context[nLen] = 0;
                log.len = nLen;
            }
            log.timetick = ::GetCurrentTimeTick();
        }

    private:
        std::string m_filepath[fileCount];
        Logfile m_logfile[fileCount];
        char m_loghead_str[fileCount][128];
        CLockUnit * m_pLock;
        TQueue<TLogContext<buffMax>, false, 1024, funtype, TLogger<fileCount, buffMax, loglock>::LogInQueue, const s8, const char *> m_queue;
        s8 m_state;
    };
}

#endif //TLOGGER_H
