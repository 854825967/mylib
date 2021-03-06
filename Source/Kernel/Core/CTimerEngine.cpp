#include "CTimerEngine.h"
#include "CoreHeader.h"
namespace core {

    CTimerEngine::CTimerEngine() {
        m_Map_TimerHandler.clear();
        m_itor = m_Map_TimerHandler.begin();
    }

    bool CTimerEngine::SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr) {
        HandlerTimerMap::iterator itor = m_Map_TimerHandler.find(id);
        if (m_Map_TimerHandler.end() == itor) {
            CTimer * pTimer = m_Pool_Timer.Create();
            s64 lCurrentTick = ::GetCurrentTimeTick();
            pTimer->Format(id, lCurrentTick, tickcount, calltimes, pHandler, debugstr);
            m_Map_TimerHandler.insert(make_pair(id, pTimer));
            pHandler->AddTimer(id, pTimer);
            CORE_DEBUG("ITimerHandler %lx 增加定时器 %d, 定时器调试信息 %s", pHandler, pTimer->m_id, pTimer->m_debug_str);
            pHandler->SetTimerEngine(this);
        } else {
            CORE_ERROR("定时器 %d 已经存在, 调试信息为 %s", id, itor->second->m_debug_str);
        }

        return true;
    }

    bool CTimerEngine::KillTimer(const s32 id, ITimerHandler * pTimer) {
        HandlerTimerMap::iterator itor = m_Map_TimerHandler.find(id);
        if (itor != m_Map_TimerHandler.end()) {
            if (pTimer == itor->second->m_pHandler) {
                pTimer->RemoveTimer(id);
                CORE_DEBUG("定时器池回收定时器, 地址:%lx, 定时器当前ID:%d, 调试信息:%s", itor->second, id, itor->second->m_debug_str);
                m_Pool_Timer.Recover(itor->second);
                if (m_itor == itor) {
                    m_itor ++;
                }
                m_Map_TimerHandler.erase(itor);
            }
        }
        return true;
    }

    bool CTimerEngine::KillTimer(ITimerHandler * pTimer) {
        pTimer->RemoveAllTimer();
        return true;
    }

    s64 CTimerEngine::OnTimer(const s64 max_tickcounts) {
        HandlerTimerMap::iterator iend = m_Map_TimerHandler.end();
        s64 lStartTick = ::GetCurrentTimeTick();
        s64 lCurrentTick = lStartTick;
        while (m_itor != iend) {
            while (lStartTick - m_itor->second->m_currenttick >= m_itor->second->m_tickcount) {
                s64 lTick = ::GetCurrentTimeTick();
                s32 nLeftCount = m_itor->second->OnTimer();
                lCurrentTick = ::GetCurrentTimeTick();
                CORE_DEBUG("定时器%d,定时器信息:%s,单次执行耗时%d", m_itor->second->m_id, m_itor->second->m_debug_str, lCurrentTick - lTick);
                if (TIMERS_OVER == nLeftCount) {
                    m_itor->second->m_pHandler->RemoveTimer(m_itor->second->m_id);
                    CORE_DEBUG("定时器池回收定时器, 地址:%lx, 定时器当前ID:%d, 调试信息:%s", m_itor->second, m_itor->second->m_id, m_itor->second->m_debug_str);
                    m_Pool_Timer.Recover(m_itor->second);
                    m_Map_TimerHandler.erase(m_itor++);
                    break;
                }
            }

            if (m_itor == iend) {
                m_Map_TimerHandler.begin();
            } else {
                m_itor++;
            }

            if (lCurrentTick - lStartTick >= max_tickcounts) {
                CORE_DEBUG("定时器引擎loop超时");
                break;
            }
        }

        if (m_itor == iend) {
            m_itor = m_Map_TimerHandler.begin();
        }

        return 0;
    }

}