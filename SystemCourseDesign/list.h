#include <stdio.h>
#include <stdlib.h>
#include <time.h>



//枚举链表状态
typedef enum {
	LISTReady,
	LISTBlocking,
	LISTDelete,
	LISTonINIT
}LIST_STATUS;

//声明进程链表结构体
typedef struct ProcessList ProcessList;

//声明链表项结构体
typedef struct ListItem ListItem;

//定义链表项结构体
typedef struct ListItem {
	clock_t runTime;
	ListItem* next;
	ListItem* previous;
	void* PCB_block;
	ProcessList* hostList;
	unsigned priorityValue;

}ListItem;

//定义进程链表结构体
typedef struct ProcessList {
	volatile unsigned numberOfProcesses;
	ListItem* ListItemIndex;
	ListItem* lastItem;
	LIST_STATUS listType;
}ProcessList;

//将PCB指针指向对应的链表
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

//初始化链表
void InitProcessList(ProcessList* list);

//初始化链表项
void InitListItem(ListItem* item);

//插入一个链表项到链表
void InsertItemIntoProcessList(ListItem* item, ProcessList* list);

//插入一个链表项到链表末尾
void InsertItemToListEnd(ListItem*item, ProcessList* list);

//移除一个链表项
int DeleteFromList(ListItem*item);

//设置链表状态
void SET_LIST_STATE(ProcessList* list, LIST_STATUS status);
