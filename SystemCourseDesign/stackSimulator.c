#include "stackSimulator.h"



int initOSstackSimulator()
{
	OSstack* newStack = (OSstack*)malloc(sizeof(OSstack));

	OSstackSimulatorItem* newItem = (OSstackSimulatorItem*)malloc(sizeof(OSstackSimulatorItem));

	OSstackSimulatorItem* newItem2 = (OSstackSimulatorItem*)malloc(sizeof(OSstackSimulatorItem));

	if (newStack == NULL || newItem == NULL || newItem2 == NULL)
	{
		return 0;
	}
	else {
		newStack->currentDeepth = 0;

		newStack->totalLength = MAX_STACK_LENGTH;

		newStack->startSimulatorItem = newItem;

		newStack->lastItem = newItem2;

		newStack->startSimulatorItem->pcb = NULL;

		newStack->lastItem->pcb = NULL;

		newStack->startSimulatorItem->next = newStack->startSimulatorItem;

		newStack->lastItem->next = newStack->lastItem;

		newStack->startSimulatorItem->index = 0;

		STATIC_OS_STACK = malloc(sizeof(OSstack));

		*STATIC_OS_STACK = newStack;

		return 1;
	}
}



int addPcbToStack(PCB_t * newPcb, void*const  paramter)
{
	if ((*STATIC_OS_STACK)->currentDeepth == MAX_STACK_LENGTH) return 0;

	OSstackSimulatorItem_t *item = (OSstackSimulatorItem*)malloc(sizeof(OSstackSimulatorItem));

	if (item == NULL)return 0;

	item->pcb = newPcb;

	newPcb->status = READY;

	item->functionValue = paramter;

	OSstackSimulatorItem_t*interator;

	for (interator = (*STATIC_OS_STACK)->startSimulatorItem; ; interator = interator->next)
	{
		if (interator->next == (*STATIC_OS_STACK)->startSimulatorItem) {
			break;
		}
	}

	interator->next = item;

	item->index = (interator->index) + 1;

	newPcb->stackPosition = item->index;

	newPcb->stackAddress.start = item->index;

	item->next = (*STATIC_OS_STACK)->startSimulatorItem;

	(*STATIC_OS_STACK)->currentDeepth += 1;

	return 1;
}



int deletePcbFromStack(int idOfPcb)
{
	int result;

	OSstackSimulatorItem* iterator;

	iterator = (*STATIC_OS_STACK)->startSimulatorItem;

	if ((*STATIC_OS_STACK)->currentDeepth == 0)
	{
		result = 0;
	}
	else {
		PCB_t*pcb = findPCB_ById(idOfPcb);

		if (pcb == NULL) {
			printf("未找到pcb\n");
			result = 0;
		}
		else {

			for (;; iterator = iterator->next)
			{
				if (iterator->next->pcb->IDofPCB == idOfPcb)
				{
					iterator->next = iterator->next->next;

					(*STATIC_OS_STACK)->currentDeepth--;
					printf("删除了一个stack");
					break;
				}
			}
			result = 1;
		}
	}
	return result;
}



PCB_t* findPCB_ById(int id)
{
	OSstackSimulatorItem_t * iterator;

	PCB_t * result = NULL;

	if ((*STATIC_OS_STACK)->currentDeepth != 0)
	{
		iterator = (*STATIC_OS_STACK)->startSimulatorItem->next;

		for (;;)
		{
			if (iterator->pcb->IDofPCB == id)
			{
				result = iterator->pcb;
			}

			if (iterator->next == (*STATIC_OS_STACK)->startSimulatorItem)
			{
				break;
			}

			iterator = iterator->next;
		}
	}
	return result;
}



void * findFunValueByPcbID(int id)
{
	OSstackSimulatorItem_t * iterator;

	void * result = NULL;

	if ((*STATIC_OS_STACK)->currentDeepth != 0)
	{
		iterator = (*STATIC_OS_STACK)->startSimulatorItem->next;

		for (;;)
		{
			if (iterator->pcb->IDofPCB == id)
			{
				result = iterator->functionValue;
				break;
			}

			if (iterator->next == (*STATIC_OS_STACK)->startSimulatorItem)
			{
				break;
			}

			iterator = iterator->next;
		}
	}
	return result;
}



OSstackSimulatorItem * findRunningItem()
{
	OSstackSimulatorItem_t * iterator;

	if ((*STATIC_OS_STACK)->currentDeepth != 0)
	{
		iterator = (*STATIC_OS_STACK)->startSimulatorItem->next;

		for (;;)
		{
			if (iterator->pcb->status == RUNNING)
			{
				return iterator;
			}

			if (iterator->next == (*STATIC_OS_STACK)->startSimulatorItem)
			{
				break;
			}

			iterator = iterator->next;
		}
	}
	return NULL;
}
