#ifndef CLOCKUNIT_H
#define CLOCKUNIT_H
#include "MultiSys.h"

namespace tlib {
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
    
#if defined CC_TARGET_OS_IPHONE
    class CLockUnit {
    public:
        CLockUnit() {
            pthread_mutex_init(&m_lock, NULL);
        }
        
        ~CLockUnit() {
            pthread_mutex_destroy(&m_lock);
        }
        
        void Lock() {
            pthread_mutex_lock(&m_lock);
        }
        
        void UnLock() {
            pthread_mutex_unlock(&m_lock);
        }
        
        bool TryLock() {
            return pthread_mutex_trylock(&m_lock);
        }
        
    private:
        pthread_mutex_t m_lock;
    };
#endif //CC_TARGET_OS_IPHONE
    
#ifdef linux
    class CLockUnit {
    public:
        CLockUnit() {
            pthread_mutex_init(&m_lock, NULL);
        }
        
        ~CLockUnit() {
            pthread_mutex_destroy(&m_lock);
        }
        
        void Lock() {
            pthread_mutex_lock(&m_lock);
        }
        
        void UnLock() {
            pthread_mutex_unlock(&m_lock);
        }
        
        bool TryLock() {
            return pthread_mutex_trylock(&m_lock);
        }
        
    private:
        pthread_mutex_t m_lock;
    };
#endif //linux
}

#endif //CLOCKUNIT_H
