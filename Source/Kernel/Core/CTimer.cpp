#include "CTimer.h"
#include "ITimerHandler.h"

namespace core {
    CTimer::CTimer() {
        memset(this, 0, sizeof(*this));
    }

    CTimer::CTimer(const s32 id, const s64 currenttick, const s64 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * pDebug) {
        Format(id, currenttick, tickcount, calltimes, pHandler, pDebug);
    }

    void CTimer::Format(const s32 id, const s64 currenttick, const s64 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * pDebug) {
        memset(this, 0, sizeof(*this));
        m_id = id;
        m_currenttick = currenttick;
        m_tickcount = tickcount;
        m_calltimes = calltimes;
        m_pHandler = pHandler;
        if (pDebug != NULL) {
            SafeSprintf(m_debug_str, sizeof(m_debug_str), "%s", pDebug);
        } else {
            SafeSprintf(m_debug_str, sizeof(m_debug_str), "no debug info");
        }
    }

    s32 CTimer::OnTimer() {
        m_pHandler->OnTimer(m_id);
        m_currenttick += m_tickcount;
        if (LOOP_TIMER == m_calltimes) {
            return LOOP_TIMER;
        }
        return m_calltimes--;
    }

}
