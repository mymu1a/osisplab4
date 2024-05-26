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
void closeCircledQueue(u_char* pHeapMemory, off_t& sizeMemory);							// close CircledQueue
void openCircledQueue(u_char** pHeapMemory, off_t& sizeMemory, CircleHead** pHead);		// read CircledQueue from the shared Memory
void startNanoSleep(CircleHead* pCircleHead);


struct Message
{
	unsigned char type;
	short hash;
	unsigned char size;
	void* pData;
};


void startNanoSleep(CircleHead* pCircleHead)
{
	struct timespec		time, time2;
	int					count = 5;
	int					result;

	time.tv_sec = 1;
	time.tv_nsec = 0.5 * 1000000000;

	while (count-- > 0)
	{
		result = nanosleep(&time, &time2);
		if (result == 0)
		{
			///// begin
			CircleElement* pElement;

			if (circleQueueNextWrite(pCircleHead, &pElement) == false)
			{
				printf("  circleQueue is full --------\n");
			}
			else
			{
				printf("  next Element\n");
			}
			///// end
		}
		if (result == -1)
		{
			if (errno != 4)
			{
				printf("Error in nanosleep():\n");
				printf("  errno: %d\n", errno);
				printf("  errno: %s\n", strerror(errno));

				return;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	printf("child process ST\n");
	u_char*		pHeapMemory;
	off_t		sizeMemory;
	CircleHead* pCircleHead;

	openCircledQueue(&pHeapMemory, sizeMemory, &pCircleHead);

	startNanoSleep(pCircleHead);

	createMassage();

	closeCircledQueue(pHeapMemory, sizeMemory);
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

int fd;
// read CircledQueue from the shared Memory
void openCircledQueue(u_char** pHeapMemory, off_t& sizeMemory, CircleHead** pCircleHead)
{
	printf("openCircledQueue ST\n");

	int i;
	struct stat sb;

	const char* nameFile = NAME_FILE_CIRCLE_QUEUE;

	fd = shm_open(nameFile, O_RDWR, 0644);
	fprintf(stderr, "Shared Memory Path: /dev/shm/%s\n", nameFile);
	fprintf(stderr, "Shared Mem Descriptor: fd=%d\n", fd);

	fstat(fd, &sb);
	off_t sizeFile = sb.st_size;

	*pHeapMemory = (u_char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	sizeMemory = sizeFile;
	fprintf(stderr, "sizeFile=%ld\n", sizeFile);

//	close(fd);

	// init
	CircleHead*		pHead;
	CircleElement*	pBuffer;

	pHead = (CircleHead*)pHeapMemory;
	pBuffer = (CircleElement*)(pHeapMemory + sizeof(CircleHead));
	pHead->pBuf = pBuffer;			// !!!!!!!!!


	printf("countElement: %d\n", pHead->size);
	printf("indexHead: %d\n", pHead->indexHead);

	*pCircleHead = pHead;
	printf("openCircledQueue OK\n");
}

void openMutex()
{
}

void closeCircledQueue(u_char* pHeapMemory, off_t& sizeMemory)
{
	munmap(pHeapMemory, sizeMemory);
}
