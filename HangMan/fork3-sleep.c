#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

	printf("I am : %d\n", (int) getpid());
	pid_t pid = fork();
	printf("fork returned:%d \n", (int) pid);
	
	if (pid<0) { 
		perror("fork failed");
		}
		
		if(pid==0) {
		printf("I am the child process witdh pid %d \n",(int) getpid());
		sleep(5);
		printf("child exiting.......");
		exit(0);
		}
		
		printf("I am the parent, waiting for the child to end \n");
		wait(NULL);
		printf("Parent ending \n");
		
	
	
	
	
	return 0;
	
	
	
}
