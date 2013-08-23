#ifndef STREAM_H
#define STREAM_H

#include "MultiSys.h"
#include "CLock.h"

#define STREAM_LENGTH 10240
#define STREAM_IS_EMPTY 0

class CStream
{
public:
    CStream();
    ~CStream();
    void Clear();
    const char * GetBuff() const;
    s32 GetLength() const;
    bool In(const void * pBuff, s32 nLength);
    const char * Out(s32 & nLength);
private:
    char * m_szBuff[STREAM_LENGTH];
    s32 m_nReadCurrent;
    s32 m_nWriteCurrent;
    s32 m_nMaxLength;
};

#endif