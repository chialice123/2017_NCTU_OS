#include <stdio.h>
#include <stdlib.h> //exit()
#include <unistd.h>
#include <sys/wait.h> //wait()
#include <sys/types.h> //pid_t

int main(int argc, char **argv)
{
    /* print main process id */
	printf("Main Process ID: %d\n\n", getpid());

	pid_t pid_child1, pid_child2, pid_child3, pid_child4, pid_child5;

    pid_child1 = fork();
    if (pid_child1<0){
    	fprintf(stderr, "Child1 Fork Failed\n");
		exit(EXIT_FAILURE);
	} else if (pid_child1==0){ // child-child1 process
		printf("Fork 1. I'm the child %d, my parent is %d.\n", getpid(), getppid());
		pid_child2 = fork();
        if(pid_child2<0){
            fprintf(stderr, "Child2 Fork Failed\n");
            exit(EXIT_FAILURE);
        } else if (pid_child2==0){ // child-child2 process
            printf("Fork 2. I'm the child %d, my parent is %d.\n", getpid(), getppid());
            pid_child4 = fork();
            if (pid_child4<0){
                fprintf(stderr, "Child4 Fork Failed\n");
                exit(EXIT_FAILURE);
            } else if (pid_child4==0){ // child-child4 process
                printf("Fork 3. I'm the child %d, my parent is %d.\n", getpid(), getppid());
                exit(EXIT_SUCCESS);
            } else { //parent-child2 process
                wait(NULL);
            }
            exit(EXIT_SUCCESS);
        } else { // parent-child1 process
            pid_child3 = fork();
            if (pid_child3<0){
                fprintf(stderr, "Child3 Fork Failed\n");
                exit(EXIT_FAILURE);
            } else if (pid_child3==0){ // child-child3 process
                printf("Fork 2. I'm the child %d, my parent is %d.\n", getpid(), getppid());
                pid_child5 = fork();
                if (pid_child5<0){
                    fprintf(stderr, "Child5 Fork Failed\n");
                    exit(EXIT_FAILURE);
                } else if (pid_child5==0){ // child-child5 process
                    printf("Fork 3. I'm the child %d, my parent is %d.\n", getpid(), getppid());
                    exit(EXIT_SUCCESS);
                } else { // parent-child3 process
                    wait(NULL);
                }
                exit(EXIT_SUCCESS);
            } else { // parent-child1 process
                wait(NULL);
            }
            exit(EXIT_SUCCESS);
        }
	} else { // main process
	    wait(NULL);
	    //system("ps aux | grep 'Z'");
	    exit(EXIT_SUCCESS);
	}

    return 0;
}

