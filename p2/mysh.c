#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char error_message[30] = "An error has occured\n";

void changeDir(char *userInput){

}

void printWD(){

}

void exitProgram(){

}

int callCommand(char *userInput){
  printf("&s\n", userInput);
  char *splitInput = malloc(sizeof(char) * 129);
  char **args = malloc(sizeof(char *) * 65);
  splitInput = strtok_r(userInput, " ", &splitInput);
  int index = 0;
  while(splitInput!=NULL){
    args[index++] = splitInput;
    splitInput = strtok_r(NULL, " ", &splitInput);
  }
  char pathPrefix[6] = "/bin/";
  char *path = strcat(pathPrefix, args[0]);
  if(execv(path, args) == -1){
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]){
  if(argc != 1){
    fprintf(stderr, "Usage: ./mysh");
    exit(1);
  }
  int quit = 0, commandCount = 0;
  char userInput[129];
  while(!quit){
    printf("\nmysh (%d)> ", commandCount++);
    if(fgets(userInput, 129, stdin)==NULL){
      fprintf(stderr, error_message);
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
        fprintf(stderr, error_message);
      }
      //everything BUT exit, cd, and pwd goes here
    }
  }
}
