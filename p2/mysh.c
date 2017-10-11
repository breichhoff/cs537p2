#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

const char error_message[30] = "An error has occured\n";
int doWait = 0;
int pipedIndex = 0;
char **parsedArgs;
char *outFile;
char *inFile;
char **firstCommand;
char **secondCommand;

void parseInput(char *userInput){
  int nextIsOut = 0, nextIsIn = 0;
  char *savePtr;
  char *splitInput = malloc(sizeof(char) * 129);
  parsedArgs = malloc(sizeof(char *) * 64);
  splitInput = strtok_r(userInput, " ", &savePtr);
  int index = 0;
  while(splitInput!=NULL){
    if(strcmp("|", splitInput) == 0){
      pipedIndex = index;
      splitInput = strtok_r(NULL, " ", &savePtr);
      continue;
    }
    else if(strcmp(">", splitInput) == 0){
      nextIsOut = 1;
      splitInput = strtok_r(NULL, " ", &savePtr);
      continue;
    }
    else if(strcmp("<", splitInput) == 0){
      nextIsIn = 1;
      splitInput = strtok_r(NULL, " ", &savePtr);
      continue;
    }
    if(nextIsOut){
      outFile = splitInput;
      nextIsOut = 0;
      continue;
    }
    else if(nextIsIn){
      inFile = splitInput;
      nextIsIn = 0;
      continue;
    }
    parsedArgs[index++] = splitInput;
    splitInput = strtok_r(NULL, " ", &savePtr);
  }
  free(splitInput);
  if(pipedIndex > 0){
    firstCommand = malloc(sizeof(char *) * pipedIndex);
    int i;
    for(i = 0; i < pipedIndex; i++){
      firstCommand[i] = parsedArgs[i];
    }
    secondCommand = malloc(sizeof(char *) * (129 - pipedIndex));
    for(i = 0; i < (index - pipedIndex); i++){
      secondCommand[i] = parsedArgs[i + pipedIndex];
    }
  }
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

void callPipedCommand(){
  int files[2];
  pipe(files);
  int pid = fork(), status, spid;
  if(pid != 0){
    spid = fork();
  }
  if(pid == 0 && spid != 0){ //first command  change output file
    close(files[0]);
    dup2(files[1], 1);
    if(execvp(firstCommand[0], firstCommand) < 0){
      fprintf(stderr, error_message);
      kill(getpid(), SIGTERM);
    }

  } else if(spid == 0){ //second command - change input file
    char buf[1000];
    close(files[1]);
    dup2(files[0], 0);
    read(files[0], buf, 1000);
    printf("%s\n", buf);
    // close(files[0]);
    if(execvp(secondCommand[0], secondCommand) < 0){
      fprintf(stderr, error_message);
      kill(getpid(), SIGTERM);
    }
  } else{
    wait(&status);
    // close(files[0]);
    // close(files[1]);
    free(firstCommand);
    free(secondCommand);
    pipedIndex = 0;
  }
}
//add forking here.
void callCommand(char **args){
  int pid = fork(), status, in, out;
  if(pid == 0){
    if(inFile){
      in = open(inFile, O_RDONLY);
      dup2(in, 0);
      close(in);
    }
    if(outFile){
      out = open(outFile, O_WRONLY|O_CREAT|O_TRUNC);
      dup2(out, 1);
      close(out);
    }
  }
  if((pid == 0) && execvp(parsedArgs[0], parsedArgs) == -1){
    printf("ici\n");
    fprintf(stderr, error_message);
    kill(getpid(), SIGTERM);
  }
  else if(doWait == 1) {
    wait(&status);
    // close(in);
    // close(out);
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
      if(pipedIndex > 0){
        callPipedCommand();
      } else{
        callCommand(parsedArgs);
      }
    }
    free(parsedArgs);
    // parsedArgs = NULL;
  }
}
