#ifndef ITIMERENGINE_H
#define ITIMERENGINE_H

#include "ICore.h"
namespace core {
    class ITimerHandler;

    class ITimerEngine : public ICore{
    public:
        virtual bool SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr) = 0; //debug string 不要超过511字节
        virtual bool KillTimer(const s32 id, ITimerHandler * pTimer) = 0;
        virtual bool KillTimer(ITimerHandler * pTimer) = 0;
        virtual s64 OnTimer(const s64 max_tickcounts) = 0;
    };
}
#endif //ITIMERENGINE_H
