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
void stopChildAll(TYPE_CHILD type);

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
	printf("Type: p c k q\n");

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
			//=== parent process ===
			struct Child* pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexConsumer;
			pChild->pid = pidChild;

			TAILQ_INSERT_TAIL(&colConsumer, pChild, allChildren);
			printf("Start Consumer: %d\n", indexConsumer);
			indexConsumer++;

			break;
		}
		case 'k':								// terminate all Child processes
		{
			onExit();							// clear resources
			TAILQ_INIT(&colProducer);           /* Initialize the queue. */
			TAILQ_INIT(&colConsumer);           /* Initialize the queue. */
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
	stopChildAll(TC_PRODUCER);			// terminate all Child processes
	stopChildAll(TC_CONSUMER);			// terminate all Child processes
}

void stopChild(pid_t pidChild)
{
	kill(pidChild, SIGKILL);
}

void stopChildAll(TYPE_CHILD type)
{
	struct HeadOfCollection* pColl = NULL;
	const char* message;

	if (type == TC_PRODUCER)
	{
		pColl = &colProducer;
		message = "Terminate Producer: %d\n";
	}
	else
	{
		pColl = &colConsumer;
		message = "Terminate Consumer: %d\n";
	}

	// delete from the collection
	for (struct Child* pChild = pColl->tqh_first; pChild != NULL; )
	{
		struct Child* pChildNext;

		pChildNext = pChild->allChildren.tqe_next;

		printf(message, pChild->index);
		stopChild(pChild->pid);						// send signal to Child
		free(pChild);

		pChild = pChildNext;
	}
}
