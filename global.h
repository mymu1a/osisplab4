#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <cerrno>
#include <cstdio>
#include <unistd.h>

#include <sys/queue.h>
#include <sys/types.h>


#define CHILD_PRODUSER_PROGRAM "produser"
#define CHILD_CONSUMER_PROGRAM "consumer"
#define CHILD_DIR "CHILD_PATH"

#define NAME_FILE_CIRCLE_QUEUE "circleQueue"
#define MESSAGE_FOLDER "messages"


enum TYPE_CHILD { TC_PRODUCER, TC_CONSUMER };


struct Message
{
	unsigned char type;
	short hash;
	unsigned char size;
	u_char* pData;
};


#endif // __GLOBALS_H__
