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
#define MAXCOMMANDS 100

//struct that stores commands
struct Command{
char** stored;//the entire command argv for exec
char* commandname;//the string it's refered to as
int opnum;//the number command it is
};



int addedcommands = 0;//tracker for total number of commands added

struct Command commands[100];//hold 100 commands
int firstrun = 1;//flag used to denote first run


//usercommand prints out all the user added commands by using the commands storage and the addedcommands
//number to loop through and print. Prints nothing on the first run
void usercommand(){
	if(firstrun!=1){//if not the first run because storage will not have been created yet
		for(int i =0; i < addedcommands; i++){
			printf("   %d. %s : User added command\n",commands[i].opnum,commands[i].commandname);
		}
	}
}

//addcommand takes a command and its name and adds it to the menu and storage. If it is the first
//time this function has been called, then it will first create the storage for all the commands. Otherwise
//it simply adds the command to the storage with a specific opnum to call it by.
void addcommand(char** command,char* name){
	if(firstrun){//create the storage for the commands
		firstrun = 0;//change firstrun to 0 so this doesn't realloc every call
		for(int i =0; i < MAXCOMMANDS; i++){
			commands[i].stored = malloc(sizeof(char*) * 2);//allocate memory to store the command strings in
			for(int j = 0; j < 2; j++){
				commands[i].stored[j] = malloc(sizeof(char) * INPUTLENGTH);
			}
		commands[i].commandname = malloc(sizeof(char) * INPUTLENGTH);
	}
	}
	//add the command to a command struct in the storage
	strcpy(commands[addedcommands].stored[0],command[0]);
	strcpy(commands[addedcommands].stored[1],command[1]);
	strcpy(commands[addedcommands].commandname,name);
	commands[addedcommands].opnum = 3 + addedcommands;
	printf("Added with ID %d!\n",commands[addedcommands].opnum);
	addedcommands++;//increment the commands to create unique opnums

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

	struct rusage r;//usage variables start and end to monitor resource usage
	gettimeofday(&start,NULL);//start timer
	
	
	wait4(-1,NULL,0,&r);//wait for the child to terminate

	gettimeofday(&end,NULL);//end clock

	double runtime = (end.tv_usec - start.tv_usec) * .001;//calculate data and print it
	int minfaults = (r.ru_minflt);
	int majfaults = (r.ru_majflt);

	printf("\n");
	printf("== Statistics == \n");
	printf("Elapsed time: %.3f milliseconds \n",runtime);
	printf("Page Faults: %d \n", majfaults);
	printf("Page Faults (reclaimed): %d \n", minfaults);

}
}

//main runs the prompt
int main(){
menu();//calls menu to print
int exit = 0;//flag to denote if someone wants to exit
char ** command;
command = malloc(sizeof(char*) * MAXARGS);//allocate memory to store the command strings in
for(int i = 0; i < MAXARGS; i++){
	command[i] = malloc(sizeof(char) * INPUTLENGTH);
}
int choice = fgetc(stdin);
getchar();
if(choice==EOF){//if the test file ends, then set choice to exit
	choice=101;//ascii code for exit
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

	case 50://ls branch check 
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

	case 97://add command case
		printf("== Add a command == \n");
		printf("Command to add?: ");
		char usercommand[INPUTLENGTH];
		fgets(usercommand,INPUTLENGTH,stdin);
		for(int i =0; i < INPUTLENGTH;i++){//similar to ls input if a \n is detected terminate string there
			if(usercommand[i]=='\n'){
				usercommand[i] = '\0';
			}
			usercommand[INPUTLENGTH] = '\0';//terminate string at the end incase fgets didn't
		}		
		char name[INPUTLENGTH];
		strcpy(name,usercommand);//copy over the name

		char* token = strtok(usercommand," ");//tokenize string to split at spaces
		strcpy(command[0],token);//copy over the first token as the command
		token = strtok(NULL, " ");//go back and tokenize again
		strcpy(command[1],token);//all other agruments go into here
		addcommand(command,name);//add the command we just tokenized and created
		break;

	case 101://exit case
		exit = 1;//denote flag to exit
		break;

	case 99://change directory
		printf("== Change Directory == \n");
		printf("New Directory?: ");
		char dir[INPUTLENGTH];
		fgets(dir,INPUTLENGTH,stdin);
		for(int i =0; i < INPUTLENGTH;i++){//check any user input above, same loop
			if(dir[i]=='\n'){
				dir[i]='\0';
			}
		}
		printf("Directory changing to: %s \n",dir);
		int i = chdir(dir);//change the directory to user input
		if(i==-1){//if it cant print out and error message and print out the errno code
			printf("Error occured directory unchanged! \n");
			printf("Error: %s\n",strerror(errno));
		}
		break;

	case 112://pwd case
		printf("== Current Directory == \n");
		strcpy(command[0],"pwd");//copy over command pwd to print directory
		command[1] = NULL;
		run(command);//run it
		break;

	default://if someone enters input that is not valid
		if(firstrun!=1){//if add command has been called
			choice = choice - 48;//subrtact 48 which is 0 in ascii to simply check if the input
			//was a user added command
			for(int i =0 ;i < MAXCOMMANDS; i++){
				if(commands[i].opnum==choice){//if a command opnum==the choice, meaning the command was added
					printf("== %s ==\n",commands[i].commandname);
					run(commands[i].stored);//run it
				}
			}
		}
		else{//otherwise its invalid input
		printf("\n");
		printf("Invalid input, try again! \n");
	}
}

//free the command string
for(int i =0; i < MAXARGS; i++){
		free(command[i]);
}
free(command);
printf("\n");
if(exit==0){//if continuing run main again
	main();
}
else{
//otherwise free all the memory for the storage
printf("Logging out!\n");

if(firstrun==0){
	for(int i =0; i < MAXCOMMANDS; i++){
		free(commands[i].stored[0]);
		free(commands[i].stored[1]);
		free(commands[i].commandname);
		free(commands[i].stored);
}//and exit
}
}
return 0;
	}

