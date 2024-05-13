#include <cerrno>
#include <cstdio>

#define CHILD_PRODUSER_PROGRAM "produser"
#define CHILD_CONSUMER_PROGRAM "consumer"
#define CHILD_DIR "CHILD_PATH"

enum TYPE_CHILD { TC_PRODUCER, TC_CONSUMER };
