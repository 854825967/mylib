#include <string>
#include "CStream.h"

CStream::CStream()
{
    memset(m_szBuff, 0, sizeof(m_szBuff));
    m_nReadCurrent = 0;
    m_nWriteCurrent = 0;
    m_nMaxLength = STREAM_LENGTH;
}

CStream::~CStream()
{

}

const char * CStream::GetBuff() const{
    return (char *)&m_szBuff[m_nReadCurrent];
}

s32 CStream::GetLength() const{
    return m_nWriteCurrent - m_nReadCurrent;
}

bool CStream::In(const void * pBuff, s32 nLength) 
{
    if (NULL == pBuff ||
        nLength <= 0 )
    {
        ASSERT(false);
        return false;
    }

    if (nLength > m_nMaxLength)
    {
        //这里要打出日志 提示 消息过长
        ASSERT(false);
        return false;
    }

    if (nLength > m_nReadCurrent + m_nMaxLength - m_nWriteCurrent)
    {
        //提示缓冲区不足
        ASSERT(false);
        return false;
    }

    if (nLength > m_nMaxLength - m_nWriteCurrent)
    {
        if (m_nWriteCurrent != m_nReadCurrent) {
            memcpy(&m_szBuff[m_nWriteCurrent], m_szBuff + m_nReadCurrent, m_nWriteCurrent - m_nReadCurrent);
        }
        m_nWriteCurrent = m_nWriteCurrent - m_nReadCurrent;
        m_nReadCurrent = 0;
    }

    memcpy(&m_szBuff[m_nWriteCurrent], pBuff, nLength);

    m_nWriteCurrent += nLength;

    return true;
}

const char * CStream::Out(s32 & nLength)
{
    u32 nPosition = m_nReadCurrent;
    if (m_nWriteCurrent - m_nReadCurrent < nLength)
    {
        if (STREAM_IS_EMPTY == (m_nWriteCurrent - m_nReadCurrent))
        {
            m_nReadCurrent = 0;
            m_nWriteCurrent = 0;
            return NULL;
        }
        
        nLength = m_nWriteCurrent - m_nReadCurrent;
        nPosition = m_nReadCurrent;
        m_nReadCurrent = 0;
        m_nWriteCurrent = 0;
    } else {
        m_nReadCurrent += nLength;
    }

    return (char *)&m_szBuff[nPosition];// + nPosition;
}

void CStream::Clear()
{
    m_nReadCurrent = 0;
    m_nWriteCurrent = 0;
}