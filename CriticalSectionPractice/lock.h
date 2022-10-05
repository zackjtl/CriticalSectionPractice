#ifndef ThreadsH
#define ThreadsH
//-----------------------------------------------------------------------------
#include <windows.h>
//-----------------------------------------------------------------------------
// �o��class�u�O²��a�NCritical Section API�]�˦��@��class�A�S����S�O
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

// �o��class�Q�ΤFdestructor�A�ϱoLeaveCriticalSection�b��ϥΨ禡�����ɷ|�Q�۰ʩI�s
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
// �o��class�N���Mutex���ܦ����쪺�A�u�n�O�~�ӥ����lclass
// �L�׸�class���ͤF�X��object��ҡA�b�I�sfunction�ɳ��|�ѦҨ�P�@����
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