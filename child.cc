#include "child.h"

#include "circleQueue.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


// read CircledQueue from the shared Memory
int openCircledQueue(u_char** pHeapMemory, off_t& sizeMemory, CircleHead** pCircleHead)
{
	printf("openCircledQueue ST\n");

	int i;
	struct stat sb;
	int fd;

	const char* nameFile = NAME_FILE_CIRCLE_QUEUE;

	fd = shm_open(nameFile, O_RDWR, 0644);
	if (fd < 0)
	{
		return fd;
	}
	/*
	fprintf(stderr, "Shared Memory Path: /dev/shm/%s\n", nameFile);
	fprintf(stderr, "Shared Mem Descriptor: fd=%d\n", fd);
	//*/
	fstat(fd, &sb);
	off_t sizeFile = sb.st_size;

	*pHeapMemory = (u_char*)mmap(NULL, sizeFile, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	sizeMemory = sizeFile;
	fprintf(stderr, "sizeFile=%ld\n", sizeFile);

	// init
	CircleHead*		pHead;
	CircleElement*	pBuffer;

	pHead = (CircleHead*)*pHeapMemory;
	pBuffer = (CircleElement*)(*pHeapMemory + sizeof(CircleHead));
	pHead->pBuf = pBuffer;			// !!!!!!!!!


	printf("countElement: %d\n", pHead->size);
	printf("indexHead: %d\n", pHead->indexHead);

	*pCircleHead = pHead;
	printf("openCircledQueue OK\n");
}

void closeCircledQueue(u_char* pHeapMemory, off_t& sizeMemory)
{
	munmap(pHeapMemory, sizeMemory);
}
