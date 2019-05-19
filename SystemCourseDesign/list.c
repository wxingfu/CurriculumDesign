#include "list.h"


//初始化链表
void InitProcessList(ProcessList * list)
{
	list->ListItemIndex = list->lastItem;
	list->numberOfProcesses = 0;
	list->lastItem->previous = list->lastItem;
	list->lastItem->next = list->lastItem;
	list->listType = LISTonINIT;
	list->lastItem->priorityValue = MAX_subordinateListItemValue;
}


//初始化链表项
void InitListItem(ListItem * item)
{
	item->hostList = NULL;
}


//插入一个链表项到链表
void InsertItemIntoProcessList(ListItem * item, ProcessList * list)
{
	ListItem* itemIterator;
	volatile unsigned newPriorityValue = item->priorityValue;
	if (newPriorityValue == MAX_subordinateListItemValue)
	{
		itemIterator = list->lastItem->previous;
	}
	else {
		//寻找到合适优先级的链表
		for (itemIterator = list->lastItem;
			itemIterator->next->priorityValue <= newPriorityValue;
			itemIterator = itemIterator->next);
	}
	item->next = itemIterator->next;
	item->previous = itemIterator;
	item->next->previous = item;
	itemIterator->next = item;
	item->hostList = list;
	list->numberOfProcesses += 1;
}


//插入一个链表项到进程链表末尾
void InsertItemToListEnd(ListItem * item, ProcessList * list)
{
	if (list->numberOfProcesses == 0) {
		list->lastItem->next = item;
		list->lastItem->previous = item;
		item->next = (ListItem*)&(list->lastItem);
		item->previous = (ListItem*)&(list->lastItem);
		item->hostList = list;
	}
	else {
		ListItem * itemIterator = list->lastItem->previous;
		itemIterator->next = item;
		item->previous = itemIterator;
		item->next = (ListItem*)&(list->lastItem);
		list->lastItem->previous = item;
	}
	list->numberOfProcesses += 1;
}


//移除一个链表项
int DeleteFromList(ListItem * item)
{
	ProcessList* hostList = item->hostList;
	item->next->previous = item->previous;
	item->previous->next = item->next;
	if (hostList->ListItemIndex == item)
	{
		hostList->ListItemIndex = item->previous;
	}
	item->hostList = NULL;
	(hostList->numberOfProcesses)--;
	return hostList->numberOfProcesses;
}


//设置链表状态
void SET_LIST_STATE(ProcessList * list, LIST_STATUS status)
{
	list->listType = status;
}
