#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/wait.h>
#include <string.h>
void parse(char *line, char **argv, char ch)
{
    int k = 0;
    char *token = strtok(line, " "), *save;
    argv[k++] = token; 
    save = token;
    token = strtok(NULL, " ");
    while (token != NULL) 
    {
    	save = token;
        argv[k++] = token;
        token = strtok(NULL, " ");
    }
  	if(ch == '2')
    	argv[k++] = "&";
    argv[k] = NULL;                   
}
int main()
{ 
	pid_t pid;
	int status, i, j; 
	char ch;
	char line[50];
	printf("Welcome to Interactive C-Based Shell Program\nType Of Commands:\n1. Start A New Foreground Process\n2. Start A New Background Process\n3. Exit the Program\n"); 
	do
	{
		printf("Enter Your Choice: ");
		fgets(line, 50, stdin);
		ch = line[0];
		if(ch == '1' || ch == '2')
		{
			printf("Enter the New Process with any options <path of executable> <space separted option(s)/parameters> ..");
			int argc = 0, len;
			char *argv[20];
			fgets(line, 50, stdin);
			len = strlen(line);
			line[len - 1] = 0;
			if(len == 0)
			{
				printf("Empty Input\n");
				continue;
			}
			parse(line, argv, ch);
			pid = fork();
			if (pid == -1)
				printf("Can't fork, Error occured %d\n", EXIT_FAILURE); 
			else if(pid == 0)
			{
				printf("Child Process, Pid = %u\n",getpid()); 
				execv(argv[0], argv); 
				exit(0); 
			}
			if(pid > 0)
			{
				if(ch == 1)
				{
					/*if (waitpid(pid, &status, 0) > 0) 
					{ 
						if (WIFEXITED(status) && !WEXITSTATUS(status)) 
						printf("Child Process Terminated Successfully\n");
						else if (WIFEXITED(status) && WEXITSTATUS(status)) 
						{ 
							if (WEXITSTATUS(status) == 127) { 
								// execv failed 
								printf("execv failed\n"); 
							} 
							else
								printf("Child Process terminated normally,"
								" but returned a non-zero status: %d\n", WEXITSTATUS(status));				 
						} 
						else
							printf("Child Process didn't terminate normally\n");			 
					} 
					else 
						printf("waitpid() failed\n");*/
					while (wait(NULL) != -1 || errno != ECHILD)
						printf("Here\n");;
				}
			}
		}
		else if(ch != '3')
			printf("Invalid Choice\n");
	} while(ch != '3');
	printf("Quitting the Interactive Shell\n");
	return 0; 
} 
