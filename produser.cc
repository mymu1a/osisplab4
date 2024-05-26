#include "global.h"

#include "circleQueue.h"
#include "child.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* nameProgram = NULL;

struct Message
{
	unsigned char type;
	short hash;
	unsigned char size;
	void* pData;
};

struct Message* createMassage();
void startNanoSleep(CircleHead* pCircleHead);


int main(int argc, char* argv[])
{
	printf("Producer ( child process ) ST\n");
	nameProgram = argv[0];

	int			fd;
	u_char*		pHeapMemory;
	off_t		sizeMemory;
	CircleHead* pCircleHead;

	if(fd = openCircledQueue(&pHeapMemory, sizeMemory, &pCircleHead) < 0)
	{
		printf("Error: cannot open CircledQueue ( %s )\n", nameProgram);
		printf("Producer ( child process ) OK\n");
		return 1;
	}
	startNanoSleep(pCircleHead);

	createMassage();

	closeCircledQueue(pHeapMemory, sizeMemory);
	close(fd);

	printf("Producer ( child process ) OK\n");
	return 0;
}

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
			CircleElement* pElement;

			pthread_mutex_lock(&pCircleHead->mutex);									// mutex lock

			if (circleQueueNextWrite(pCircleHead, &pElement) == false)
			{
				printf("  circleQueue is full --------\t");
///				circleQueueLogState(pCircleHead);
			}
			else
			{
				printf("  add next Element\t");
///				circleQueueLogState(pCircleHead);
			}
			printf(" ( %s )\n", nameProgram);

			usleep(1000000);
			pthread_mutex_unlock(&pCircleHead->mutex);									// mutex unlock
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
