#ifndef INET_H
#define INET_H
#include "MultiSys.h"

typedef enum call_type {
    CALL_CONNECTED = 0, /** ����Զ�˳ɹ� */
    CALL_CONNECT_FAILED, /** ����Զ��ʧ�� */
    CALL_REMOTE_CONNECTED, /** �µ�Զ�����ӽ��� */
    CALL_CONNECTION_BREAK, /** ���ӶϿ� */
    CALL_RECV_DATA,  /** �µ����ݵ��� */

    /** add call type before this */
    CALL_TYPE_COUNT
}CALLBACK_TYPE;

typedef void (*CALL_FUN)(const s32 nConnectID, const void * pContext, const s32 nSize);

class INet {
public:
    /** �첽����,�����Ļ���CALL_REMOTE_CONNECTED��CALL_CONNECTION_BREAK�¼��лش���ʹ����,ע��ֻ�ǻش�һ��ָ���ַ */
    virtual bool CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog = 2048)  = 0;

    /** �첽����,�����Ļ���CALL_CONNECTED��CALL_CONNECTION_BREAK�¼��лش���ʹ����,ע��ֻ�ǻش�һ��ָ�� */
    virtual bool CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext = NULL)  = 0;

    /** �ر�����,��ȫ�ر�,���������ݾ����ܷ�����֮��ر����� (�ò�����ٷ�CALL_CONNECTION_BREAK�¼� */
    virtual bool CClose(const s32 nConnectID)  = 0;

    /** �������� */
    virtual void CSend(const s32 nConnectID, const void * pData, const s32 nSize)  = 0;

    /** ��ȡ����Զ����Ϣ */
    virtual void CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort)  = 0;

    /** ���ûص���ַ,�ص����ͼ�CALLBACK_TYPE */
    virtual void CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address) = 0;

    /** �����¼���׽ demon�Ƿ�Ϊ����ģʽ, ����Ϊ����ģʽ, nFramemsΪ��֡loop�¼�, ��nFramemsΪ0ʱCLoopһֱ���� */
    virtual void CLoop(bool demon, s32 nFramems) = 0;
};


#define NAME_OF_GET_NET_FUN "GetNet"
#define GET_NET_FUN GetNet

#define NAME_OF_RETURN_NET_FUN "ReturnNet"
#define RETURN_NET_FUN ReturnNet

typedef INet* (*FUN_GET_NET)(const u8, const u16, const u16);
typedef void (*FUN_RETURN_NET)(INet *);

inline INet * GetNetWorker(const char * dllPath, const u32 threadCount = 1, const u32 conCount = 4096, const u16 waitTime = 10) {
    INet * pNetCore = NULL;
#if defined WIN32 || defined WIN64
    HINSTANCE hinst = ::LoadLibrary(dllPath);
    if (NULL == hinst) {
        ECHO_ERROR("LoadLibrary error, code : %d", GetLastError());
        ASSERT(hinst != NULL);
        return NULL;
    }

    FUN_GET_NET pFunGetNet = (FUN_GET_NET)::GetProcAddress(hinst, NAME_OF_GET_NET_FUN);
    ASSERT(pFunGetNet != NULL);
    if (NULL == pFunGetNet) {
        return NULL;
    }

    pNetCore = pFunGetNet(threadCount, conCount, waitTime);
    ASSERT(pNetCore != NULL);
#endif // WIN32 || WIN64
    return pNetCore;
}

#endif
