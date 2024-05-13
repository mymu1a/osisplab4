#include <stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	printf("main ST\n");
	if (argc < 2)
	{
		printUsage(argv[0], 2);
		printf("main OK\n");
		return 1;
	}
	char* dirChild;

	dirChild = getenv(CHILD_DIR);
	if (dirChild == NULL)
	{
		printUsage(argv[0], 1);
		printf("main OK\n");
		return 1;
	}
	printf("Parent program name: %s\n", argv[0]);
	printf("Parent program pid: %d\n", getpid());
	printf("Path to ChildEnv: %s\n", argv[1]);
	printf("\n");

	printf("Type: + - l k s g s<num> g<num> p<num>\n");

	//=== read user input ===
	int ch;
	unsigned indexChild = 0;
	TAILQ_INIT(&head);                      /* Initialize the queue. */
	int childType = 0;

	//	while ((ch = getchar()) != EOF)
	while (true)
	{
		///// begin
		ch = getchar();		///
		if (ch == EOF)
		{
			printf("EOF");
			continue;
		}
		///// end

		//=== select command ===
		switch (ch)
		{
		case 'p':
		{
			pid_t	pidChild;
			childType = 1;

			pidChild = createChild(indexChild, argv[1], dirChild, childType);

			//=== child process or error ===
			if (pidChild == -1)
			{
				return 0;
			}
			break;
		}
		case 'c':
		{
			pid_t	pidChild;
			childType = 2;

			pidChild = createChild(indexChild, argv[1], dirChild, childType);

			//=== child process or error ===
			if (pidChild == -1)
			{
				return 0;
			}
			break;
		}
		case 'q':
		{
			onExit();							// clear resources

			printf("main OK quit\n");
			return 0;
		}
	}
	printf("main OK main");
	onExit();							// clear resources

	return 0;
}