#include "global.h"

#include <pthread.h>


struct CircleHead
{
	short indexHead;
	short indexTail;
	short size;
	struct CircleElement* pBuf;

	bool isEmpty;			// no info in the buffer
	pthread_mutex_t mutex;	// mutual exclusive access to the Queue
};

struct CircleElement
{
	unsigned	index;
	pid_t		pid;
};

void circleQueueInit(CircleHead* pHead, short sizeQueue, CircleElement* pBuffer);
bool circleQueueNextRead(CircleHead* pHead, CircleElement** pElement);
bool circleQueueNextWrite(CircleHead* pHead, CircleElement** pElement);
