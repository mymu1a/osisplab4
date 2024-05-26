#include "global.h"

#include <string.h>
#include <unistd.h>

int startChild(char* dirChild, char* nameChild, unsigned index);


int createChild(unsigned index, char* dirChild, TYPE_CHILD childType)
{
	pid_t	pid;

	pid = fork();

	if (pid == -1)
	{
		printf("Error in fork(): %d\n", errno);
		return -1;
	}
	if (pid == 0)
	{
		if (childType == TC_PRODUCER)
		{
			startChild(dirChild, CHILD_PRODUSER_PROGRAM, index);
		}
		if (childType == TC_CONSUMER)
		{
			startChild(dirChild, CHILD_CONSUMER_PROGRAM, index);
		}
		return -1;
	}
	return pid;
}

int startChild(char* dirChild, char* nameChild, unsigned index)
{
	char nameProgram[12] = { 0, };
	char* argv[]		= { NULL, NULL, NULL };
	char pathChild[256] = { 0, };


	sprintf(nameProgram, "%s_%02d", nameChild, index);
	argv[0] = nameProgram;

	sprintf(pathChild, "%s%s", dirChild, nameChild);
	execve(pathChild, argv, NULL);

	// we get here only in case of Error

	printf("Error in execve(): %d\n", errno);

	return 0;
}
