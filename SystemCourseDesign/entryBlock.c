#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <mmsystem.h>

#include "stackSimulator.h"
#include "process.h"

#define MAX_RUN_TIME 2000000000


void test(void*a)
{
	srand((unsigned)time(NULL));

	int count = a;

	while (count++)
	{
		clock_t start = clock();

		Sleep(rand() % 100 + 50);

		OSstackSimulatorItem_t*placeOfValue = findRunningItem();
		//printf("test现在执行的进程:%s\n",placeOfValue->pcb->PCBname);
		//printf("进程的运行结果(将被写会内存):%d\n",count);
		if (placeOfValue != NULL)
		{
			ENTER_CRITICAL();
			{
				placeOfValue->functionValue = count;
				//printf("更新完成\n");
				//printf("%d\n",(*STATIC_OS_STACK)->startSimulatorItem->next->functionValue);
			}
			EXIT_CRITICAL();

			//ReleaseSemaphore(toKillProcessThread,1,NULL);
			clock_t end = clock() - start;

			placeOfValue->pcb->runTime -= end;

			if (placeOfValue->pcb->runTime < 0)
			{
				(*processExitBuf)->pcb = placeOfValue->pcb;
				exit_signal = TRUE;
				return;
			}
		}
		//printf("剩余时间:%d\n", placeOfValue->pcb->runTime);
	}
}



void test1(void*a)
{

	while (1)
	{
		Sleep(200);
		printf("process2 is running...\n");
	}
}



void test0(void*a)
{
	while (1)
	{
		Sleep(200);
		printf("process3 is running...\n");
	}
}


void printInformation();


DWORD WINAPI displayFun(LPVOID param)
{
	int choice;
	char name[MAX_NAME_LENGTH];
	int paramter;
	int prority;
	time_t runTime;
	PCB_t**pcbToCreate;
	int pcbIdToBlock = -1;
	int pcbIdToWakeUp = -1;
	int pcbIdToDelete = -1;
	PCB*pcbTemp;
	while (1)
	{
		printf("|--------------------------------------------------------------------------------------------------|\n");
		printf("|请选择您要进行的操作：(1)创建进程 (2)阻塞进程 (3)唤醒进程 (4)终止进程 (5)显示进程信息 (0)退出程序|\n");
		scanf_s("%d", &choice);
		switch (choice) {
		case 0:
			exit(0);
		case 1:
			printf("|-------------------------------------------创建进程------------------------------------------------|\n");
			printf("|进程名称：");
			scanf_s("%s", name, MAX_NAME_LENGTH);
			printf("\n|进程函数初始参数(数字)：");
			scanf_s("%d", &paramter);
			printf("\n|进程优先级(0-34)：");
			scanf_s("%d", &prority);
			printf("\n|程序运行时间:");
			scanf_s("%lld", &runTime);
			pcbToCreate = (PCB_t**)malloc(sizeof(PCB_t));
			CreateNewProcess(test, name, 1, (int*)paramter, prority, pcbToCreate, runTime*CLOCKS_PER_SEC);
			printf("|创建成功!\n");
			break;
		case 2:
			printf("|-------------------------------------------阻塞进程----------------------------------------------|\n");
			printf("|阻塞的进程ID：");
			scanf_s("%d", &pcbIdToBlock);
			BlockedProcess(pcbIdToBlock);
			printf("|阻塞成功！\n");
			break;
		case 3:
			printf("|-------------------------------------------唤醒进程----------------------------------------------|\n");
			printf("|要唤醒的进程ID:");
			scanf_s("%d", &pcbIdToWakeUp);
			WakeupProcess(pcbIdToWakeUp);
			printf("|唤醒成功！\n");
			break;
		case 4:
			printf("|-------------------------------------------终止进程-----------------------------------------------|\n");
			printf("|要终止的进程ID:");
			scanf_s("%d", &pcbIdToDelete);
			pcbTemp = findPCB_ById(pcbIdToDelete);
			DeleteProcess(pcbTemp);
			printf("|终止成功！\n");
			break;
		case 5:
			printf("|------------------------------------------显示进程信息--------------------------------------------|\n");
			printf("进程名称\t进程ID\t进程状态\t进程优先级\t进程运行剩余时间\t进程当前结果\n");
			printInformation();
			break;
		default:
			printf("输入有误，请重新输入!\n");
		}
	}
}



void printInformation()
{

	OSstackSimulatorItem * iter = (*STATIC_OS_STACK)->startSimulatorItem;

	while (1)
	{
		iter = iter->next;
		if (iter == (*STATIC_OS_STACK)->startSimulatorItem)
		{
			break;
		}
		if (iter->pcb != NULL)
		{
			printf("  %s  \t\t  %d  \t %d \t\t   %d   \t\t      %d   \t%d\n",
				iter->pcb->PCBname, iter->pcb->IDofPCB, iter->pcb->status,
				iter->pcb->processPriority, iter->pcb->runTime, (int)iter->functionValue);
		}
	}
	printf("\n");
}




int main()
{
	//完成一些初始化
	initOSstackSimulator();

	initStaticLists();

	initSemphores();

	exit_signal = FALSE;

	blocking_signal = 0;

	processExitBuf = malloc(sizeof(EXIT_PROCESS));

	(*processExitBuf) = (EXIT_PROCESS*)malloc(sizeof(EXIT_PROCESS));

	CurrentProcessNumer = 0;

	TopPriorityReadyProcess = 30;

	PCB_t **freeProcess = malloc(sizeof(PCB));

	char name3[MAX_NAME_LENGTH] = "FreeProcess";

	CreateNewProcess(runInFreeTime, name3, 1, NULL, 0, freeProcess, INFINITE);

	CreateTimer();

	HANDLE display = CreateThread(NULL, 0, displayFun, NULL, 0, NULL);

	startScheduler();

	free(*processExitBuf);

	free(processExitBuf);

	freeStaticLists();

	system("pause");
	return 0;
}
