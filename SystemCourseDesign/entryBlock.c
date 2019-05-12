#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <mmsystem.h>

#include "stackSimulator.h"
#include "process.h"

//最大运行时间
#define MAX_RUN_TIME 2000000000


//声明打印信息函数
void printInformation();


//任务函数
void test(void*a)
{
	//随机种子
	srand((unsigned)time(NULL));

	int count = a;

	while (count++)
	{
		//计时，用于模拟时间片
		clock_t start = clock();

		//随机[100,150]s睡眠
		Sleep(rand() % 100 + 50);

		//查找堆栈中正在运行的进程，目的是获取任务函数的返回值
		OSstackSimulatorItem_t*placeOfValue = findRunningItem();

		//printf("test现在执行的进程:%s\n",placeOfValue->pcb->PCBname);
		//printf("进程的运行结果(将被写会内存):%d\n",count);
		if (placeOfValue != NULL)
		{
			ENTER_CRITICAL();
			{
				//更新任务函数参数值
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


//任务函数1
void test1(void*a)
{
	while (1)
	{
		Sleep(200);
		printf("process2 is running...\n");
	}
}


//任务函数0
void test0(void*a)
{
	while (1)
	{
		Sleep(200);
		printf("process3 is running...\n");
	}
}


//菜单显示函数
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


//打印信息函数实现
void printInformation()
{
	//获取堆栈迭代器
	OSstackSimulatorItem * iter = (*STATIC_OS_STACK)->startSimulatorItem;

	//循环打印进程信息
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
	//初始化堆栈
	initOSstackSimulator();

	//初始化静态链表
	initStaticLists();

	//初始化信号量
	initSemphores();

	//设置退出信号
	exit_signal = FALSE;

	//设置阻塞信号
	blocking_signal = 0;

	//分配进程终止时的空间
	processExitBuf = malloc(sizeof(EXIT_PROCESS));

	//分配指向退出进程函数的指针的空间
	(*processExitBuf) = (EXIT_PROCESS*)malloc(sizeof(EXIT_PROCESS));

	//初始进程数
	CurrentProcessNumer = 0;

	//初始就绪进程的最高优先级
	TopPriorityReadyProcess = 30;

	//初始进程指针的内存空间
	PCB_t **freeProcess = malloc(sizeof(PCB));

	//系统进程名称
	char name3[MAX_NAME_LENGTH] = "FreeProcess";

	//创建系统进程
	CreateNewProcess(runInFreeTime, name3, 1, NULL, 0, freeProcess, INFINITE);

	//创建定时器
	CreateTimer();

	//调用菜单函数
	HANDLE display = CreateThread(NULL, 0, displayFun, NULL, 0, NULL);

	//启动调度器
	startScheduler();

	//释放内存
	free(*processExitBuf);
	free(processExitBuf);
	freeStaticLists();

	system("pause");
	return 0;
}
