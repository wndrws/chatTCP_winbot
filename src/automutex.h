#pragma once

#include <windows.h>
#include <assert.h>

// класс-оболочка, создающий и удаляющий мютекс (Windows)
class CAutoMutex {
    // дескриптор создаваемого мютекса
    HANDLE m_h_mutex;

    // запрет копирования
    CAutoMutex(const CAutoMutex&);
    CAutoMutex& operator=(const CAutoMutex&);
public:
    CAutoMutex() {
        m_h_mutex = CreateMutex(NULL, FALSE, NULL);
        assert(m_h_mutex);
    }
    ~CAutoMutex() { CloseHandle(m_h_mutex); }
    HANDLE get() { return m_h_mutex; }
};

// класс-оболочка, занимающий и освобождающий мютекс
class CMutexLock
{
    HANDLE m_mutex;

    // запрещаем копирование
    CMutexLock(const CMutexLock&);
    CMutexLock& operator=(const CMutexLock&);
public:
    // занимаем мютекс при конструировании объекта
    CMutexLock(HANDLE mutex): m_mutex(mutex) {
        const DWORD res = WaitForSingleObject(m_mutex, INFINITE);
        assert(res == WAIT_OBJECT_0);
    }
    // освобождаем мютекс при удалении объекта
    ~CMutexLock() {
        const BOOL res = ReleaseMutex(m_mutex);
        assert(res);
    }
};

// макрос, занимающий мютекс до конца области действия
#define SCOPE_LOCK_MUTEX(hMutex) CMutexLock _tmp_mtx_capt(hMutex);