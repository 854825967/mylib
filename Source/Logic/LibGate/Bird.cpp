#include "Bird.h"
#include "IKernel.h"

UpdateField::_Format a;

Bird::Bird() {
    Clear();
}

void Bird::Clear() {
    m_pKernel = NULL;
    memset(&m_States, 0, sizeof(m_States));
    m_lAccountID = 0;
    m_nConnectID = CONNECTION_BREAK;
}

void Bird::SetConnection(IKernel * pKernel, const s32 nConnectID, const s64 lAccountID) {
    m_pKernel = pKernel;
    m_nConnectID = nConnectID;
    m_lAccountID = lAccountID;
}

void Bird::Send(const s32 nMsgID, const void * pData, const s32 nSize) {
    s32 nLen = sizeof(s32) + sizeof(nMsgID) + nSize;
    m_pKernel->Send(m_nConnectID, &nLen, sizeof(nLen));
    m_pKernel->Send(m_nConnectID, &nMsgID, sizeof(nMsgID));
    m_pKernel->Send(m_nConnectID, pData, nSize);
}

s64 Bird::GetAccountID() {
    return m_lAccountID;
}

void Bird::Sync(const s32 nAttribID, const void * pData, const s32 nLen) {
    ECHO("Sync Attrib %d, Len %d", nAttribID, nLen);
}
