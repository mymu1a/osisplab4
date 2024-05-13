#include "global.h"

#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/queue.h>

//===== `Child` Struct =====
struct Child
{
	unsigned	index;
	pid_t		pid;

	TAILQ_ENTRY(Child) allChildren;         /* Tail queue. */
};

//===== Functions =====
int createChild(unsigned indexChild, char* dirChild, TYPE_CHILD childType);
void onExit();
void stopChildAll();

//=== Global Variables ===
TAILQ_HEAD(HeadOfCollection, Child)
	colProducer = TAILQ_HEAD_INITIALIZER(colProducer),
	colConsumer = TAILQ_HEAD_INITIALIZER(colConsumer);



int main(int argc, char* argv[], char* envp[])
{
	printf("main ST\n");

	char dirChild[] = "./";

	//	dirChild = getenv(CHILD_DIR);
	if (dirChild == NULL)
	{
		//		printUsage(argv[0], 1);
		printf("main OK\n");
		return 1;
	}
	printf("Type: + - l k s g s<num> g<num> p<num>\n");

	//=== read user input ===
	int ch;
	unsigned indexProduser = 0;
	unsigned indexConsumer = 0;

	TAILQ_INIT(&colProducer);                      /* Initialize the queue. */
	TAILQ_INIT(&colConsumer);                      /* Initialize the queue. */

	while ((ch = getchar()) != EOF)
	{
		//=== select command ===
		switch (ch)
		{
		case 'p':
		{
			pid_t	pidChild;

			pidChild = createChild(indexProduser, dirChild, TC_PRODUCER);

			//=== child process or error ===
			if (pidChild == -1)
			{
				return 0;
			}
			//=== parent process ===
			struct Child* pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexProduser;
			pChild->pid = pidChild;

			TAILQ_INSERT_TAIL(&colProducer, pChild, allChildren);
			printf("Start Producer: %d\n", indexProduser);
			indexProduser++;

			break;
		}
		case 'c':
		{
			pid_t	pidChild;

			pidChild = createChild(indexConsumer, dirChild, TC_CONSUMER);

			//=== child process or error ===
			if (pidChild == -1)
			{
				return 0;
			}
			indexConsumer++;

			break;
		}
		case 'q':
		{
			onExit();							// clear resources

			printf("main OK quit\n");
			return 0;
		}
		}
	}
	printf("main OK main");
	onExit();							// clear resources

	return 0;
}
// clear resources
void onExit()
{
	stopChildAll();						// terminate all Child processes
}

void stopChild(pid_t pidChild)
{
	kill(pidChild, SIGKILL);
}

void stopChildAll()
{
	for (struct Child* pChild = colProducer.tqh_first; pChild != NULL; )
	{
		struct Child* pChildNext;

		pChildNext = pChild->allChildren.tqe_next;

		printf("Terminate Producer: %d\n", pChild->index);
		stopChild(pChild->pid);				// send signal to Child
		free(pChild);

		pChild = pChildNext;
	}
}
