int main(int argc, char *argv[]){
  int quit = 0;
  while(!quit){
    switch(userinput){
      case "cd":
        changeDir(userInput);
      break;
      case "pwd":
        printWD();
      break;
      case "exit":
        exitProgram();
        quit = 1;
      break;
      default:
      //everything BUT exit, cd, and pwd goes here
      break;
    }
  }
}

void changeDir(char *userInput){

}

void printWD(){

}

void exitProgram(){

}
