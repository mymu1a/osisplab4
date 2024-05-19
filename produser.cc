#include "global.h"

#include "circleQueue.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <cassert>


struct Message* createMassage();
void openCircledQueue();

struct Message
{
	unsigned char type;
	short hash;
	unsigned char size;
	void* pData;
};

int main(int argc, char* argv[])
{
	printf("child process ST\n");

	openCircledQueue();
	createMassage();

	printf("child process OK\n\n");

	return 0;
}

struct Message* createMassage()
{
	int random;
	struct Message* pMessage = (Message*)malloc(sizeof(struct Message));

	random = 0;
	while (random == 0)
	{
		random = rand();
		random = random % 257;
	}

	pMessage->hash = 0;
	pMessage->type = 1;
	pMessage->size = random;
	pMessage->pData = malloc(((pMessage->size + 3) / 4) * 4);

	return pMessage;
}

void openCircledQueue()
{
	printf("openCircledQueue ST\n");

	int i;
	struct stat sb;

	const char* nameFile = NAME_FILE_CIRCLE_QUEUE;
	int     fd = shm_open(nameFile, O_RDWR, 0644);

	fstat(fd, &sb);
	off_t sizeFile = sb.st_size;

	u_char* pHeapMemory = (u_char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	CircleHead* pHead;
	CircleElement* pBuffer;

	pHead = (CircleHead*)pHeapMemory;
	pBuffer = (CircleElement*)(pHeapMemory + sizeof(CircleHead));


	printf("countElement: %d\n", pHead->size);
	printf("indexHead: %d\n", pHead->indexHead);

	///// begin
	CircleElement* pElement;

	pHead->pBuf = pBuffer;			// !!!!!!!!!

	while (circleQueueNextRead(pHead, &pElement) == true)
	{
		printf("  next Element: %d\n", pElement->index);
	}
	///// end


	close(fd);
	printf("openCircledQueue OK\n");
}
