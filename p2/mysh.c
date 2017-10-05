#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

const char error_message[30] = "An error has occured\n";
int doWait = 0;
char **parsedArgs;
void parseInput(char *userInput){
  char *savePtr;
  char *splitInput = malloc(sizeof(char) * 129);
  char **args = malloc(sizeof(char *) * 2);
  splitInput = strtok_r(userInput, " ", &savePtr);
  int index = 0;
  while(splitInput!=NULL){
    args[index++] = splitInput;
    splitInput = strtok_r(NULL, " ", &savePtr);
  }
  free(splitInput);
  parsedArgs = args;
}
void changeDir(char **args){
  if(args[1] != NULL && strlen(args[1])>0){
    if(args[1][0] == '/'){
      chdir(args[1]);
    }else{
      char directory[129];
      getcwd(directory, 128);
      strcat(directory, "/");
      char *dest = strcat(directory, args[1]);
      chdir(dest);
    }
  }else{
    chdir(getenv("HOME"));
  }
}

void printWD(){
  char directory[129];
  printf("%s\n", getcwd(directory, 128));
}

void exitProgram(){
  exit(1);
}

//add forking here.
int callCommand(char **args){
  int pid = fork(), status;
  if((pid == 0) && execvp(args[0], args) == -1){
    fprintf(stderr, error_message);
    kill(getpid(), SIGTERM);
  }
  else if(doWait == 1) {
    wait(&status);
  }
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
    fflush(stdout);
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
    parseInput(userInput);
    if(strcmp("cd", parsedArgs[0]) == 0){
      changeDir(parsedArgs);
    }
    else if(strcmp("pwd", parsedArgs[0]) == 0){
      printWD();
    }
    else if(strcmp("exit", parsedArgs[0]) == 0){
      exitProgram();
      quit = 1;
    }
    else if(userInput != NULL){
      callCommand(parsedArgs);
    }
    free(parsedArgs);
  }
}
