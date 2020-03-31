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
    int i,j;
    int temp;

    orHistoryAdd(o);

    if(duAddr+160<MAXADDR){
        orDumpPrint(duAddr,duAddr+159);
        duAddr+=160;
    }
    else{
        orDumpPrint(duAddr,MAXADDR-1);
        duAddr=0;
    }

}

void orDumpStart(command co, char o[200]){ //for order dump x
    int decstart;

    if(orCheckHex(co.first)==0){
        printf("Error: wrong command\n");
        return;
    }

    decstart=(int)strtol(co.first,NULL,16);

    if(decstart<MAXADDR){
      orHistoryAdd(o);
      if(decstart+160<MAXADDR)
        orDumpPrint(decstart, decstart+159);
      else
        orDumpPrint(decstart, MAXADDR-1);
    }
    else{
        printf("Error: over address\n");
        return;
    }
}

void orDumpStartEnd(command co, char o[200]){ //for order dump x, y
    int decstart, decend;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0){
        printf("Error: wrong command\n");
        return;
    }

    decstart=(int)strtol(co.first,NULL,16);
    decend=(int)strtol(co.second,NULL,16);

    if(decstart<MAXADDR && decend<MAXADDR){
        if(decend<decstart){
            printf("Error: wrong command\n");
            return;
        }
        else{
            orHistoryAdd(o);
            orDumpPrint(decstart, decend);
        }
    }
    else{
        printf("Error: over address\n");
        return;
    }
}

void orDumpSelector(command co, char o[200]){ 
    // for select which dump this command is
  // printf("%s",o); 
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

void orDumpPrint(int start, int end){
    int i,j;
    int stline=(start/16)*16;
    int edline=(end/16)*16;
    int valuepoint=stline;
    int charpoint=stline;

    for(i=stline;i<=edline;i+=16){
        printf("%05X ",i);

        for(j=0;j<16;j++){
            if(valuepoint<start || valuepoint>end){
                printf("   ");
            }
            else{
                //printf("%d",memory[i/16][j]);
                printf("%02X ",memory[i/16][j]);
            }
            valuepoint++;
        }

        printf(";");

        for(j=0;j<16;j++){
            if(charpoint<start || charpoint>end){
                printf(".");
            }
            else if(memory[i/16][j]>=0x20 && memory[i/16][j]<=0x7E){
                printf("%c",memory[i/16][j]);
            }
            else{
                printf(".");
            }
            charpoint++;
        }

        printf("\n");
    }

}

int orCheckHex(char tok[10]){
    int i=0;

    while(1){
        if((tok[i]>=48 && tok[i]<=57) || (tok[i]>=65 && tok[i]<=70) || (tok[i]>=97 && tok[i]<=102)){
            if(tok[i+1]!='\0')
              i++;
            else
              break;
        }
        else{
            return 0;
        }
    }
    
    return 1;
}

void orEdit(command co, char o[200]){
    int addr;
    int value;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0){
        printf("Error: wrong command\n");
        return;
    }
    if(strcmp(co.third,"-")!=0){
        printf("Error: wrong command\n");
        return;
    }

    addr=(int)strtol(co.first,NULL,16);
    value=(int)strtol(co.second,NULL,16);

    if(addr<MAXADDR){
        if(value>0xff){
            printf("Error: over value\n");
            return;
        }

        else{
            orHistoryAdd(o);
            memory[addr/16][addr%16]=value;
        }

    }
    else{
        printf("Error: over address\n");
        return;
    }

}

void orFill(command co, char o[200]){
    int start, end, value;
    int i;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0 || orCheckHex(co.third)==0){
        printf("Error: wrong command\n");
        return;
    }

    start=(int)strtol(co.first,NULL,16);
    end=(int)strtol(co.second,NULL,16);
    value=(int)strtol(co.third,NULL,16);

    if(start<MAXADDR && end<MAXADDR){
        if(end<start){
            printf("Error: wrong command\n");
            return;
        }
        else if(value>0xff){
            printf("Error: over value\n");
            return;
        }
        else{
            for(i=start;i<=end;i++){
                memory[i/16][i%16]=value;
            }
            orHistoryAdd(o);
        }

    }
    else{
        printf("Error: over address\n");
        return;
    }
      
}

void orReset(){
    int i,j;

    for(i=0;i<65536;i++){
        for(j=0;j<16;j++){
            memory[i][j]=0;
        }
    }
}

void hashMake(){
    int i;
    int point;
    int scan;
    char op[10];
    char mnem[10];
    char form[10];
    FILE* fp=fopen("opcode.txt","r");

    if(fp==NULL){
        printf("No files\n");
        return;
    }

    while(1){
        scan=fscanf(fp,"%s",op);

        if(scan==EOF)
          break;

        fscanf(fp,"%s",mnem);
        fscanf(fp,"%s",form);

        point=(int)mnem[0]-65;

        if(point>=20)
          point=hashFind();

        hashAdd(op,mnem,form,point);
    }

    /*for(i=0;i<20;i++){
        if(hTable[i].link!=NULL){
            printf("%s\n",hTable[i].link->mnem);
        }
        else
          printf("null\n");
    }*/

    fclose(fp);
}

void hashAdd(char op[10], char mnem[10], char form[10], int key){
    hash* temp=(hash*)malloc(sizeof(hash));
    
    strcpy(temp->op, op);
    strcpy(temp->mnem, mnem);
    strcpy(temp->form, form);

    temp->link=hTable[key].link;
    hTable[key].link=temp;
}

int hashFind(){
    int i=0;
    int result;

    while(i<20){
        if(hTable[i].link==NULL){
            result=i;
            break;
        }
        i++;
    }

    if(i==20)
      return 19;
    else
      return result;
}

void orOpcode(command co, char o[200]){
    int inx=co.first[0]-65;
    hash* point;

    if(co.first[0]>90 || co.first[0]<65){
        printf("Error: wrong command\n");
        return;
    }

    if(inx>=20){
        inx=opFind(co.first);
    }
    //printf("%d\n",inx);
    point=hTable[inx].link;

    while(1){
        if(point==NULL)
          break;

        if(strcmp(point->mnem,co.first)==0){
            orHistoryAdd(o);
            printf("opcode is %s\n",point->op);
            return;
        }

        point=point->link;
    }

    printf("Error: there is no opcode\n");


}

int opFind(char op[10]){
    int i;

    for(i=0;i<20;i++){
        if(hTable[i].link->mnem[0] == op[0])
          return i;
    }

    return 19;
}

void orOpcodeList(){
    hash* point;
    int i;

    for(i=0;i<20;i++){
        point=hTable[i].link;

        printf("%d : ",i);

        while(1){
            if(point==NULL)
              break;

            if(point->link==NULL)
              printf("[%s,%s]",point->mnem, point->op);
            else
              printf("[%s,%s] -> ",point->mnem, point->op);
            point=point->link;
        }
        printf("\n");
    }
}

int main(){
  int i,j;
  char input[200];
  char savein[200];
  command co;

  st=(His*)malloc(sizeof(His));
  st->link=NULL;
  ed=st;

  hTable=(hash*)malloc(sizeof(hash)*20);

  for(i=0;i<20;i++){
      //printf("dd");
      hTable[i].link=NULL;
  }

  hashMake();

  while(1){
      printf("sicsim> ");

      scanf("%[^\n]s",input);
      getchar();
     // printf("%s",input);
      
      strcpy(savein,input);
      co=splitInput(input);

      if(strcmp(co.order,"help")==0 || strcmp(co.order,"h")==0){
          orHistoryAdd(savein);
          orHelp();
      }
      else if(strcmp(co.order,"d")==0 || strcmp(co.order,"dir")==0){
          orHistoryAdd(savein);
          orDir();
      }
      else if(strcmp(co.order,"q")==0 || strcmp(co.order,"quit")==0){
          orHistoryAdd(savein);
          orQuit();
      }
      else if(strcmp(co.order,"hi")==0 || strcmp(co.order,"history")==0){
          orHistoryAdd(savein);
          orHistory();
      }
      else if(strcmp(co.order,"du")==0 || strcmp(co.order,"dump")==0){
          orDumpSelector(co, savein);
      }
      else if(strcmp(co.order,"e")==0 || strcmp(co.order,"edit")==0){
          orEdit(co,savein);
      }
      else if(strcmp(co.order,"f")==0 || strcmp(co.order,"fill")==0){
          orFill(co,savein);
      }
      else if(strcmp(co.order,"reset")==0){
          orHistoryAdd(savein);
          orReset();
      }
      else if(strcmp(co.order,"opcode")==0){
          orOpcode(co, savein);
      }
      else if(strcmp(co.order,"opcodelist")==0){
          orHistoryAdd(savein);
          orOpcodeList();
      }


      else{
          printf("error\n");
      }
      
  }
}
