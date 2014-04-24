#ifndef CCALENGINE_H
#define CCALENGINE_H
#include "ICallEngine.h"
#include "TCallBack.h"

namespace core {
    template<typename type, typename call_type, typename... Args>
    class CCallEngine : public ICallEngine<type, call_type, Args...> {
        typedef CHashMap<type, tlib::TCallBack<s32, call_type, Args...> > CALLPOOL_MAP;

    public:
        virtual bool RegsCall(const type & index, const s32 id, const call_type calladdress, const char * debuginfo) {
            return m_callpool[index].RegisterCall(id, calladdress, debuginfo);
        }

        virtual bool UnRegsCall(const type & index, const s32 id, const call_type calladdress) {
            return m_callpool[index].UnRgsCall(id, calladdress);
        }

        virtual void Call(const type & index, const s32 id, Args... args) {
           m_callpool[index].Call(id, args...);
        }

    private:
        CALLPOOL_MAP m_callpool;
    };
}
#endif //CCALENGINE_H
