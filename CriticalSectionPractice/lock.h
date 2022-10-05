#ifndef ThreadsH
#define ThreadsH
//-----------------------------------------------------------------------------
#include <windows.h>
//-----------------------------------------------------------------------------
// 這個class只是簡單地將Critical Section API包裝成一個class，沒什麼特別
class Mutex
{

public:
    Mutex()
    {
        ::InitializeCriticalSection(&_mtx);
    }
    ~Mutex()
    {
        ::DeleteCriticalSection(&_mtx);
    }
    void Lock()
    {
        ::EnterCriticalSection(&_mtx);
    }
    void Unlock()
    {
        ::LeaveCriticalSection(&_mtx);
    }
private:
    mutable CRITICAL_SECTION _mtx;
};

// 這個class利用了destructor，使得LeaveCriticalSection在其使用函式結束時會被自動呼叫
template < class Host, class MutexPolicy = Mutex >
class ObjectLevelLockable
{
private:
    mutable MutexPolicy _mtx;
public:
    ObjectLevelLockable() : _mtx() {}
    ObjectLevelLockable(const ObjectLevelLockable&) : _mtx() {}
    ~ObjectLevelLockable() {}

    class Lock;
    friend class Lock;

    class Lock
    {
    public:
        explicit Lock(const ObjectLevelLockable& host) : _host(host)
        {
            _host._mtx.Lock();
        }
        explicit Lock(const ObjectLevelLockable* host) : _host(*host)
        {
            _host._mtx.Lock();
        }
        ~Lock()
        {
            _host._mtx.Unlock();
        }
    private:
        Lock();
        Lock(const Lock&);
        Lock& operator=(const Lock&);
        const ObjectLevelLockable& _host;
    };
};
//-----------------------------------------------------------------------------
// 這個class將整個Mutex鎖變成全域的，只要是繼承它的子class
// 無論該class產生了幾個object實例，在呼叫function時都會參考到同一個鎖
template <class Host, class MutexPolicy = Mutex >
class ClassLevelLockable
{
private:
    class Initializer
    {
    public:
        Initializer() : _init(true), _mtx()
        {
        }
        ~Initializer()
        {
        }
    public:
        bool         _init;
        MutexPolicy  _mtx;
    };

    static Initializer _initializer;

public:
    class Lock;
    friend class Lock;

    class Lock
    {
    public:
        Lock()
        {
            _initializer._mtx.Lock();
        }
        explicit Lock(const ClassLevelLockable&)
        {
            _initializer._mtx.Lock();
        }
        explicit Lock(const ClassLevelLockable*)
        {
            _initializer._mtx.Lock();
        }
        ~Lock()
        {
            _initializer._mtx.Unlock();
        }
    private:
        Lock(const Lock&);
        Lock& operator=(const Lock&);
    };
};
template < class Host, class MutexPolicy >
typename ClassLevelLockable< Host, MutexPolicy >::Initializer
ClassLevelLockable< Host, MutexPolicy >::_initializer;
//-----------------------------------------------------------------------------
#endif