#include "global.h"

#include "circleQueue.h"

#include <ctype.h>
#include <dirent.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>	/* For mode constants */
#include <fcntl.h>		/* For O_* constants */
#include <sys/wait.h>



#define SIZE_MESSAGE_QUEUE 8

//===== `Child` Struct =====
struct Child
{
	unsigned	index;
	pid_t		pid;

	TAILQ_ENTRY(Child) allChildren;         /* Tail queue. */
};

//=== Global Variables ===
TAILQ_HEAD(HeadOfCollection, Child)
	colProducer = TAILQ_HEAD_INITIALIZER(colProducer),
	colConsumer = TAILQ_HEAD_INITIALIZER(colConsumer);


//===== Functions =====
bool cleanDirectory(const char* path);		// remove all the files and directories within directory
int createChild(unsigned indexChild, char* dirChild, TYPE_CHILD childType);
void createColMessage();					// initialize circled collection for Messages
void onExit();
sem_t* openSemaphore(const char* nameSemaphore, unsigned value);
void stopChildAll(TYPE_CHILD type);



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
	if (cleanDirectory(MESSAGE_FOLDER) == false)
	{
		printf("Error: cannot clear `messages` folder\n");
		return 1;
	}
	sem_t* pSemaphoreProducer;

	pSemaphoreProducer = openSemaphore(SEM_PRODUSER_NAME, SEM_PRODUSER_VALUE);
	if (pSemaphoreProducer == NULL)
	{
		printf("Error: cannot open Semaphore\n");
		return 1;
	}
	sem_t* pSemaphoreConsumer;

	pSemaphoreConsumer = openSemaphore(SEM_CONSUMER_NAME, SEM_CONSUMER_VALUE);
	if (pSemaphoreConsumer == NULL)
	{
		printf("Error: cannot open Semaphore\n");
		return 1;
	}
	printf("Type: p c k q\n");

	//=== read user input ===
	int ch;
	unsigned indexProduser = 0;
	unsigned indexConsumer = 0;

	TAILQ_INIT(&colProducer);                   /* Initialize the queue. */
	TAILQ_INIT(&colConsumer);                   /* Initialize the queue. */

	createColMessage();							// initialize circled collection for Messages

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
		case 'z':						// kill zombie processes
		{
			int stat;

			while (waitpid(-1, &stat, WUNTRACED) > 0)
			{
			}
			break;
		}
		}
	}
	printf("main OK main");
	onExit();							// clear resources
	
	// wait for children termination
	while (wait(NULL) > 0)
	{
	}

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
	kill(pidChild, SIGUSR1);
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

// initialize circled collection for Messages
void createColMessage()
{
	off_t		sizeFile;
	const char* nameFile = NAME_FILE_CIRCLE_QUEUE;
	int			fd;

	fd = shm_open(nameFile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	fprintf(stderr, "Shared Mem Descriptor: fd=%d\n", fd);

	sizeFile = sizeof(CircleHead) + sizeof(CircleElement) * SIZE_MESSAGE_QUEUE;
	ftruncate(fd, sizeFile);

	u_char* pHeapMemory = (u_char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
/*
	fprintf(stderr, "Shared Memory Address: %p [0..%lu]\n", pHeapMemory, sizeFile - 1);
	fprintf(stderr, "Shared Memory Path: /dev/shm/%s\n", nameFile);
	printf("sizeof(CircleHead)=0x%x\n", sizeof(CircleHead));
	printf("sizeof(CircleElement)=0x%x\n", sizeof(CircleElement));
//*/
	CircleHead* pHead;

	pHead = (CircleHead*)pHeapMemory;

	circleQueueInit(pHead, SIZE_MESSAGE_QUEUE);
}

// remove all the files and directories within directory
bool cleanDirectory(const char* path)
{
	struct dirent* ent;
	DIR* dir;
	char pathFile[255 /*MAXPATHLEN*/] = {0,};
	
	dir = opendir(path);
	if (dir == NULL)
	{
		return false;
	}
	while ((ent = readdir(dir)) != NULL)
	{
		sprintf(pathFile, "%s/%05d", path, ent->d_name);
		remove(pathFile);
	}
	closedir(dir);

	return true;
}

sem_t* openSemaphore(const char* nameSemaphore, unsigned value)
{
	sem_t* pSemaphore;
	pSemaphore = sem_open(nameSemaphore, O_CREAT, 0660, value);
	if (pSemaphore == SEM_FAILED)
	{
		return NULL;
	}
	return pSemaphore;
}
