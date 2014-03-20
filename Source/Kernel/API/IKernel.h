/*
 * @defgroup ģ���� ģ���˵������

 * @{

 */
 
#include "MultiSys.h"
#include "IModule.h"

enum error_code {
    KERNEL_ERROR_CODE_MAX   /** @brief ��Ҫ˵�����֣���ǰ��� @brief �Ǳ�׼��ʽ�� */
};

#define ANY_CALL 0
#define MSG_CALL_ARGS const char *, const s32, const void *, const s32 size
typedef s32 (*MSG_CALL)(MSG_CALL_ARGS);

#define EVENT_CALL_ARGS const char *, const s32, const void *, const s32 size
typedef s32 (*EVENT_CALL)(EVENT_CALL_ARGS);

#define ADD_MSG_CALL(pKernel, type, msgid, call) \
    pKernel->AddMsgCall(type, msgid, call, #call);

#define ADD_NET_EVENT_CALL(pKernel, type, eventid, call) \
    pKernel->AddNetEventCall(type, eventid, call, #call);

class IKernel {
    /*
    * ��Ҫ�ĺ���˵������ 
    *  @param [in] param1 ����1˵��
    *  @param [out] param2 ����2˵��
    *  @return ����ֵ˵��
    */

    //���緽��ӿ�
    virtual void Send(const s32 nConID, const void * pBuff, const s32 nSize) = 0;
    virtual void AddMsgCall(const char * pStrType, const s32 nMsgID, const MSG_CALL pCall, const char * pStrCallName) = 0;
    virtual void RemoveMsgCall(const char * pStrType, const s32 nMsgID, const MSG_CALL pCall) = 0;

    virtual void AddNetEventCall(const char * pStrType, const s32 nEventID, const EVENT_CALL pCall, const char * pStrCallName) = 0;
    virtual void RemoveNetEventCall(const char * pStrType, const s32 nEventID, const EVENT_CALL pCall) = 0;

    //���ݿⷽ��ӿ�(�첽/ͬ��)
    virtual s32 DBRequest(const s32 nIndex, const s32 nRequestID, const s64 nRequesterID, 
        const void * pInData, const s32 nInSize, IModule * pModule) = 0;

    //Redis����ӿ�(�첽/ͬ��)
    virtual s32 RedisRequest(const s32 nIndex, const s32 nRequersID, const s64 nRequesterID, 
        const void * pInData, const s32 nInSize, IModule * pModule) = 0;

    //��ʱ��
    virtual void AddTimerCall(const s32 nTimerID, const s32 nUid, const void * pData, const s32 nSize) = 0;
    virtual void RemoveTimerCall(const s32 nTimerID, const s32 nUid) = 0;

};

/** @} */ // ģ���β
