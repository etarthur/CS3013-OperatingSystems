#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#define INPUTLENGTH 128
#define MAXARGS 32
#define MAXBACK 20

//same struct from m1, read more there
struct Command{
char** stored;
char* commandname;
int opnum;
int background;//now has background int which is 1 if its a background task or 0 otherwise
};

int leave = 0;//exit flag

int removepid = 0;//current pid to remove from the queue

int running[MAXBACK];//the background queue of all running processes

int addedcommands = 0;//# of commands added

struct Command commands[100];//command storage


//add a command to the background queue, by enter PID the process is added to the background,
//the return value is the index added at, otherwise -1 is returned if there is no space left.
int addrunning(int pid){
	for(int i =0; i < MAXBACK; i++){
		if(running[i]==0){
			running[i]=pid;//add running and return the index added at
			return i;
		}
	}
	return -1;//return -1 to symbolize no room to add
}

//printrunning() prints out the running background processess
void printrunning(){
	for(int i =0; i < MAXBACK; i++){
		if(running[i] > 0){// >0 means that something was added to that index
			printf("[%d] PID: %d\n",i+1,running[i]);
		}
		}
	}

//removebackground() takes a pid and removes it from the array queue, this is done and then
//every index after the removal gets shifted down one. This creates  a queue read more in the readme.
void removebackground(int pid){
	for(int i =0; i < MAXBACK; i++){
		if(running[i]==pid){
			running[i] = 0;//first remove the PID
		}
	}
	int index;
	for(int i =0; i < MAXBACK; i++){
		if(running[i]==0){
			index = i;//find earliest occurance of a zero
				}
			}
		if(index == 0){
			running[0] = running[1];//if its zero start moving now
		}
	for(int i = index; i < 19; i++){
		running[i] = running[i+1];//shift all data down
		}
	}

//isbackground consumes a string and returns 1 if the string contains a & and 0 otherwise.
int isbackground(char* string){
	for(int i = 0; i < INPUTLENGTH;i++){
		if(string[i]=='&'){
			return 1;
		}
	}
	return 0;
}

//emptybackground looks at the running array and checks if any processes are running if so it returns 0,
//otherwise 1
int emptybackground(){
	for(int i =0; i < MAXBACK; i++){
		if(running[i]>0){
			return 0;
		}
	}
	return 1;
}

int main();

void usercommand(){
		for(int i =0; i < addedcommands; i++){
			printf("   %d. %s : User added command\n",commands[i].opnum,commands[i].commandname);
	}
}

void addcommand(char** command,char* name){
	strcpy(commands[addedcommands].stored[0],command[0]);
	strcpy(commands[addedcommands].stored[1],command[1]);
	strcpy(commands[addedcommands].commandname,name);
	commands[addedcommands].opnum = 3 + addedcommands;
	commands[addedcommands].background = 0;
	if(isbackground(name)){//if the command contains a &
		commands[addedcommands].background = 1;//set the background flag to reflect this
	}
	printf("Added with ID %d!\n",commands[addedcommands].opnum);
	addedcommands++;

}

//menu function simply prints out all the options avaliable to the user
void menu(){
printf("===== Mid-Day Commander, v2 ===== \n");
printf("G'day, Commander! What command would you like to run? \n");
printf("   0. whoami   : Prints out the result of the whoami command \n");
printf("   1. last     : Prints out the result of the last command \n");
printf("   2. ls       : Prints out the result of a listing on a user-specific path \n");
usercommand();
printf("   a. add command : Adds a new command to the menu \n");
printf("   c. change directory : Changes process working directory \n");
printf("   e. exit     : Leave Mid-Day Commander \n");
printf("   p. pwd      : Prints working directory \n");
printf("   r. running processes : Print list of running processes\n");
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

	printf("\n");
	printf("\n");
	printf("== Statistics == \n");
	printf("Elapsed time: %.3f milliseconds \n",runtime);
	printf("Page Faults: %d \n", minfaults);
	printf("Page Faults (reclaimed): %d \n", majfaults);

}
}

int mainprocess(char** command){
int z = waitpid(-1,NULL,WNOHANG);//check if any ended
if(z>0){//if so remove them from the array
	removebackground(z);
}
	if(leave){//if done exit
		return 0;
	}
menu();
int choice = fgetc(stdin);
getchar();
if(choice==EOF){
	printf("\n Logging out \n");
	return 0;
}
printf("\n");
switch(choice){//jump to execution based on choice
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
		strcpy(command[0],"ls");
		printf("Arguments?:");
		fgets(options,INPUTLENGTH,stdin);
		if(options[0]=='\n'){
		}
		else{
			for(int i =0; i < INPUTLENGTH;i++){
				if(options[i]=='\n'){
					options[i] = '\0';
				}
			}
			strcpy(command[current],options);
			current++;
		}

		printf("\n Path?:");
		fgets(path,INPUTLENGTH,stdin);
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

	case 97://add command case
		printf("== Add a command == \n");
		printf("Command to add?: ");
		char usercommand[INPUTLENGTH];
		fgets(usercommand,INPUTLENGTH,stdin);
		for(int i =0; i < INPUTLENGTH;i++){
			if(usercommand[i]=='\n'){
				usercommand[i] = '\0';
			}
			usercommand[INPUTLENGTH] = '\0';
		}		
		char name[INPUTLENGTH];
		strcpy(name,usercommand);

		//maybe add a function to tokenize input so its not so messy
		char* token = strtok(usercommand," ");
		strcpy(command[0],token);
		token = strtok(NULL, " ");
		strcpy(command[1],token);
		addcommand(command,name);
		break;

	case 101://exit case
		printf("\n Logging out \n");
		return 0;
		break;

	case 99://change directory
		printf("== Change Directory == \n");
		printf("New Directory?: ");
		char dir[INPUTLENGTH];
		fgets(dir,INPUTLENGTH,stdin);
		for(int i =0; i < INPUTLENGTH;i++){
			if(dir[i]=='\n'){
				dir[i]='\0';
			}
		}
		printf("Directory changing to: %s \n",dir);
		int i = chdir(dir);
		if(i==-1){
			printf("Error occured directory unchanged! \n");
			printf("Error: %s\n",strerror(errno));
		}
		break;

	case 112://pwd case
		printf("== Current Directory == \n");
		strcpy(command[0],"pwd");
		command[1] = NULL;
		run(command);
		break;

	case 114://r case where background processes are listed
		printf("== Background Processes==\n");
		printrunning();
		break;

	default://if someone enters input that is not valid
			choice = choice - 48;
			int invalid = 1;//invalid flag
			for(int i =0 ;i < 100; i++){
				if(commands[i].opnum==choice){//if the choice is an entered command
					invalid = 0;//not invalid if this path is chosen
					if(commands[i].background==0){//if the command is a normal command run it
						printf("== %s ==\n",commands[i].commandname);
						run(commands[i].stored);
						return 1;
					}
					else{//if the command is a background command
						int pid = fork();//fork into two processes
						if(pid==0){//child process, that runs the code
							run(commands[i].stored);//run the background task
							printf("\n");
							return 1;
						}
						else{//the parent process that monitors
							int index = addrunning(pid);//add the pid to the background queue
							if(index==-1){//if no room print an error
								printf("Background process array full!!!! will not be added!\n");
							}
							else{
								printf("Command ran with id: %d and pid: %d\n",index + 1,pid);
							}
							printf("\n");
							return 1;
					}
				}
			}
		}
		printf("\n");
		if(invalid){
			printf("Invalid input, try again! \n");
		}
	}

printf("\n");
return 1;
}
//main runs the prompt
int main(){

	for(int i =0; i < 100; i++){
			commands[i].stored = malloc(sizeof(char*) * 2);//allocate memory to store the command strings in
			for(int j = 0; j < 2; j++){
				commands[i].stored[j] = malloc(sizeof(char) * INPUTLENGTH);
			}
		commands[i].commandname = malloc(sizeof(char) * INPUTLENGTH);
		commands[i].opnum = -1;
	}

char ** command;
command = malloc(sizeof(char*) * MAXARGS);//allocate memory to store the command strings in
for(int i = 0; i < MAXARGS; i++){
	command[i] = malloc(sizeof(char) * INPUTLENGTH);
}

while(mainprocess(command)){

removebackground(removepid);

for(int i =0; i < MAXARGS; i++){
	free(command[i]);
}
free(command);
if(leave){
	break;
}

command = malloc(sizeof(char*) * MAXARGS);//allocate memory to store the command strings in
for(int i = 0; i < MAXARGS; i++){
	command[i] = malloc(sizeof(char) * INPUTLENGTH);
}
}

leave = 1;
if(emptybackground()==0){//if there are background processes
	printf("Waiting for background processes to terminate \n");
	waitpid(-1,0,0);
}

for(int i =0; i < MAXARGS; i++){
	free(command[i]);
}
free(command);


for(int i =0; i < 100; i++){
		for(int j = 0; j < 2; j++){
			free(commands[i].stored[j]);
		}
	free(commands[i].stored);
	free(commands[i].commandname);
}

return 0;
	}

