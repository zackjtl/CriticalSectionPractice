// CriticalSectionPractice.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <Windows.h>
#include <time.h>
#include "lock.h"

DWORD WINAPI thread_proc(LPVOID lpParameter);

int main()
{
    std::cout << "Hello World!\n";

    DWORD threadID[8];
    HANDLE threadHandle[8];
    
    for (int i = 0; i < 8; ++i) {
        threadHandle[i] = CreateThread(0, 0, thread_proc, (LPVOID)i, 0, & threadID[i]);
    }

    system("PAUSE");
}

class ClassA : public ObjectLevelLockable<ClassA>
{
public:
    // Constructor
    ClassA()
    {
        _resource = 0;
    }
    ~ClassA()
    {
    }
    void add(int value)
    {
        Lock lock(*this); // 可以試看看把這行拿掉的結果有何不同
        
        if (value < 1)
            printf("resource = %d - %d = %d\n", ClassA::_resource, -value, ClassA::_resource + value);
        else
            printf("resource = %d + %d = %d\n", ClassA::_resource, value, ClassA::_resource + value);

        ClassA::_resource += value;
    }

    // 讀取最新的resource值
    int get_resource_val() const
    {
        return _resource;
    }

public:
    int _resource;
};


// 寫在class裡的static變數只是宣告，必須要在外面寫一次才會變實體
// Static變數的功能和Global變數一樣
//int ClassA::_resource = 0;

ClassA* g_Ca = new ClassA();

DWORD WINAPI thread_proc(LPVOID lpParameter)
{
    // ClassA不再是全域變數，而是隨著thread產生的區域變數，因此可能同時有很多個

    //ClassA ca;
    time_t t;

    int threadIdx = (int)lpParameter;

    //srand((unsigned)time(&t));

    srand((int)lpParameter);

    int value = rand();
    int res = 0;
    bool sign = ((rand() % 100 + 1) - (rand() % 100 + 1)) > 50 ? true : false;
    if (sign) {        
        value = -value;        
    }    

    g_Ca->add(value);

    return 0;
}