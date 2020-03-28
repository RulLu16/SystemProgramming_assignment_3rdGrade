#include "20171666.h"

command splitInput(char o[200]){
    char* ptr;
    int i=0;
    command result;

    result=initCommand();

    ptr=strtok(o," ");
    strcpy(result.order,ptr);

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result; //just order
    strcpy(result.first,ptr); //order x

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result;
    strcpy(result.second,ptr); //order x, y

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result;
    strcpy(result.third,ptr); //order x, y, z

    ptr=strtok(NULL," ");
    if(ptr!=NULL){
        strcpy(result.order,"-");
        return result;
    } // if there is more character so it is not correct command

    return result;    
}

command initCommand(){
    command temp;
    
    strcpy(temp.order,"-");
    strcpy(temp.first,"-");
    strcpy(temp.second,"-");
    strcpy(temp.third,"-");

      //printf("%s %s %s %s",temp.order, temp.first, temp.second, temp.third);
    return temp;
}

void orHelp(){ //for order 'help'
    printf("\nh[elp]\nd[ir]\nq[uit]\nhi[story]\n");
    printf("du[mp] [start, end]\ne[dit] address, value\n");
    printf("f[ill] start, end, value\nreset\nopcode mnemonic\n");
    printf("opcodelist\n\n");
}

void orDir(){ //for order dir
    DIR* dir=NULL;
    struct dirent* dlist=NULL;
    struct stat buf;
    int i=0;

    if((dir=opendir("."))==NULL){
        printf("No such directory\n");
        return;
    } //if there is no file

    while((dlist=readdir(dir))!=NULL){
        i++;
        lstat(dlist->d_name, &buf);

        if(S_ISDIR(buf.st_mode))
          printf("%s/\t",dlist->d_name); // directory name

        else if(S_ISREG(buf.st_mode)){
            if(buf.st_mode & S_IXUSR)
              printf("%s*\t",dlist->d_name); // execute file name

            else
              printf("%s\t",dlist->d_name); // normal file name
        }

        if(i==4){
            printf("\n");
            i=0;
        } //for make better view
    }
    printf("\n");

    closedir(dir);

}

void orQuit(){ //for order 'quit'
    exit(0);
}

void orHistory(){ //for order 'history'
    His* point=st->link;
    int temp=1;

    printf("\n");
    while(1){
        if(point==NULL)
          break;

        printf("%d\t%s\n",temp,point->order);
        point=point->link;
        temp++;
    }
    printf("\n");

}

void orHistoryAdd(char o[200]){ //to add another history
    His* temp=(His*)malloc(sizeof(His));
    strcpy(temp->order,o);
    temp->link=ed->link;
    ed->link=temp;
    ed=temp;
}

void orDump(command co, char o[200]){ //for order dump
    printf("dump!\n");
}

void orDumpStart(command co, char o[200]){ //for order dump x
    printf("dump x\n");
}

void orDumpStartEnd(command co, char o[200]){ //for order dump x, y
    printf("dump x, y\n");
}

void orDumpSelector(command co, char o[200]){ 
    // for select which dump this command is
    
    if(strcmp(co.third,"-")!=0){
        printf("Error: too much command\n");
        return;
    } // if dump x, y, z is input. wrong command

    if(strcmp(co.second,"-")!=0){
        orDumpStartEnd(co, o);
        return;
    }
    else if(strcmp(co.first,"-")!=0){
        orDumpStart(co, o);
        return;
    }
    else{
        orDump(co, o);
        return;
    }        

}

int main(){
  int i,j;
  int start, end;
  char input[200];
  command co;

  st=(His*)malloc(sizeof(His));
  st->link=NULL;
  ed=st;

  while(1){
      printf("sicsim> ");

      scanf("%[^\n]s",input);
      getchar();
      //printf("%s",order);

      co=splitInput(input);

      //printf("%s %s %s %s",co.order, co.first, co.second, co.third);

      if(strcmp(co.order,"help")==0 || strcmp(co.order,"h")==0){
          orHistoryAdd(input);
          orHelp();
      }
      else if(strcmp(co.order,"d")==0 || strcmp(co.order,"dir")==0){
          orHistoryAdd(input);
         orDir();
      }
      else if(strcmp(co.order,"q")==0 || strcmp(co.order,"quit")==0){
          orHistoryAdd(input);
          orQuit();
      }
      else if(strcmp(co.order,"hi")==0 || strcmp(co.order,"history")==0){
          orHistoryAdd(input);
          orHistory();
      }
      else if(strcmp(co.order,"du")==0 || strcmp(co.order,"dump")==0){
          orDumpSelector(co, input);
      }
      else if(strcmp(co.order,"e")==0 || strcmp(co.order,"edit")==0){
      }
      else if(strcmp(co.order,"f")==0 || strcmp(co.order,"fill")==0){
      }
      else if(strcmp(co.order,"reset")==0){
      }
      else if(strcmp(co.order,"opcode")==0){
      }
      else if(strcmp(co.order,"opcodelist")==0){
      }


      else{
          printf("error\n");
      }
      
  }
}
