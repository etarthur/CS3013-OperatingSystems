#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>

#define INPUTLENGTH 128
#define MAXARGS 32

////menu function simply prints out all the options avaliable to the user
void menu(){
printf("===== Mid-Day Commander, v0 ===== \n");
printf("G'day, Commander! What command would you like to run? \n");
printf("   0. whoami   : Prints out the result of the whoami command \n");
printf("   1. last     : Prints out the result of the last command \n");
printf("   2. ls       : Prints out the result of a listing on a user-specific path \n");
printf("Option?:");
}

//run consumes a char** and uses fork and exec to run the command then print out statistics about it.
//the first stirng in arg, arg[0] should be the command, the following strings are options, must
//terminate in a null string for exec to work
void run(char* args[]){
char * command = args[0];//the command to run
if(fork()==0){//child process
	execvp(command, &args[0]);//run the command
	printf("\n");
}
else{
	struct timeval start;//timer variables start and end to monitor process time
	struct timeval end;

	struct rusage rstart;//usage variables start and end to monitor resource usage
	struct rusage rend;
	gettimeofday(&start,NULL);//start timer
	getrusage(RUSAGE_CHILDREN,&rstart);//start watching children resources
	
	wait(NULL);//wait for the child to terminate

	gettimeofday(&end,NULL);//end clock
	getrusage(RUSAGE_CHILDREN,&rend);//end resource monitor

	double runtime = (end.tv_usec - start.tv_usec) * .001;//calculate data and print it
	int minfaults = (rend.ru_minflt - rstart.ru_minflt);
	int majfaults = (rend.ru_majflt - rstart.ru_majflt);


	printf("== Statistics == \n");
	printf("Elapsed time: %.3f milliseconds \n",runtime);
	printf("Page Faults: %d \n", minfaults);
	printf("Page Faults (reclaimed): %d \n", majfaults);

}
}

//main runs the prompt
int main(){
menu();//calls menu to print
char ** command;
command = malloc(sizeof(char*) * MAXARGS);//allocate memory to store the command strings in
for(int i = 0; i < MAXARGS; i++){
	command[i] = malloc(sizeof(char) * INPUTLENGTH);
}
int choice = fgetc(stdin);//get input char
getchar();//remove the \n from stdin
switch(choice){//jump to execution based on choice using ASCII values for input
	case 48://whoami branch
		printf("== Who Am I? == \n");//print more menu stuff
		strcpy(command[0],"whoami");//copy command over whoami
		command[1] = NULL;//terminate command arguments with null
		run(command);//run it
		break;

	case 49://last branch
		printf("== Last Logins == \n");
		strcpy(command[0],"last");//copy command over last
		command[1] = NULL;
		run(command);
		break;

	case 50://ls branch
		printf("== Directory Listing == \n");
		printf("Enter none for no args/path \n");
		int current = 1;
		char options[INPUTLENGTH];
		char path[INPUTLENGTH];
		strcpy(command[0],"ls");//copy the command
		printf("Arguments?:");
		fgets(options,INPUTLENGTH,stdin);
		if(options[0]=='\n'){
		}
		else{
			for(int i =0; i < INPUTLENGTH;i++){
				if(options[i]=='\n'){//if any char in the string is a newline character
					options[i] = '\0';//terminate the string there
				}
			}
			strcpy(command[current],options);//copy over the input
			current++;//increment current for next arguments
		}

		printf("\n Path?:");
		fgets(path,INPUTLENGTH,stdin);//exact same process as above
		if(path[0]=='\n'){
			command[current] = NULL;
		}
		else{
			for(int i =0; i < INPUTLENGTH;i++){
				if(path[i]=='\n'){
					path[i] = '\0';
				}
			}
			strcpy(command[current],path);
			current++;
		}
		command[current] = NULL;
		printf("\n\n");
		run(command);
		break;
	default://if someone enters input that is not valid
		printf("\n");
		printf("Invalid input, try again! \n");
}

printf("\n \n \n");
for(int i =0; i < MAXARGS; i++){
	free(command[i]);
}
free(command);//free the command and rerun
main();//no way to exit except crtl-c
return 0;
}
