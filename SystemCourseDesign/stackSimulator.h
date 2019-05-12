#include "pcbUtil.h"

#include <stdio.h>


#define MAX_STACK_LENGTH 20



typedef struct OSstackSimulatorItem {
	PCB_t* pcb;
	void* functionValue;
	struct OSstackSimulatorItem* next;
	int index;
}OSstackSimulatorItem;


typedef struct OSstackSimulatorItem OSstackSimulatorItem_t;


typedef struct OSstack {
	OSstackSimulatorItem_t * startSimulatorItem;
	int currentDeepth;
	int totalLength;
	OSstackSimulatorItem_t*lastItem;
}OSstack;


//全局堆栈表头
OSstack **STATIC_OS_STACK;


int initOSstackSimulator();


int addPcbToStack(PCB_t* newPcb, void*param);


int deletePcbFromStack(int idOfPcb);


PCB_t* findPCB_ById(int id);


void*findFunValueByPcbID(int id);


OSstackSimulatorItem*findRunningItem();
