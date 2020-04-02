#include "20171666.h"

command splitInput(char o[200]){ //split the input with space bar and ','
    char* ptr;
    int i=0;
    command result;

    result=initCommand();

    ptr=strtok(o," ");
    strcpy(result.order,ptr);

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result; //format is just order
    strcpy(result.first,ptr); //format is order x

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result;
    strcpy(result.second,ptr); //format is order x, y

    ptr=strtok(NULL," ,");
    if(ptr==NULL)
      return result;
    strcpy(result.third,ptr); //format is order x, y, z

    ptr=strtok(NULL," ");
    if(ptr!=NULL){
        strcpy(result.order,"-");
        return result;
    } // if there is more character, then it is not correct command

    return result;    
}

command initCommand(){ //init a new command struct
    command temp;
    
    strcpy(temp.order,"-");
    strcpy(temp.first,"-");
    strcpy(temp.second,"-");
    strcpy(temp.third,"-");

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
          printf("%s/\t",dlist->d_name); //if file is directory

        else if(S_ISREG(buf.st_mode)){
            if(buf.st_mode & S_IXUSR)
              printf("%s*\t",dlist->d_name); //if file is execute file

            else
              printf("%s\t",dlist->d_name); //other normal files
        }

        if(i==4){
            printf("\n");
            i=0;
        } //for make better view, print '\n' in every four print
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

    strcpy(temp->order,o); // make history struct
    temp->link=ed->link;
    ed->link=temp;
    ed=temp; //link to the history list
}

void orDump(command co, char o[200]){ //for order dump
    int i,j;
    int temp;

    orHistoryAdd(o);

    if(duAddr+160<MAXADDR){ //10 lines after duAddr in not exceed the last address
        orDumpPrint(duAddr,duAddr+159);
        duAddr+=160;
    }
    else{ //10 lines after duAddr exceed the last address
        orDumpPrint(duAddr,MAXADDR-1);
        duAddr=0;
    }

}

void orDumpStart(command co, char o[200]){ //for order format dump x
    int decstart;

    if(orCheckHex(co.first)==0){ // if start address is not hex
        printf("Error: wrong command\n");
        return;
    }

    decstart=(int)strtol(co.first,NULL,16);

    if(decstart<MAXADDR){
      orHistoryAdd(o); // add to the history
      if(decstart+160<MAXADDR)
        orDumpPrint(decstart, decstart+159); // 160 address after start don't exceed the last address
      else
        orDumpPrint(decstart, MAXADDR-1); //160 address after start exceed the last address
    }
    else{ //if start address exceed the last address
        printf("Error: over address\n");
        return;
    }
}

void orDumpStartEnd(command co, char o[200]){ //for order dump x, y
    int decstart, decend;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0){
        printf("Error: wrong command\n"); //if start and end address in not hex
        return;
    }

    decstart=(int)strtol(co.first,NULL,16);
    decend=(int)strtol(co.second,NULL,16);

    if(decstart<MAXADDR && decend<MAXADDR){
        if(decend<decstart){ //if end is smaller than start
            printf("Error: wrong command\n");
            return;
        }
        else{
            orHistoryAdd(o);
            orDumpPrint(decstart, decend);
        }
    }
    else{ // start or end address exceed the last address
        printf("Error: over address\n");
        return;
    }
}

void orDumpSelector(command co, char o[200]){ 
    // for select which dump command is
 
    if(strcmp(co.third,"-")!=0){
        printf("Error: too much command\n");
        return;
    } // if dump x, y, z is input. wrong command

    if(strcmp(co.second,"-")!=0){
        orDumpStartEnd(co, o);
        return;
    } // if dump x, y is input format.
    else if(strcmp(co.first,"-")!=0){
        orDumpStart(co, o);
        return;
    } // if dump x is input format.
    else{
        orDump(co, o);
        return;
    }    // if there is just dump    

}

void orDumpPrint(int start, int end){
    int i,j;
    int stline=(start/16)*16; // start line address
    int edline=(end/16)*16; // end line address
    int valuepoint=stline; // to store present address for print value
    int charpoint=stline; //to store present address for print char

    for(i=stline;i<=edline;i+=16){
        printf("%05X ",i); //print address

        for(j=0;j<16;j++){
            if(valuepoint<start || valuepoint>end){ // if present address is not in the range from start to end
                printf("   ");
            }
            else{
                printf("%02X ",memory[i/16][j]); // print values
            }
            valuepoint++;
        }

        printf(";");

        for(j=0;j<16;j++){
            if(charpoint<start || charpoint>end){ // if present address is not in the range from start to end
                printf(".");
            }
            else if(memory[i/16][j]>=0x20 && memory[i/16][j]<=0x7E){
                printf("%c",memory[i/16][j]);
            }
            else{ // if value is not in range from 0x20 to 0x7E, print '.'
                printf(".");
            }
            charpoint++;
        }

        printf("\n");
    }

}

int orCheckHex(char tok[10]){ // check whether tok is hex or not
    int i=0;

    while(1){
        if((tok[i]>=48 && tok[i]<=57) || (tok[i]>=65 && tok[i]<=70) || (tok[i]>=97 && tok[i]<=102)){ // tok is in the range of hex
            if(tok[i+1]!='\0') // if it isn't the last word
              i++;
            else
              break; // if reach to the last word then it is hex
        }
        else{
            return 0; // return 0 it is not hex
        }
    }
    
    return 1; //return 1 to tell that it is hex
}

void orEdit(command co, char o[200]){
    int addr;
    int value;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0){ // in format edit x, y / x or y is not a hex value
        printf("Error: wrong command\n");
        return;
    }
    if(strcmp(co.third,"-")!=0){ // if there is a third, it is not the format of edit
        printf("Error: wrong command\n");
        return;
    }

    addr=(int)strtol(co.first,NULL,16);
    value=(int)strtol(co.second,NULL,16);

    if(addr<MAXADDR){ // address don't exceed the last address
        if(value>0xff){ // value exceed the unsigned char range
            printf("Error: over value\n");
            return;
        }

        else{ //value don't exceed
            orHistoryAdd(o);
            memory[addr/16][addr%16]=value;
        }

    }
    else{ // address exceed the last address
        printf("Error: over address\n");
        return;
    }

}

void orFill(command co, char o[200]){
    int start, end, value;
    int i;

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0 || orCheckHex(co.third)==0){ // in the format fill x, y, z / x or y or z is not hex
        printf("Error: wrong command\n");
        return;
    }

    start=(int)strtol(co.first,NULL,16);
    end=(int)strtol(co.second,NULL,16);
    value=(int)strtol(co.third,NULL,16);

    if(start<MAXADDR && end<MAXADDR){
        if(end<start){ // end address < start address
            printf("Error: wrong command\n");
            return;
        }
        else if(value>0xff){ // value exceed unsigned char range
            printf("Error: over value\n");
            return;
        }
        else{
            for(i=start;i<=end;i++){
                memory[i/16][i%16]=value;
            } //fill the table
            orHistoryAdd(o);
        }

    }
    else{ // start and end address exceed the last address
        printf("Error: over address\n");
        return;
    }
      
}

void orReset(){ // for reset the table
    int i,j;

    for(i=0;i<65536;i++){
        for(j=0;j<16;j++){
            memory[i][j]=0;
        }
    }
}

void hashMake(){ // for make hash table
    int i;
    int point;
    int scan;
    char op[10];
    char mnem[10];
    char form[10];
    FILE* fp=fopen("opcode.txt","r"); // open the file

    if(fp==NULL){
        printf("No files\n");
        return;
    } // there is no file

    while(1){
        scan=fscanf(fp,"%s",op);

        if(scan==EOF)
          break; // reach to EOF, then stop the read

        fscanf(fp,"%s",mnem);
        fscanf(fp,"%s",form);

        point=((int)mnem[0])%20; // get hash key

        hashAdd(op,mnem,form,point); // add to hash table
    }

    fclose(fp);
}

void hashAdd(char op[10], char mnem[10], char form[10], int key){
    hash* temp=(hash*)malloc(sizeof(hash));
    
    strcpy(temp->op, op);
    strcpy(temp->mnem, mnem);
    strcpy(temp->form, form); // make hash struct

    temp->link=hTable[key].link;
    hTable[key].link=temp; // link to the hash table
}

void orOpcode(command co, char o[200]){ // for opcode 
    int inx=((int)co.first[0])%20; //find hash key
    hash* point;

    if(co.first[0]>90 || co.first[0]<65){ // if mnemonic is not capital letters
        printf("Error: wrong command\n");
        return;
    }

    point=hTable[inx].link;

    while(1){
        if(point==NULL)
          break;

        if(strcmp(point->mnem,co.first)==0){ // if mnemonic is match to the hash table
            orHistoryAdd(o);
            printf("opcode is %s\n\n",point->op); 
            return;
        }

        point=point->link;
    }

    printf("Error: there is no opcode\n");


}

void orOpcodeList(){ //for print opcodelist
    hash* point;
    int i;

    printf("\n");

    for(i=0;i<20;i++){
        point=hTable[i].link;

        printf("%d : ",i);

        while(1){
            if(point==NULL)
              break;

            if(point->link==NULL) // last print of hash list
              printf("[%s,%s]",point->mnem, point->op);
            else
              printf("[%s,%s] -> ",point->mnem, point->op);
            point=point->link;
        }
        printf("\n");
    }
    printf("\n");
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
      hTable[i].link=NULL; // init the hash table
  }

  hashMake(); // make hash table

  while(1){
      printf("sicsim> ");

      scanf("%[^\n]s",input);
      getchar();
      
      strcpy(savein,input); // save the whole user input
      co=splitInput(input); //split the input then save to the command struct

      if(strcmp(co.order,"help")==0 || strcmp(co.order,"h")==0){ //if command is help
          orHistoryAdd(savein);
          orHelp();
      }
      else if(strcmp(co.order,"d")==0 || strcmp(co.order,"dir")==0){ // if command is dir
          orHistoryAdd(savein);
          orDir();
      }
      else if(strcmp(co.order,"q")==0 || strcmp(co.order,"quit")==0){ // if command is quit
          orHistoryAdd(savein);
          orQuit();
      }
      else if(strcmp(co.order,"hi")==0 || strcmp(co.order,"history")==0){ //if command is history
          orHistoryAdd(savein);
          orHistory();
      }
      else if(strcmp(co.order,"du")==0 || strcmp(co.order,"dump")==0){ // if command is dump
          orDumpSelector(co, savein);
      }
      else if(strcmp(co.order,"e")==0 || strcmp(co.order,"edit")==0){ // if command is edit
          orEdit(co,savein);
      }
      else if(strcmp(co.order,"f")==0 || strcmp(co.order,"fill")==0){ // if command is fill
          orFill(co,savein);
      }
      else if(strcmp(co.order,"reset")==0){ // if command is reset
          orHistoryAdd(savein);
          orReset();
      }
      else if(strcmp(co.order,"opcode")==0){ // if command is opcode
          orOpcode(co, savein);
      }
      else if(strcmp(co.order,"opcodelist")==0){ // if command is opcodelist
          orHistoryAdd(savein);
          orOpcodeList();
      }


      else{ // undefined command 
          printf("Error: wrong command\n");
      }
      
  }
}
