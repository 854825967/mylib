#ifndef ITIMERHANDLER_H
#define ITIMERHANDLER_H
#include "MultiSys.h"
#include "CHashMap.h"

#define LOOP_TIMER 0
#define TIMERS_OVER 1

namespace core {
    class CTimer;
    class ITimerEngine;

    typedef CHashMap<s32, CTimer *> HandlerTimerMap;

    class ITimerHandler {
    public:
        virtual s32 OnTimer(const s32 id) = 0;

        void AddTimer(const s32 id, CTimer * pTimer);
        void RemoveAllTimer();
        void RemoveTimer(const s32 id);
        void SetTimerEngine(ITimerEngine * pTimerEngine);
        ~ITimerHandler();
    private:
        ITimerEngine * m_pTimeEngine;
        HandlerTimerMap m_Map_TimerHandler;
    };

}
#endif //ITIMERHANDLER_H
