#include "ticks.h"


//初始化信号量
void initSemphores()
{
	//时钟中断互斥量
	timeInterruptMutex = CreateMutex(NULL, TRUE, NULL);

	//修改链表的互斥量
	modifyListMutex = CreateMutex(NULL, TRUE, NULL);

	//时钟数的互斥量
	tickCountMutex = CreateMutex(NULL, TRUE, NULL);

	//中断信号量
	INTERRUPTION = CreateSemaphore(NULL, 0, 1, NULL);

	//调度互斥量
	schedulerMutex = CreateMutex(NULL, TRUE, NULL);

	//互斥量的释放
	ReleaseMutex(timeInterruptMutex);
	ReleaseMutex(modifyListMutex);
	ReleaseMutex(tickCountMutex);
	ReleaseMutex(schedulerMutex);
}


//计数器线程执行函数
DWORD WINAPI startTimer(LPVOID param)
{
	tickCount = 0;

	while (1)
	{
		Sleep(tickTime);

		checkTickCountOverflow();

		WaitForSingleObject(tickCountMutex, INFINITE);

		//时钟加一
		tickCount++;

		ReleaseMutex(tickCountMutex);

		//获得修改时钟中断信号的信号量，用于在进程进入临界区等情况下关中断
		WaitForSingleObject(timeInterruptMutex, INFINITE);

		ReleaseSemaphore(INTERRUPTION, 1, NULL);

		ReleaseMutex(timeInterruptMutex);
	}
}


//判断时钟计数是否溢出
BOOL checkTickCountOverflow()
{
	BaseType_t currentTickCount = 0;

	BOOL result;

	//获得访问时钟计数的信号量
	WaitForSingleObject(tickCountMutex, INFINITE);

	currentTickCount = tickCount;

	//如果溢出返回
	if ((currentTickCount++) < currentTickCount)
	{
		result = FALSE;
		tickCount = 0;
	}
	else {
		result = TRUE;
	}

	ReleaseMutex(tickCountMutex);

	return result;

}


//创建计数器
void CreateTimer() {

	timerThread = CreateThread(NULL, 0, startTimer, NULL, 0, NULL);
}


//进入修改列表数据结构临界区
void enter_list_critical()
{
	//通过获得时钟信号互斥锁达到关中断的目的
	WaitForSingleObject(timeInterruptMutex, INFINITE);

	//修改列表的互斥锁
	WaitForSingleObject(modifyListMutex, INFINITE);
}


//退出修改数据结构临界区
void exit_list_critical()
{
	ReleaseMutex(modifyListMutex);

	ReleaseMutex(timeInterruptMutex);
}
