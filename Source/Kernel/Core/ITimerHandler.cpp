#include "ITimerHandler.h"
#include "CTimer.h"
#include "CoreHeader.h"
#include "Interface/ITimerEngine.h"

namespace core {

    ITimerHandler::~ITimerHandler() {
        m_pTimeEngine->KillTimer(this);
    }

    void ITimerHandler::SetTimerEngine(ITimerEngine * pTimerEngine) {
        m_pTimeEngine = pTimerEngine;
    }

    void ITimerHandler::AddTimer(const s32 id, CTimer * pTimer) {
        m_Map_TimerHandler.insert(make_pair(id, pTimer));
        CORE_DEBUG("ITimerHandler %lx 增加定时器 %d, 定时器调试信息 %s", this, id, pTimer->m_debug_str);
    }

    void ITimerHandler::RemoveAllTimer() {
        HandlerTimerMap::iterator ibegin = m_Map_TimerHandler.begin();
        while (ibegin != m_Map_TimerHandler.end()) {
            m_pTimeEngine->KillTimer(ibegin->second->m_id, this);
            ibegin = m_Map_TimerHandler.begin();
        }
    }

    void ITimerHandler::RemoveTimer(const s32 id) {
        HandlerTimerMap::iterator itor = m_Map_TimerHandler.find(id);
        if (itor != m_Map_TimerHandler.end()) {
            CORE_DEBUG("清除定时器:%d, 定时器调试信息: %s", itor->second->m_id, itor->second->m_debug_str);
            m_Map_TimerHandler.erase(itor);
        }
    }

}