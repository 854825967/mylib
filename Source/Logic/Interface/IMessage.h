#ifndef IMESSAGE_H
#define IMESSAGE_H

#include "IModule.h"

#define MSG_ARGS const char *, s32, void *, const s32
typedef s32 (*MSG_CALL_BACK)(MSG_ARGS);

#define RGS_MSG_CALL(pMsg, type, id, call) \
    pMsg->RgsMsgCall(type, id, call, #call);

#define SEND_MSG(pKernel, connectid, msgid, data) { \
    s32 nMsgId = msgid; \
    s32 nLen = sizeof(s32) + sizeof(nMsgId) + sizeof(data); \
    pKernel->Send(connectid, &nLen, sizeof(nLen)); \
    pKernel->Send(connectid, &nMsgId, sizeof(nMsgId)); \
    pKernel->Send(connectid, &data, sizeof(data)); \
}

class IMessage : public IModule {
public:
    virtual bool RgsMsgCall(const char * type, const s32 nMsgID, const MSG_CALL_BACK pCallBack, const char * pCallName) = 0;
};

#endif //IMESSAGE_H
