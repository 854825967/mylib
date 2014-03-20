#include "CTimerEngine.h"
#include "CoreHeader.h"
namespace core {

    CTimerEngine::CTimerEngine() {
        m_Map_TimerHandler.clear();
    }

    bool CTimerEngine::SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr) {
        HandlerTimerMap::iterator itor = m_Map_TimerHandler.find(id);
        if (m_Map_TimerHandler.end() == itor) {
            CTimer * pTimer = m_Pool_Timer.Create();
            s64 lCurrentTick = ::GetCurrentTimeTick();
            pTimer->Format(id, lCurrentTick, tickcount, calltimes, pHandler, debugstr);
            m_Map_TimerHandler.insert(make_pair(id, pTimer));
            pHandler->AddTimer(id, pTimer);
            CORE_DEBUG("ITimerHandler %lx ���Ӷ�ʱ�� %d, ��ʱ��������Ϣ %s", pHandler, pTimer->m_id, pTimer->m_debug_str);
            pHandler->SetTimerEngine(this);
        } else {
            CORE_ERROR("��ʱ�� %d �Ѿ�����, ������ϢΪ %s", id, itor->second->m_debug_str);
        }

        return true;
    }

    bool CTimerEngine::KillTimer(const s32 id, ITimerHandler * pTimer) {
        HandlerTimerMap::iterator itor = m_Map_TimerHandler.find(id);
        if (itor != m_Map_TimerHandler.end()) {
            if (pTimer == itor->second->m_pHandler) {
                pTimer->RemoveTimer(id);
                CORE_DEBUG("��ʱ���ػ��ն�ʱ��, ��ַ:%lx, ��ʱ����ǰID:%d, ������Ϣ:%s", itor->second, id, itor->second->m_debug_str);
                m_Pool_Timer.Recover(itor->second);
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
        static HandlerTimerMap::iterator itor = m_Map_TimerHandler.begin();
        HandlerTimerMap::iterator iend = m_Map_TimerHandler.end();
        s64 lStartTick = ::GetCurrentTimeTick();
        s64 lCurrentTick = lStartTick;
        while (itor != iend) {
            while (lStartTick - itor->second->m_currenttick >= itor->second->m_tickcount) {
                s64 lTick = ::GetCurrentTimeTick();
                s32 nLeftCount = itor->second->OnTimer();
                lCurrentTick = ::GetCurrentTimeTick();
                CORE_DEBUG("��ʱ��%d,��ʱ����Ϣ:%s,����ִ�к�ʱ%d", itor->second->m_id, itor->second->m_debug_str, lCurrentTick - lTick);
                if (TIMERS_OVER == nLeftCount) {
                    itor->second->m_pHandler->RemoveTimer(itor->second->m_id);
                    CORE_DEBUG("��ʱ���ػ��ն�ʱ��, ��ַ:%lx, ��ʱ����ǰID:%d, ������Ϣ:%s", itor->second, itor->second->m_id, itor->second->m_debug_str);
                    m_Pool_Timer.Recover(itor->second);
                    itor = m_Map_TimerHandler.erase(itor);
                    break;
                }
            }
            if (itor == iend) {
                break;
            } else {
                itor++;
            }
            if (lCurrentTick - lStartTick >= max_tickcounts) {
                CORE_DEBUG("��ʱ������loop��ʱ");
                break;
            }
        }

        if (itor == iend) {
            itor = m_Map_TimerHandler.begin();
        }

        return 0;
    }

}