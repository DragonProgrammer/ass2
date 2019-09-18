#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define Out 0 // Define the ends of the pipes so i do not have to remember numbers
#define In 1
#define BufferS 15 //define buffer size
void alarmHandler(int sig) {
	fprintf(stderr, "Process killed\n");
	exit(-1);
}
void ParentF(int PipeOutFD, int PipeInFD){  //Takes as a parameter the output pipe end, and input pipe end
	char Buffer[BufferS]; // create tje buffer
	char Value[BufferS]="1"; // create the valeu array and initialize to 1
	long M =1; // create the cariable m and initilize
	write(PipeOutFD, Value, strlen(Buffer)); // write the value tp tje write end of the pipe

	printf("Parent process ready to run\nParent:\t\tValue = %ld\n", M); // print stuff
	while (M<85999999999){ //do this loop while M<99999999999
		if(read(PipeInFD, Value, BufferS)==-1){ //etempt to read for mthe read end of pipe
			printf("Parent read fail\n"); // if not kill process
			exit(-1);
		}
		else  // if so buffer is now Vlue
		{
			M = atol(Value); //make M the integer value of Value 
			M = M*3+7; //aritmatic
			printf("Parent:\t\tValue = %ld\n", M); // print value to screen
			sprintf(Buffer, "%ld", M);  // printing M to Buffer
			write(PipeOutFD, Buffer, strlen(Buffer)); // write buffer to write end

		}
	}
	return;// end function
}

void ChildF(int PipeOutFD, int PipeInFD){ //the child does the same as ParentF withough an initial writing to pipe
	char Buffer[BufferS];
	char Value[BufferS]="1";
	long M=1;
	printf("The CHild is ready to start\n");
	while (M<99999999999){
	printf("here\n");
		if(read(PipeInFD, Value, BufferS)==-1){
			printf("Child read fail\n");
			exit(-1);
		}
		else
		{
			M = atol(Value);
			M = M*2+5; // diferrnet arithmetic
			printf("Child:\t\tValue = %ld\n", M);
			sprintf(Buffer, "%ld", M);  // printing M to Buffer
			write(PipeOutFD, Buffer, strlen(Buffer));

		}
	}
	return;
}
void GranF(int PipeOutFD, int PipeInFD){ //Granchild funtion does the same as the Child function
	char Buffer[BufferS];
	char Value[BufferS]="1";
	long M=1;
	printf("The Granchild is ready to start\n");
	while (M<99999999999){
		if(read(PipeInFD, Value, BufferS)==-1){
			printf("Granchild read fail\n");
			exit(-1);
		}
		else
		{
			M = atol(Value);
			M = M*5+1; //diferent aritmetic
			printf("Granchild:\tValue = %ld\n", M);
			sprintf(Buffer, "%ld", M);  // printing M to Buffer
			write(PipeOutFD, Buffer, strlen(Buffer));

		}
	}
	return;
}

int main(int argc,char**argv){
signal(SIGALRM, alarmHandler);
	int partochild[2]; //create three int arrays to store pipe ends
	int childtogran[2];
	int grantopar[2];
	if(pipe(partochild)==-1){ // atempt to create parent to child pipe
		printf("Parent to child failed"); // if not kill prossess
		exit (-5);
	}
	if(pipe(childtogran)==-1){//attempt to create child to granchild pipe, if not quit
		printf("Child to Granchild failed");
		exit (-5);
	}
	if(pipe(grantopar)==-1){
		printf("Granchild to Parent failed");// Create last pipe, if not quit
		exit (-5);
	}
	pid_t child=fork();//creat first child
	if(child ==-1)
	{
		printf("Fork failed\n");// if fork fail quit
		exit(-5);
	}
	alarm(3);
	if(child){ // in parent
		close(partochild[Out]); // close unused pipe ends
		close(grantopar[In]);
		close(childtogran[In]);
		close(childtogran[Out]);	
		ParentF(partochild[In], grantopar[Out]);	//call parent function pass in pipe end to from granchild and to child

		wait(0); //wait for child to quit
		close(partochild[In]); // close the used pipe ends
		close(grantopar[Out]);
		exit(0); //quit parent

	}
	else{ // in child
		pid_t gran=fork(); //create granchild
		if(gran ==-1)// if not quit child
		{
			printf("Second Fork failed\n");
			exit(-5);
		}
		alarm(2);
		if(gran){
			close(partochild[In]);// cloose unsed pipes for child function
			close(childtogran[Out]);
			close(grantopar[In]);
			close(grantopar[Out]);
			ChildF(childtogran[In], partochild[Out]); //call child function pass in pipe end from parent and to grandchild
			wait(0);//wait for granchild to quit
			close(partochild[Out]);//close used pipe ends
			close(childtogran[In]);
			exit(0);// exit child
		}
		else{// in granchild
			close(childtogran[In]); // close unused pipe ends 
			close(grantopar[Out]);
			close(partochild[In]);
			close(partochild[Out]);
			GranF(grantopar[In], childtogran[Out]); // call granchild function pass in pipe to parent and from child

			close(childtogran[Out]);// close used pipe ends
			close(grantopar[In]);
			exit(0);//exit granchild
		}
	}
	return 0;// exit prcess
}

