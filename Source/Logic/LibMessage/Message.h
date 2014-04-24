#ifndef MESSAGE_H
#define MESSAGE_H

#include "IMessage.h"
#include "TCallBack.h"
#include "TStream.h"
#include "TPool.h"
#include "Tools.h"
using namespace std;
using namespace tlib;

typedef CHashMap<s32, TStream<4096, false> *> CONNECTID_STREAM_MAP;
typedef CHashMap<string, TCallBack<s32, MSG_CALL_BACK, MSG_ARGS> > CALL_POOL_MAP;

class Message : public IMessage {
public:
    /*
    * Initialize 预备启动回调 (主要用作各模块自己内部做准备)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool Initialize(IKernel * pKernel);
    /*
    * DelayInitialize 延时启动回调 (主要用作各模块之间的联合启动)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool DelayInitialize(IKernel * pKernel);
    /*
    * Destroy 模块销毁 (模块销毁数据回收)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool Destroy(IKernel * pKernel);

    //注册消息回调
    virtual bool RgsMsgCall(const char * type, const s32 nMsgID, const MSG_CALL_BACK pCallBack, const char * pCallName);

private:
    static s32 OnRecv(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize);
    static s32 OnConnected(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize);
    static s32 OnRemoteConnect(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize);
    static s32 OnConnectBreak(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize);
    static s32 OnConnectFailed(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize);

    static Message * s_pSelf;
    static CONNECTID_STREAM_MAP s_ConnectID_Stream_Map;
    static TPOOL<TStream<4096, false>, false, 8092> s_Stream_Pool;
    static CALL_POOL_MAP s_CallPool_Map;
};

#endif //MESAGE_H


