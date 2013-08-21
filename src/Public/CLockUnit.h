#ifndef CLOCKUNIT_H
#define CLOCKUNIT_H

#if defined WIN32 || defined WIN64
class CLockUnit {
public:
    CLockUnit() {
        ::InitializeCriticalSection(&m_lock);
    }

    ~CLockUnit() {
        ::DeleteCriticalSection(&m_lock);
    }

    void Lock() {
        ::EnterCriticalSection(&m_lock);
    }

    void UnLock() {
        ::LeaveCriticalSection(&m_lock);
    }

    bool TryLock() {
        ::TryEnterCriticalSection(&m_lock);
    }

private:
    CRITICAL_SECTION m_lock;
};
#endif //#if defined WIN32 || defined WIN64

#ifdef linux
class CLockUnit {
public:
    CLockUnit() {

    }

    ~CLockUnit() {

    }

    void Lock() {

    }

    void UnLock() {

    }

    bool TryLock() {

    }

private:
    //CRITICAL_SECTION m_lock;
};
#endif //linux

#endif //CLOCKUNIT_H
