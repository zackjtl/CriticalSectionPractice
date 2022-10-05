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
    
    // 併發8個thread對同一個變數 (ClassA裡的_resource)做加減運算
    for (int i = 0; i < 8; ++i) {
        // 第三個參數原本是要傳入一個無形別指標(LPVOID)，但我們利用它來傳一個數字(int)，因為指標本身就是一個整數，我們傳入一個數字編譯器並不會阻擋
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
        Lock lock(*this); // !! 可以試看看把這行拿掉的結果有何不同 !!
        
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

// 產生ClassA的全域變數
ClassA* g_Ca = new ClassA();

DWORD WINAPI thread_proc(LPVOID lpParameter)
{
    //ClassA ca;
    time_t t;

    int threadIdx = (int)lpParameter;
                
    // 在rand之前要呼叫srand，使得每次執行的亂數值都不一樣，如果不呼叫srand，每個thread的rand取得的值都一樣
    // 用傳進來的index當作random seed，也可以如下行用時間當作seed，但由於CPU執行太快，每個thread的time()所得到的時間可能會都一樣，而用index可以確保不一樣
    // srand((unsigned)time(&t));
    
    srand(threadIdx);

    int value = rand();
    int res = 0;
    bool sign = ((rand() % 100 + 1) - (rand() % 100 + 1)) > 50 ? true : false;
    if (sign) {        
        value = -value;        
    }    

    g_Ca->add(value);

    return 0;
}