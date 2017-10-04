#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char error_message[30] = "An error has occured\n";
int doWait = 0;

void changeDir(char *userInput){

}

void printWD(){

}

void exitProgram(){

}

//add forking here.
int callCommand(char *userInput){
  char *savePtr;
  char *splitInput = malloc(sizeof(char) * 129);
  char **args = malloc(sizeof(char *) * 65);
  splitInput = strtok_r(userInput, " ", &savePtr);
  int index = 0;
  while(splitInput!=NULL){
    args[index++] = splitInput;
    splitInput = strtok_r(NULL, " ", &savePtr);
  }
  args[index] = NULL;

  int pid = fork(), status;
  if((pid == 0) && (execvp(args[0], args) == -1)){
    return 0;
  }
  
  else if(doWait == 1) {
    wait(&status);
  }


  free(splitInput);
  free(args);
  return 1; 
}

int main(int argc, char *argv[]){
  if(argc != 1){
    fprintf(stderr, "Usage: ./mysh\n");
    exit(1);
  }
  int quit = 0, commandCount = 1;
  char userInput[129];
  while(!quit){
    printf("mysh (%d)> ", commandCount++);
    if(fgets(userInput, 129, stdin)==NULL){
      fprintf(stderr, error_message);
    }
    char *newlinePos = strchr(userInput, '\n');
    *newlinePos = '\0';
    char *andChar = strchr(userInput, '&');
    

    if(andChar != NULL) {
      doWait = 0;
      *andChar = '\0'; 
    } 
    else {
      doWait = 1;
    }

    if(strcmp("cd", userInput) == 0){
      changeDir(userInput);
    }
    else if(strcmp("pwd", userInput) == 0){
      printWD();
    }
    else if(strcmp("exit", userInput) == 0){
      exitProgram();
      quit = 1;
    }
    else if(userInput != NULL){
      if(!callCommand(userInput)){
        //debug extra newline here
        fprintf(stderr, error_message);
      }
      //everything BUT exit, cd, and pwd goes here
    }
  }
}
