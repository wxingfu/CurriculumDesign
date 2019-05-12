#ifndef TICK_H
#define TICK_H


#include <Windows.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>


//时钟计数器
long volatile tickCount;

//基本类型
typedef long BaseType_t;

//时钟节拍类型
typedef uint32_t TickType_t;

//堆栈类型
typedef unsigned int StackType_t;

//开启中断信号量
HANDLE timeInterruptMutex;

//更改进程列表或者全局数据结构的信号量
HANDLE modifyListMutex;

//时钟计数器信号量
HANDLE tickCountMutex;

//中断信号量
HANDLE INTERRUPTION;

//杀死线程的信号量
HANDLE toKillProcessThread;

//调度器阻塞信号量
HANDLE schedulerMutex;

//初始化信号量
void initSemphores();

//进程模拟工作线程
HANDLE processThread;

//时钟工作线程
HANDLE timerThread;

//时钟周期
#define tickTime 500

//进入临界区
#define ENTER_CRITICAL() enter_list_critical() 

//退出临界区
#define EXIT_CRITICAL() exit_list_critical()

//计数器线程执行函数
DWORD WINAPI startTimer(LPVOID param);

//检查计数器是否溢出
BOOL checkTickCountOverflow();

//创建计数器
void CreateTimer();

//进入修改列表数据结构临界区
void enter_list_critical();

//退出修改数据结构临界区
void exit_list_critical();



#endif // !TICK_H
