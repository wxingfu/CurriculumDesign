#include"ticks.h"
#include<stdio.h>

void initSemphores()
{
	timeInterruptMutex = CreateMutex(NULL, TRUE, NULL);

	modifyListMutex = CreateMutex(NULL, TRUE, NULL);

	tickCountMutex = CreateMutex(NULL, TRUE, NULL);

	INTERRUPTION = CreateSemaphore(NULL, 0, 1, NULL);
	//INTERRUPTION = CreateMutex(NULL, TRUE, NULL);

	schedulerMutex = CreateMutex(NULL, TRUE, NULL);

	//toKillProcessThread = CreateSemaphore(NULL, 0, 1, NULL);

	ReleaseMutex(timeInterruptMutex);
	ReleaseMutex(modifyListMutex);
	ReleaseMutex(tickCountMutex);
	ReleaseMutex(schedulerMutex);
	//ReleaseMutex(toKillProcessThread);
}


DWORD WINAPI startTimer(LPVOID param) {
	tickCount = 0;



	while (1) {

		Sleep(tickTime);
		//Sleep(50);
		//printf("\n时钟数:%d\n",tickCount);
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
BOOL checkTickCountOverflow() {

	BaseType_t currentTickCount = 0;

	BOOL result;
	//获得访问时钟计数的信号量
	WaitForSingleObject(tickCountMutex, INFINITE);

	currentTickCount = tickCount;
	//如果溢出返回
	if ((currentTickCount++) < currentTickCount) {

		result = FALSE;

		tickCount = 0;
	}
	else {
		result = TRUE;
	}

	ReleaseMutex(tickCountMutex);

	return result;

}

void CreateTimer() {

	timerThread = CreateThread(NULL, 0, startTimer, NULL, 0, NULL);

}

void enter_list_critical()
{
	//通过获得时钟信号互斥锁达到关中断的目的
	WaitForSingleObject(timeInterruptMutex, INFINITE);
	//修改列表的互斥锁
	WaitForSingleObject(modifyListMutex, INFINITE);


}

void exit_list_critical()
{

	ReleaseMutex(modifyListMutex);
	ReleaseMutex(timeInterruptMutex);
}
