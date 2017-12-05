/*************************************************************************
	> File Name: fork-02.c
	> Author: 
	> Mail: 
	> Created Time: 2017年12月04日 16時39分28秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_COUNT 200

void ChildProcess(void);   /* Child process prototype */
void ParentProcess(void);  /* Parent process prototype */

int main(void)
{
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        ChildProcess();    
    }
    else
    {
        ParentProcess();
    }

    return EXIT_SUCCESS;
}

void ChildProcess(void)
{
    int i;
    for (i = 1; i < MAX_COUNT; i ++)
    {
        printf("This line is from child, value = %d\n", i);
    }
    printf("    *** Child process is done ***\n");
}

void ParentProcess(void)
{
    int i;

    for (i = 1; i < MAX_COUNT; i ++)
    {
        printf("This line is from parent, value = %d\n", i);
    }
    printf("    *** Parent process is done ***\n");
}
