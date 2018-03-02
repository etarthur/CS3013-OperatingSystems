Compiling supports make all and make clean. Individuals can be made through make [mc0|mc1|mc2].
To run simply enter ./mc0 or whatever variant you are running. This program supports stdin piping therefore it can also be run as ./mc0 < test.txt where test.txt is a test document following the format of the ones included in this zip.

The program will not terminate if you have multiple background tasks running and it encounters EOF or e, this is because when a background task terminates it interrupts the menu and therefore reprints it. Therefore to fully exit the program hit enter or EOF and the program will tell you its waiting. Once all the tasks are finished another prompt will appear, simply enter e and it will safely exit. Apologies but I cannot figure out how to debug this, so unfortunately it's there.

Occasionally it will print logging out and then stay there, simply press enter and it should end the program not sure why this bug exists either.

There are no test cases for mc0 as it doesn't support the exit function and therefore can only be terminated by ctrl-c!

Data structures:
Only 100 user commands can be added, and only 20 background tasks are possible at a time, this number can easily be changed in the top of mc2, simply change MAXBACK to a different value and it will work.


The Command data structure is outlined in mc1 and mc2, it simply contains all the relevant information about a command. 

The running array is an array that contains the PID of all running background processes. There are many functions that work with this that will be explain below:

In the array the value 0 denotes that no PID is currently running in that slot.

The array functions as a queue, as elements are not added a certain position by the program, instead they are added on the back or open spot and when an element is removed the whole array shifts to compensate.

Whenever the mainprocess forks in order to run a background command that PID is added to the array, and at the start of mainprocess when it prompts for input it loops through and removes PIDs from the array that have terminated. 

Functions that support this structure:
(all at the top of mc2.c)

addrunning(int pid):
takes a pid and adds it to the array by searching through and adding it to the first index found with value zero. This guarentees that it will be added in the correct order at the back, and then we don't have to worry about the array size. If it returns -1, the array is full and no more processes can be tracked. They still may run just not tracked.

printrunning():
simply loops through and prints all running processes ids and pids

removebackground(int pid):
takes a pid and removes it from the array by looping through and setting that index to zero. Then it will shift all values after the removed one down by one, this acts as a queue pop, except it can be from any position.

isbackground(char* string):
consumes a string and return 1 if there is an & in it, otherwise 0

emptybackground():
returns 0 if there are processes running in the background, if all are completed return 1.


