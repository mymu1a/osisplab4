#include "global.h"

#include "circleQueue.h"
#include "child.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


const char* nameProgram = NULL;


struct Message* createMessage();
void startNanoSleep(CircleHead* pCircleHead);
void writeMessage(unsigned indexMessage, struct Message* pMessage);


int main(int argc, char* argv[])
{
	nameProgram = argv[0];

	int			fd;
	u_char*		pHeapMemory;
	off_t		sizeMemory;
	CircleHead* pCircleHead;

	if(fd = openCircledQueue(&pHeapMemory, sizeMemory, &pCircleHead) < 0)
	{
		printf("Error: cannot open CircledQueue ( %s )\n", nameProgram);
		return 1;
	}
	startNanoSleep(pCircleHead);
	
	closeCircledQueue(pHeapMemory, sizeMemory);
	close(fd);

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
				struct Message* pMessage;

				pCircleHead->indexMessage++;									// name File

				pMessage = createMessage();
				writeMessage(pCircleHead->indexMessage, pMessage);
				
				pCircleHead->countWrite++;												// increment `счетчик добавленных сообщений`
				printf("  %d ( count write )\t", pCircleHead->countWrite);

				pElement->indexMessage = pCircleHead->indexMessage;
				
///				circleQueueLogState(pCircleHead);
			}
			printf(" ( %s )\n", nameProgram);

			usleep(1000000);
			pthread_mutex_unlock(&pCircleHead->mutex);									// mutex unlock
			usleep(2);
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

struct Message* createMessage()
{
	int random;
	struct Message* pMessage;

	pMessage = (Message*)malloc(sizeof(struct Message));
	random = 0;

	while (random == 0)
	{
		random = rand();
		random = random % 257;
	}
	pMessage->size = random;
	pMessage->pData = (u_char*)malloc(pMessage->size);

	printf("pMessage->size=%d\n", pMessage->size);

	u_char byte;

	for (int i = 0; i < pMessage->size; i++)
	{
		byte = 0;
		while (byte == 0)
		{
			byte = (u_char)rand();
		}
		pMessage->pData[i] = byte;
		printf("%02x ", byte);
	}
	printf("\n");
	pMessage->hash = 0;
	pMessage->type = 1;

	return pMessage;
}

void writeMessage(unsigned indexMessage, struct Message* pMessage)
{
	int fd;
	char pathMessage[8 + 6] = { 0, };

	sprintf(pathMessage, "%s/%05d", MESSAGE_FOLDER, indexMessage);

	fd = open(pathMessage, O_RDWR | O_CREAT, 0666);
	if (fd < 0)
	{
		printf("Error: cannot open Message file:\n");
		return;
	}
	write(fd, (void*)&pMessage->type, 1);
	write(fd, (void*)&pMessage->hash, 2);
	write(fd, (void*)&pMessage->size, 1);
	write(fd, (void*)pMessage->pData, pMessage->size);

	// extended info
	const char message[] = { 10, 13 };
	write(fd, message, 2);
	write(fd, nameProgram, 11);

	close(fd);
}
