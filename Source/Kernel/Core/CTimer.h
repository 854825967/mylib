#ifndef CTIMER_H
#define CTIMER_H

#include "MultiSys.h"

namespace core {
    class ITimerHandler;

    class CTimer {
    public:
        CTimer();
        CTimer(const s32 id, const s64 currenttick, const s64 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * pDebug);

        inline void Format(const s32 id, const s64 currenttick, const s64 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * pDebug);
        s32 OnTimer();
    
        s32 m_id;
        s64 m_currenttick;
        s64 m_tickcount;
        s32 m_calltimes;
        ITimerHandler * m_pHandler;
        char m_debug_str[512];
    };
}

#endif //CTIMER_H
