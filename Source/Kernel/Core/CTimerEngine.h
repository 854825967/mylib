#ifndef CTIMERENGINE_H
#define CTIMERENGINE_H

#include "Interface/ITimerEngine.h"
#include "ITimerHandler.h"
#include "TPool.h"
#include "CTimer.h"
#include <list>

namespace core {

    class CTimerEngine : public ITimerEngine {
    public:
        CTimerEngine();
        virtual bool SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr);  //同一个ID只能注册一次
        virtual bool KillTimer(const s32 id, ITimerHandler * pTimer);
        virtual bool KillTimer(ITimerHandler * pTimer); 
        virtual s64 OnTimer(const s64 max_tickcounts);
    private:
        HandlerTimerMap m_Map_TimerHandler;
        tlib::TPOOL<CTimer, false, 8096> m_Pool_Timer;
        HandlerTimerMap::iterator m_itor;

    };
}

#endif //CTIMERENGINE_H
