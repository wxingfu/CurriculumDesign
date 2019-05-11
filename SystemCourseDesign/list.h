#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef enum {
	LISTReady,
	LISTBlocking,
	LISTDelete,
	LISTonINIT
}LIST_STATUS;

typedef struct ProcessList ProcessList;
typedef struct ListItem ListItem;
//typedef struct subordinateListItem subordinateListItem;
typedef struct ListItem {
	clock_t runTime;
	ListItem* next;
	ListItem* previous;
	void* PCB_block;
	ProcessList* hostList;
	unsigned priorityValue;

}ListItem;

//typedef struct subordinateListItem {
//	ListItem* next;
//	ListItem* previous;
//	unsigned priorityValue;
//}subordinateListItem;

typedef struct ProcessList {
	volatile unsigned numberOfProcesses;
	ListItem* ListItemIndex;
	ListItem* lastItem;
	LIST_STATUS listType;

}ProcessList;

#define setListPCB_Pointer(ListItem,PCBpointer) ((ListItem)->PCB_block=(void*)PCBpointer)

//定义最多进程数
#define MAX_PORCESS_NUMBER 20

//定义最低优先级
#define MAX_subordinateListItemValue 35

//获得当前链表进程数
#define GET_LIST_NUMBER(List) ((List)->numberOfProcesses)
//获得当前列表项优先级
#define GET_priorityValue(Item) ((Item)->priorityValue)
//设置当前列表项优先级
#define SET_priorityValue(Item,value) ((Item)->priorityValue=value)
//判断当前列表是否为空
#define LIST_IS_EMPTY(list) ((list->numberOfProcesses==0)?1:0)




//获得链表下一个时间片的进程
#define listGET_OWNER_OF_NEXT_ENTRY( pcb, pxList )										\
{																							\
ProcessList * const pxConstList = ( pxList );													\
	/* Increment the index to the next item and return the item, ensuring */				\
	/* we don't return the marker used at the end of the list.  */							\
	( pxConstList )->ListItemIndex = ( pxConstList )->ListItemIndex->next;							\
	if( ( void * ) ( pxConstList )->ListItemIndex == ( void * ) &( ( pxConstList )->lastItem ) )	\
	{																						\
		( pxConstList )->ListItemIndex = ( pxConstList )->ListItemIndex->next;						\
	}																						\
	( pcb ) = ( pxConstList )->ListItemIndex->PCB_block;											\
}

//获得链表 第一个进程
#define listGET_OWNER_OF_HEAD_ENTRY(list) (list->lastItem->next->PCB_block)

//检查列表是否被初始化
#define listIS_INITIAL(list) (list->lastItem->priorityValue==MAX_subordinateListItemValue)

//设置列表项值
#define listSetListItemValue(listItem,value) ((listItem)->runTime=value)


void InitProcessList(ProcessList* list);

void InitListItem(ListItem* item);

void InsertItemIntoProcessList(ListItem* item, ProcessList* list);

void InsertItemToListEnd(ListItem*item, ProcessList* list);

int DeleteFromList(ListItem*item);

void SET_LIST_STATE(ProcessList* list, LIST_STATUS status);
