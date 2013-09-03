#ifndef STREAM_H
#define STREAM_H

#include "MultiSys.h"
#include "CLock.h"

#define STREAM_SIZE 10240

class CStream
{
public:
    CStream();
    ~CStream();
    void clear();
    const char * buff() const;
    s32 size() const;
    void in(const void * pbuff, s32 size);
    void out(s32 size);

private:
    void malloc_double();
    void half_free();

private:
    CLockUnit m_rlock;
    CLockUnit m_wlock;
    char * m_pbuff;
    s32 m_read;
    s32 m_write;
    s32 m_max;
};

#endif