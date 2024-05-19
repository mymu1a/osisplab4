#include "circleQueue.h"


void circleQueueInit(CircleHead* pHead, short sizeQueue, CircleElement* pBuffer)
{
	pHead->indexHead = -1;
	pHead->indexTail = -1;
	pHead->size = sizeQueue;
	pHead->pBuf = pBuffer;
	pHead->isEmpty = true;
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
