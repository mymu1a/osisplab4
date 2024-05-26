#include "circleQueue.h"

// initialize Mutex for mutual exclusive access to the Queue
void initMutex(CircleHead* pHead);


void circleQueueInit(CircleHead* pHead, short sizeQueue, CircleElement* pBuffer)
{
	pHead->indexHead = -1;
	pHead->indexTail = -1;
	pHead->size = sizeQueue;
	pHead->pBuf = pBuffer;
	pHead->isEmpty = true;

	initMutex(pHead);
}

void circleQueueLogState(CircleHead* pHead)
{
	printf("indexHead=%d\tindexTail=%d\tisEmpty=%s\tsizeQueue=%d", pHead->indexHead, pHead->indexTail, pHead->isEmpty ? "true":"false", pHead->size);
}

bool circleQueueNextWrite(CircleHead* pHead, CircleElement** pElement)
{
	if (pHead->indexHead == (pHead->size - 1) && pHead->indexTail == -1)
	{
		return false;
	}
	if (pHead->indexHead == pHead->indexTail && pHead->isEmpty == false)
	{
		return false;
	}
	pHead->isEmpty = false;

	if (++pHead->indexHead == pHead->size)
	{
		pHead->indexHead = 0;
	}
	*pElement = &pHead->pBuf[pHead->indexHead];

	return true;
}

bool circleQueueNextRead(CircleHead* pHead, CircleElement** pElement)
{
	if (pHead->isEmpty == true)
	{
		return false;
	}
	if (++pHead->indexTail == pHead->size)
	{
		pHead->indexTail = 0;
	}
	if (pHead->indexTail == pHead->indexHead)
	{
		pHead->isEmpty = true;
	}
	*pElement = &pHead->pBuf[pHead->indexTail];

	return true;
}

// initialize Mutex for mutual exclusive access to the Queue
void initMutex(CircleHead* pHead)
{
	pthread_mutexattr_t	attribute;

	pthread_mutexattr_init(&attribute);
	pthread_mutexattr_setpshared(&attribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&pHead->mutex, &attribute);
}
