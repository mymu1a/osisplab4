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
extern bool inProcess;					// Porducer works with Message
extern bool isExit;						// whether Porducer should exit


void readMessage(unsigned indexMessage, struct Message* pMessage);
void startNanoSleep(CircleHead* pCircleHead);


int main(int argc, char* argv[])
{
	if (connectExitSignal() == false)
	{
		printf("Error: cannot connect Exit Signal\n");
		return 1;
	}
	nameProgram = argv[0];

	int			fd;
	u_char*		pHeapMemory;
	off_t		sizeMemory;
	CircleHead* pCircleHead;

	if (fd = openCircledQueue(&pHeapMemory, sizeMemory, &pCircleHead) < 0)
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
			inProcess = true;  // prevent terminating by Exit Signal

			if (circleQueueNextRead(pCircleHead, &pElement) == false)
			{
				printf("  circleQueue is empty -- xxxx --\t");
			}
			else
			{
				struct Message pMessage;

				readMessage(pElement->indexMessage, &pMessage);
				printf("  read next Element\t");
				pCircleHead->countRead++;												// increment `счетчик извлеченных сообщений`
				printf("  %d ( count read )\t", pCircleHead->countRead);

			}
			printf(" ( %s )\n", nameProgram);

			usleep(500000);
			inProcess = false;  // prevent terminating by Exit Signal
			pthread_mutex_unlock(&pCircleHead->mutex);									// mutex unlock
			if (isExit == true)
			{
				printf("isExit == true !!!!!!!!!!!!!\n");
				exit(0);
			}

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

void readMessage(unsigned indexMessage, struct Message* pMessage)
{
	int fd;
	char pathMessage[8 + 6] = { 0, };

	sprintf(pathMessage, "%s/%05d", MESSAGE_FOLDER, indexMessage);

	fd = open(pathMessage, O_RDWR, 0666);
	if (fd < 0)
	{
		printf("Error: cannot open Message file:\n");
		return;
	}




	read(fd, (void*)&pMessage->type, 1);
	read(fd, (void*)&pMessage->hash, 2);
	read(fd, (void*)&pMessage->size, 1);

	printf("pMessage->type = %d", pMessage->type);
	printf("pMessage->size = %d", pMessage->size);

	pMessage->pData = (u_char*)malloc(pMessage->size);
	read(fd, (void*)pMessage->pData, pMessage->size);

	close(fd);
}
