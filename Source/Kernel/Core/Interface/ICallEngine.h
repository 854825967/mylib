#ifndef ICALLENGINE_H
#define ICALLENGINE_H
#include "ICore.h"
namespace core {
    
#define EVENTCALL_ARGS const s8, const s32, const void *, const s32
    typedef s32 (*EVENTCALLFUN)(EVENTCALL_ARGS);

    template<typename type, typename calltype, typename... Args>
    class ICallEngine : public ICore {
    public:
        virtual bool RegsCall(const type & index, const s32 id, const calltype calladdress, const char * debuginfo) = 0;

        virtual bool UnRegsCall(const type & index, const s32 id, const calltype calladdress) = 0;

        virtual void Call(const type & index, const s32 id, Args... args) = 0;
    };

}
#endif // !ICALLENGINE_H
