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
    * Initialize Ԥ�������ص� (��Ҫ������ģ���Լ��ڲ���׼��)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Initialize(IKernel * pKernel);
    /*
    * DelayInitialize ��ʱ�����ص� (��Ҫ������ģ��֮�����������)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool DelayInitialize(IKernel * pKernel);
    /*
    * Destroy ģ������ (ģ���������ݻ���)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Destroy(IKernel * pKernel);

    //ע����Ϣ�ص�
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


