#include <string.h>
#include <unistd.h>


int createChild(unsigned int indexChild, char* pathChildEnv, char* dirChild, int, childType)
{
	char	pathChild[256] = { 0, };
	pid_t	pid;

	sprintf(pathChild, "%s%s", dirChild, CHILD_NAME_PROGRAM);
	pid = fork();

	if (pid == -1)
	{
		printf("Error in fork(): %d\n", errno);
		return -1;
	}
	if (pid == 0 && childType == 1)
	{
		startProduser(pathChild, pathChildEnv, indexChild);
		return -1;
	}
	if (pid == 0 && childType == 2)
	{
		startConsumer(pathChild, pathChildEnv, indexChild);
		return -1;
	}
	return pid;
}