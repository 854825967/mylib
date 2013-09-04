#include <string>
#include "CStream.h"

CStream::CStream() {
    m_pbuff = NEW char[STREAM_SIZE];
    memset(m_pbuff, 0, STREAM_SIZE);
    m_read = 0;
    m_write = 0;
    m_max = STREAM_SIZE;
}

CStream::~CStream() {
    if (m_pbuff != NULL) {
        delete m_pbuff;
        m_pbuff = NULL;
    }
}
void CStream::clear() {
    m_read = 0;
    m_write = 0;
    if (m_max > STREAM_SIZE) {
        if (m_pbuff != NULL) {
            delete m_pbuff;
            m_pbuff = NULL;
        }
        m_pbuff = NEW char[STREAM_SIZE];
        m_max = STREAM_SIZE;
    }
    memset(m_pbuff, 0, STREAM_SIZE);
}

const char * CStream::buff() const {
    return &m_pbuff[m_read];
}

s32 CStream::size() const {
    return m_write - m_read;
}

void CStream::malloc_double() {
    CAutoLock rlock(&m_rlock);
    s32 cursize = m_write - m_read;
    m_max *= 2;
    char * ptemp = m_pbuff;
    m_pbuff = NEW char[m_max];
    memcpy(m_pbuff, &ptemp[m_read], cursize);
    delete ptemp;
    m_write = cursize;
    m_read = 0;
}

void CStream::half_free() {
    if (m_max > STREAM_SIZE) {
        s32 cursize = m_write - m_read;
        if (m_max > 4 * cursize) {
            CAutoLock wlock(&m_wlock);
            CAutoLock rlock(&m_rlock);
            m_max /= 2;
            char * ptemp = m_pbuff;
            m_pbuff = NEW char[m_max];
            memcpy(m_pbuff, &ptemp[m_read], cursize);
            delete ptemp;
            m_write = cursize;
            m_read = 0;
        }
    }
}

void CStream::in(const void * pbuff, s32 size) {
    CAutoLock wlock(&m_wlock);
    if (size > m_max - m_write) {
        s32 cursize = m_write - m_read;
        if (m_max - cursize < size) {
            while (m_max - cursize < size) {
                malloc_double();
            }
        } else {
            CAutoLock rlock(&m_rlock);
            memcpy(m_pbuff, &m_pbuff[m_read], cursize);
            m_write = cursize;
            m_read = 0;
        }
    }

    memcpy(&m_pbuff[m_write], pbuff, size);
    m_write += size;
    wlock.Free();

    half_free();
}

void CStream::out(s32 size) {
    CAutoLock rlock(&m_rlock);
    if (size > m_write - m_read) {
        ASSERT(false);
        m_read = m_write;
    } else {
        m_read += size;
    }
    rlock.Free();
    half_free();
}
