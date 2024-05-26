#include "global.h"

#include "circleQueue.h"
#include "child.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>

const char* nameProgram = NULL;

void startNanoSleep(CircleHead* pCircleHead);


int main(int argc, char* argv[])
{
	printf("Consumer ( child process ) ST\n");
	nameProgram = argv[0];

	int			fd;
	u_char*		pHeapMemory;
	off_t		sizeMemory;
	CircleHead* pCircleHead;

	if (fd = openCircledQueue(&pHeapMemory, sizeMemory, &pCircleHead) < 0)
	{
		printf("Error: cannot open CircledQueue ( %s )\n", nameProgram);
		printf("Consumer ( child process ) OK\n");
		return 1;
	}
	startNanoSleep(pCircleHead);

	closeCircledQueue(pHeapMemory, sizeMemory);
	close(fd);
	
	printf("Consumer ( child process ) OK\n");
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

			if (circleQueueNextRead(pCircleHead, &pElement) == false)
			{
				printf("  circleQueue is empty -- xxxx --\t");
			}
			else
			{
				printf("  read next Element\t");
			}
			printf(" ( %s )\n", nameProgram);

			usleep(500000);
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