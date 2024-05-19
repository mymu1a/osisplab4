#include "global.h"

struct CircleHead
{
	short indexHead;
	short indexTail;
	short size;
	struct CircleElement* pBuf;

	bool isEmpty;		// no info in the buffer
};

struct CircleElement
{
	unsigned	index;
	pid_t		pid;
};

void circleQueueInit(CircleHead* pHead, short sizeQueue, CircleElement* pBuffer);
bool circleQueueNextRead(CircleHead* pHead, CircleElement** pElement);
bool circleQueueNextWrite(CircleHead* pHead, CircleElement** pElement);
