#include <stdio.h>

int running[20] = {20,12,14,25,26,78};

int numprocesses = 6;

void printarr(){
	for(int i =0; i < numprocesses; i++){
			printf("[%d] PID: %d\n",i+1,running[i]);
		}
	}


void removebackground(int pid){
	for(int i =0; i < 20; i++){
		if(running[i]==pid){
			running[i] = 0;
			numprocesses--;
		}
	}
	int index;
	for(int i =0; i < 20; i++){
		if(running[i]==0){
			index = i;
				}
			}
		if(index = 0){
			running[0] = running[1];
		}
	for(int i = index; i < 19; i++){
		running[i] = running[i+1];
		}
	}

int main(){
	//printarr();
	removebackground(20);
	printf("\n");
	printarr();
	}