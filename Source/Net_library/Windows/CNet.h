#if defined WIN32 || defined WIN64

#ifndef CNET_H
#define CNET_H

#include "INet.h"
#include "TQueue.h"
#include "TPool.h"
#include "CConnection.h"

class CNet : public INet {
public:
    CNet(const s8 nThreadCount, const s32 nLinkConut, const s32 nWaitMs);
    ~CNet();

    /** �첽����,�����Ļ���CALL_REMOTE_CONNECTED�¼��лش���ʹ����,ע��ֻ�ǻش�һ��ָ���ַ */
    virtual bool CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog);

    /** �첽����,�����Ļ���CALL_CONNECTED�¼��лش���ʹ����,ע��ֻ�ǻش�һ��ָ�� */
    virtual bool CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext);

    /** �������� */
    virtual void CSend(const s32 nConnectID, const void * pData, const s32 nSize);

    /** ��ȡ����Զ����Ϣ */
    virtual void CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort);

    /** �ر�����,��ȫ�ر�,���������ݾ����ܷ�����֮��ر����� (�ò�����ٷ�CALL_CONNECTION_BREAK�¼� */
    virtual bool CClose(const s32 nConnectID);

    /** ���ûص���ַ,�ص����ͼ�CALLBACK_TYPE */
    virtual void CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address);    
    
    /** �����¼���׽ demon�Ƿ�Ϊ����ģʽ, ����Ϊ����ģʽ, nFramemsΪ��֡loop�¼�, ��nFramemsΪ0ʱCLoopһֱ���� */
    virtual void CLoop(bool demon, s32 nFramems);

private:
    void DealConnectEvent(struct iocp_event * pEvent);
    void DealAcceptEvent(struct iocp_event * pEvent);
    void DealRecvEvent(struct iocp_event * pEvent);
    void DealSendEvent(struct iocp_event * pEvent);

    void SafeShutdwon(struct iocp_event * pEvent, s32 sdtags);

private:
    static THREAD_FUN DemonLoop(LPVOID p);
    static THREAD_FUN NetLoop(LPVOID p);
    
private:
    s64 m_nWaitMs;
    s64 m_nFrameMs;
    s32 m_nThreadCount;
    CALL_FUN m_szCallAddress[CALL_TYPE_COUNT];
    tlib::TPOOL<CConnection, true, 8192> m_ConnectPool;
    tlib::TQueue<struct iocp_event *, true, 8192> m_queue;
    bool m_demo;
};

#endif
#endif //#if defined WIN32 || defined WIN64
