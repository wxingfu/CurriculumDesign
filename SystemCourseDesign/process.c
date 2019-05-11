#include "process.h"
#include"stackSimulator.h"
#include<stddef.h>
#include<string.h>
//用于挂起调度器的变量
static volatile long SchedulerSuspended = (long)FALSE;

void  FindTopProrityProcess()
{
	unsigned int topProrityProcess = MAX_PROCESS_PRIORITY - 1;


	while (listIsEmpty(ProcessReadyList[topProrityProcess])) {

		--topProrityProcess;
	}
	//printf("当前最高优先级：%d\n", topProrityProcess);
	listChangeListItemWithTime(ProcessReadyList[topProrityProcess]);

	TopPriorityReadyProcess = topProrityProcess;
}

void proSELECT_HIGHEST_PRIORITY_PROCESS()
{
	unsigned int uxTopPriority = TopPriorityReadyProcess;
	FindTopProrityProcess();
	listChangeListItemWithTime(ProcessReadyList[uxTopPriority]);
}




//时间片到时切换列表项
void listChangeListItemWithTime(ProcessList * list)
{
	ProcessList* ConstList = (list);
	//printf("切换之前的进程id：%d\n", CurrentPCB_pointer->IDofPCB);
	ConstList->ListItemIndex = ConstList->ListItemIndex->next;
	if (ConstList->ListItemIndex == ConstList->lastItem) {
		ConstList->ListItemIndex = ConstList->ListItemIndex->next;
	}
	CurrentPCB_pointer = ConstList->ListItemIndex->PCB_block;
	//printf("现在执行的进程id：%d\n", CurrentPCB_pointer->IDofPCB);
}

int initStaticLists()
{

	int result = 1;
	ListItem* ProcessReadyListLastItem[MAX_PROCESS_PRIORITY];

	ListItem* ProcessBlockingListLastItem = (ListItem*)malloc(sizeof(ListItem));

	ListItem* ProcessDeleteListLastItem = (ListItem*)malloc(sizeof(ListItem));

	for (int i = 0; i < MAX_PROCESS_PRIORITY; i++) {

		ProcessReadyList[i] = (ProcessList*)malloc(sizeof(ProcessList));

		ProcessReadyListLastItem[i] = (ListItem*)malloc(sizeof(ListItem));

		InitListItem(ProcessReadyListLastItem[i]);

		ProcessReadyList[i]->lastItem = ProcessReadyListLastItem[i];

		InitProcessList(ProcessReadyList[i]);

		SET_LIST_STATE(ProcessReadyList[i], LISTReady);
	}
	ProcessBlockingList = (ProcessList*)malloc(sizeof(ProcessList));

	InitListItem(ProcessBlockingListLastItem);

	ProcessBlockingList->lastItem = ProcessBlockingListLastItem;

	InitProcessList(ProcessBlockingList);


	SET_LIST_STATE(ProcessBlockingList, LISTBlocking);


	ProcessDeleteList = (ProcessList*)malloc(sizeof(ProcessList));

	InitListItem(ProcessDeleteListLastItem);

	ProcessDeleteList->lastItem = ProcessDeleteListLastItem;

	InitProcessList(ProcessDeleteList);

	SET_LIST_STATE(ProcessDeleteList, LISTDelete);

	for (int i = 0; i < MAX_PROCESS_PRIORITY; i++) {

		if (ProcessReadyList[i] == NULL) {

			result = 0;

		}
	}
	if (ProcessBlockingList == NULL || ProcessDeleteList == NULL || ProcessBlockingListLastItem == NULL
		|| ProcessDeleteListLastItem == NULL) {
		result = 0;
	}
	/*if (result == 0) {

		freeStaticLists();
	}
	else {
		result = 1;
	}*/
	return result;
}

void freeStaticLists()
{
	for (int i = 0; i < MAX_PROCESS_PRIORITY; i++) {

		free(ProcessReadyList[i]);
	}

	free(ProcessBlockingList);

	free(ProcessDeleteList);

}

int CreateNewProcess(ProcessFunction_t function, const char * const name, const unsigned int stackLength,
	void * const parameters, unsigned int prority, PCB**pcb, time_t runTime)
{
	PCB_t* newPCB;
	int createResult;

	newPCB = myMalloc(sizeof(PCB_t));
	if (newPCB != NULL) {
		if (addPcbToStack(newPCB, parameters) == 0) {
			myFree(newPCB);

			newPCB = NULL;
			createResult = 0;
		}
		else {

			InitialNewProcess(function, name, stackLength, parameters, prority, newPCB, runTime);

			addProcessToReadyList(newPCB);


			newPCB->status = READY;

			createResult = 1;

		}

	}

	*pcb = newPCB;

	return createResult;
}

void InitialNewProcess(ProcessFunction_t function, const char * const name,
	const unsigned int stackLength, void * const parameters, unsigned int prority, PCB * pcb, time_t runTime)
{

	pcb->function = function;
	/*for (int i = 0; i != '\0'; i++) {
		if (19 == i) {
			(pcb->PCBname)[i] = name[i];
			break;
		}
		pcb->PCBname[i] = name[i];
	}
	pcb->PCBname[MAX_NAME_LENGTH - 1] = '\0';*/
	strcpy_s(pcb->PCBname, MAX_NAME_LENGTH, name);

	pcb->stackAddress.length = stackLength;

	if (prority >= MAX_PROCESS_PRIORITY || prority < 0) {
		printf("进程优先级错误");
		pcb->processPriority = 0;
	}
	else {
		pcb->processPriority = prority;
	}

	pcb->IDofPCB = pcb->stackPosition;
	//printf("进程id：%d\n", pcb->IDofPCB);
	//初始化堆栈中的进程任务初始参数
	pcb->runTime = runTime;

}



void addProcessToReadyList(PCB_t * newPcb)
{

	int prority = newPcb->processPriority;
	//当前进程指针是否为空
	if (CurrentPCB_pointer == NULL) {
		CurrentPCB_pointer = newPcb;
		//当前进程数量为0
		if (CurrentProcessNumer == 0 && ProcessReadyList == NULL) {
			int initResult = initStaticLists();
			//初始化静态全局列表失败
			if (initResult == 0) {
				printf("就绪列表初始化错误\n");
				return;
			}
			//初始化成功
			else {
				ListItem* newListItem = myMalloc(sizeof(ListItem));

				InitListItem(newListItem);

				newListItem->PCB_block = newPcb;

				newPcb->hostItem = newListItem;

				SET_priorityValue(newListItem, newPcb->processPriority);

				InsertItemIntoProcessList(newListItem, ProcessReadyList[prority]);

				//newListItem->PCB_block.status = READY;
				if (newPcb->processPriority >= TopPriorityReadyProcess) {
					TopPriorityReadyProcess = newPcb->processPriority;

				}
				//printf("%d %d", newPcb->processPriority, TopPriorityReadyProcess);
				CurrentProcessNumer++;
			}
		}
		//进程数量不为零
		else {

			ListItem* newListItem = myMalloc(sizeof(ListItem));

			InitListItem(newListItem);

			newListItem->PCB_block = newPcb;

			newPcb->hostItem = newListItem;

			SET_priorityValue(newListItem, newPcb->processPriority);

			InsertItemIntoProcessList(newListItem, ProcessReadyList[prority]);

			//newListItem->PCB_block.status = READY;
			if ((newPcb->processPriority) >= TopPriorityReadyProcess) {
				TopPriorityReadyProcess = newPcb->processPriority;
			}
			//printf("%d %d", newPcb->processPriority, TopPriorityReadyProcess);
			CurrentProcessNumer++;
		}
	}
	//进程指针不为空
	else {
		//调度器没有执行
		if (schdulerStatus == SCHEDULER_STOP) {
			//新建的进程优先级比当前进程优先级高
			//切换新建进程为当前进程
			if (newPcb->processPriority >= CurrentPCB_pointer->processPriority) {
				TopPriorityReadyProcess = newPcb->processPriority;
				CurrentPCB_pointer->status = READY;
				CurrentPCB_pointer = newPcb;
			}
			//printf("%d %d", newPcb->processPriority, TopPriorityReadyProcess);
		}
		else {
			if (newPcb->processPriority >= CurrentPCB_pointer->processPriority) {
				//打断调度器，切换任务进程
				schedulerStopAll();
				TopPriorityReadyProcess = newPcb->processPriority;
				CurrentPCB_pointer->status = READY;
				CurrentPCB_pointer = newPcb;

				schedulerResume();

			}
		}

		ListItem* newListItem = myMalloc(sizeof(ListItem));

		InitListItem(newListItem);

		newListItem->PCB_block = newPcb;

		newPcb->hostItem = newListItem;

		SET_priorityValue(newListItem, newPcb->processPriority);

		InsertItemIntoProcessList(newListItem, ProcessReadyList[prority]);

		CurrentProcessNumer++;

	}
}

int DeleteProcess(PCB * pcb)
{
	PCB* pcbToDelete = pcb;

	ListItem* hostItemOfpcbToDelete = pcbToDelete->hostItem;
	int preNumber = hostItemOfpcbToDelete->hostList->numberOfProcesses;
	int result = 0;
	//如果要删除的进程是当前进程
	//将当前列表指针指向列表项的下一个
	if (pcbToDelete == CurrentPCB_pointer) {
		if (pcbToDelete->hostItem->hostList->numberOfProcesses == 1) {
			CurrentPCB_pointer = NULL;
		}
		else {
			CurrentPCB_pointer = pcbToDelete->hostItem->hostList->lastItem->next->PCB_block;
		}
		CurrentProcessNumer--;
	}
	else {
		CurrentProcessNumer--;
	}

	if (DeleteFromList(hostItemOfpcbToDelete) != (preNumber - 1)) {
		printf("进程删除失败\n");
		result = 0;
	}
	else {
		if (0 == deletePcbFromStack(pcb->IDofPCB)) {
			printf("堆栈清除失败\n");
			return 0;
		}



		result = 1;
		myFree(pcb);
	}

	return result;
}

int BlockedProcess(int pcbID)
{
	//找到要阻塞的PCB
	PCB_t*pcbToBlock = findPCB_ById(pcbID);
	//对应的优先级
	int prority = pcbToBlock->processPriority;

	if (0 == ProcessReadyList[prority]->numberOfProcesses) {
		//printf("进程已经退出！\n");
		return 0;
	}

	//ListItem* ItemOfRemoveToBlockList = pcbToBlock->hostItem;
	//printf("控制块优先级：%d,列表项优先级:%d",pcbToBlock->processPriority,ItemOfRemoveToBlockList->priorityValue);
	/*printf("进程数量：%d",ItemOfRemoveToBlockList->hostList->numberOfProcesses);*/
	//ENTER_CRITICAL();
	else {
		DeleteFromList(pcbToBlock->hostItem);
		InsertItemIntoProcessList(pcbToBlock->hostItem, ProcessBlockingList);
		blocking_signal = 1;
		pcbToBlock->status = BLOCKING;



		return 1;
	}
	//EXIT_CRITICAL();

}

int WakeupProcess(int pcbID)
{
	//找到要唤醒的PCB
	PCB_t*pcbToReady = findPCB_ById(pcbID);
	int prority = pcbToReady->processPriority;
	//找到相应的列表项
	//ListItem* ItemOfRemoveToReadList = pcbToReady->hostItem;
	//ENTER_CRITICAL();
	{
		DeleteFromList(pcbToReady->hostItem);
		InsertItemIntoProcessList(pcbToReady->hostItem, ProcessReadyList[prority]);
		pcbToReady->status = READY;
	}
	//EXIT_CRITICAL();
	return 1;
}

void schedulerStopAll(void)
{
	//等待中断信号量，相当于关中断
	//WaitForSingleObject(timeInterruptMutex, INFINITE);
	WaitForSingleObject(modifyListMutex, INFINITE);

}

void schedulerResume(void)
{
	ReleaseMutex(modifyListMutex);
}


void * myMalloc(size_t newSize)
{
	void*parameterOfReturn;

	schedulerStopAll();

	parameterOfReturn = malloc(newSize);

	(void)schedulerResume();

	return parameterOfReturn;
}

void myFree(void*pointer) {
	if (NULL != pointer) {
		schedulerStopAll();
		free(pointer);
		(void)schedulerResume();
	}

}


void processSwitchContext()
{
	if (schdulerStatus != SCHEDULER_STOP) {
		xYieldPending = TRUE;
	}
	else {
		xYieldPending = FALSE;
		proSELECT_HIGHEST_PRIORITY_PROCESS();
	}
}

//进程处理函数(执行进程函数)
DWORD WINAPI processThreadFun(LPVOID param)
{


	FindTopProrityProcess();
	//printf("现在最高的优先级：%d\n",TopPriorityReadyProcess);
	//printf("现在执行的进程：%s\n", CurrentPCB_pointer->PCBname);
	int*value = NULL;

	//PCB_t*current = CurrentPCB_pointer;

	value = (int*)findFunValueByPcbID(CurrentPCB_pointer->IDofPCB);
	//printf("找到的值：%d\n",value);
	//printf("进程：%d\n",CurrentPCB_pointer->IDofPCB);
	//执行进程的函数
	//！！！想要记录进程函数的值 需要在进程函数的内部在模拟的堆栈中记录(因为在进程工作函数是没有返回值的)
	CurrentPCB_pointer->status = RUNNING;
	(CurrentPCB_pointer->function)(value);


}

void startScheduler()
{

	//创建一个进程 模拟调度器
	processThread = CreateThread(NULL, 0, processThreadFun, NULL, 0, NULL);

	for (;;) {
		WaitForSingleObject(INTERRUPTION, INFINITE);
		//WaitForSingleObject(toKillProcessThread,INFINITE);


		if (exit_signal == TRUE) {

			//printf("要删除的进程：%s\n",(*processExitBuf)->pcb->PCBname);
			DeleteProcess((*processExitBuf)->pcb);

			//OSstackSimulatorItem*iter = (*STATIC_OS_STACK)->startSimulatorItem->next;
			//ListItem*item = ProcessReadyList[6]->lastItem->next;
			//for (int i = 0;; i++) {
			//	if (iter == ProcessReadyList[6]->lastItem) {
			//		break;
			//	}
			//	//printf("在列表中的进程名称:%s\n", ((PCB*)item->PCB_block)->PCBname);
			//	printf("进程函数值是否为空:%s\n",iter->functionValue==NULL);
			//	iter = iter->next;
			//	

			//FindTopProrityProcess();

			exit_signal = FALSE;
		}
		TerminateThread(processThread, 0);
		//ReleaseMutex(modifyListMutex);
		//ReleaseMutex(timeInterruptMutex);
		if (CurrentPCB_pointer != NULL) {
			CurrentPCB_pointer->status = READY;
		}
		if (blocking_signal = 1 && CurrentPCB_pointer != NULL) {
			CurrentPCB_pointer->status = BLOCKING;
			blocking_signal = 0;
		}
		processThread = CreateThread(NULL, 0, processThreadFun, NULL, 0, NULL);

	}
}

void runInFreeTime(void*a) {
	while (1) {
		Sleep(tickTime / 2);
		//printf("系统当前空闲\n");

	}
}

int listIsEmpty(ProcessList * list)
{
	if (list->numberOfProcesses == 0) return 1;
	else return 0;
}
