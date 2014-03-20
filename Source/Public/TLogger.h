#ifndef TLOGGER_H
#define TLOGGER_H
#include "MultiSys.h"
#include "TQueue.h"
#include "Tools.h"
#include <fstream>
#include <string>

#define TICK_OF_DAY (24 * 60 * 60 * 1000)

namespace tlib {
    class Logfile {
    public:
        Logfile();
        Logfile(const char * filepath);
        void InitializeTLogger(const char * filepath);
        void Close();
        void In(const s64 lTick, const char * TLogContext, const char * head = NULL);

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

    template<const s32 fileCount, s32 buffMax = 2048>
    class TLogger {
    public:
        TLogger() {
            m_state = WORK_THREAD_STOPED;
            memset(m_loghead_str, 0, sizeof(m_loghead_str));
        }

        ~TLogger() {
            Stop();
            for (s32 i=0; i<fileCount; i++) {
                m_logfile[i].Close();
            }
        }

        void Start() {
            m_state = WORK_THREAD_WORK;
            HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(TLogger<fileCount, buffMax>::LogThread), (LPVOID)this, 0, NULL);
            if (hThread == NULL) {
                DWORD dw = GetLastError();
                ECHO_ERROR("Start log thread error, last error code %d", dw);
                ASSERT(false);
            } else {
                CloseHandle(hThread);
                ECHO_TRACE("Log thread started");
            }
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
            std::string filename = m_filepath[fileIndex] + "/" + loghead + "_" + ::GetCurrentTimeString("%4d-%d-%d") + ".log";
            m_logfile[fileIndex].InitializeTLogger(filename.c_str());
            if (loghead != NULL) {
                SafeSprintf(m_loghead_str[fileIndex], sizeof(m_loghead_str[fileIndex]), "%s", loghead);
            }
        }

        void Log(const s32 fileIndex, const char * TLogContext) {
            if (m_state != WORK_THREAD_WORK) {
                return;
            }
            m_queue.addByArgs(fileIndex, TLogContext);
        }

    private:
        static THREAD_FUN LogThread(LPVOID * p) {
            TLogger<fileCount, buffMax> * pThis = (TLogger<fileCount, buffMax> *)p;
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
                }
            }
        }

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
        TQueue<TLogContext<buffMax>, true, 1024, TLogger<fileCount, buffMax>::LogInQueue, const s8, const char *> m_queue;
        s8 m_state;
    };
}

#endif //TLOGGER_H
