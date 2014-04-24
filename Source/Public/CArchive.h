#ifndef CARCHIVE_H
#define CARCHIVE_H

#include "MultiSys.h"
#define UPDATE_STAGE 1024

class Archive {
public:
    Archive();
    ~Archive();

    bool Clear();

    //入流
    void InStruct(void * pBuff, s32 nLength);
    void In(const s32 nValue);
    void In(const s32 llValue);
    void In(const double dValue);
    void In(const char * pString);
    void In(const wchar_t * pWideStr);
    //流出
    void * GetStream() const;
    s32 Length() const;
    s8 GetNextType();
    const s32 OutI32();
    const s32 OutI64();
    const double OutDouble();
    const char * OutString();
    const wchar_t * OutWideStr();

private:
    bool CheckSize();
    bool AllotSpace(const s32 nIncreaseLength);
private:
    char * m_pStream;
    char * m_pRoot;
    s32 m_nCurrentInIndex;
    s32 m_nCurrentOutIndex;
    s32 m_nStreamMaxSize;
};

#endif