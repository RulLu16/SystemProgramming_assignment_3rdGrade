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
    printf("opcodelist\n");
    printf("assemble filename\ntype filename\n");
    printf("symbol\n\n");
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

    if(orCheckHex(co.first)==0 || orCheckHex(co.second)==0 || orCheckHex(co.third)==0){
        // in the format fill x, y, z / x or y or z is not hex
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
    char op[50];
    char mnem[20];
    char form[30];
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

void hashAdd(char op[50], char mnem[20], char form[30], int key){
    hash* temp=(hash*)malloc(sizeof(hash));
    
    strcpy(temp->op, op);
    strcpy(temp->mnem, mnem);
    strcpy(temp->form, form); // make hash struct

    temp->link=hTable[key].link;
    hTable[key].link=temp; // link to the hash table
}

void orOpcode(command co, char o[200]){ // for opcode 
    int inx=((int)co.first[0])%20; //find hash key
    hash* result;
    hash* point;

    if(co.first[0]>90 || co.first[0]<65){ // if mnemonic is not capital letters
        printf("Error: wrong command\n");
        return;
    }

    point=hTable[inx].link;

    result=opcodeFind(point, co.first);

    if(result==NULL)
      printf("Error: there is no opcode\n");

    else{
        orHistoryAdd(o);
        printf("opcode is %02X\n\n",(int)strtol(result->op,NULL, 16));
    }

}

hash* opcodeFind(hash* point, char op[50]){
    int result;

    while(1){
        if(point==NULL)
          break;

        if(strcmp(point->mnem,op)==0){ // if mnemonic is match to the hash table
            //result=(int)strtol(point->op, NULL, 16);
            return point;
        }

        point=point->link;
    }

    return NULL;
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

void orType(command co, char o[200]){
    char temp[100];
    // 디렉토리 판별
    DIR* tdir=NULL;
    struct dirent* tdlist=NULL;
    struct stat tbuf;
    FILE* tfp;

    if((tdir=opendir("."))==NULL){
        printf("Error: there is no such file\n");
        closedir(tdir);
        return;
    } //if there is no file

    while((tdlist=readdir(tdir))!=NULL){
        lstat(tdlist->d_name, &tbuf);

        if(S_ISDIR(tbuf.st_mode) && strcmp(tdlist->d_name, co.first)==0){
          printf("Error: It is a directory name, not file name\n");
          closedir(tdir);
          return;
        }
    }

    tfp=fopen(co.first,"r");
    
    if(tfp==NULL){
        printf("Error: there is no such file\n");
        closedir(tdir);
        fclose(tfp);
        return;
    }

    orHistoryAdd(o);

    while(fgets(temp, sizeof(temp),tfp)!=NULL){
        printf("%s",temp);
    }

    fclose(tfp);
    closedir(tdir);    
}

void orAssemble(command co, char o[200]){ // for make obj, lst files
    int asmError;
    int objError;

    asmError=asmMake(co.first);

    //assem* a=Ast->link;

    if(asmError<0){
        /*while(1){
            if(a==NULL)
              break;
            printf("%04X\t%s\t%s\t%s\n",a->loc, a->state, a->mnem, a->addr);
            a=a->link;
        }*/
        objError=lstObjMake(co.first); // make lst and obj files

        if(objError>0){ // if there is error in lst and obj file
            printf("Assemble Error at %d line.\n",objError);
            assembleDelete();
            symbolDelete();
            return;
        }
      }

    else{
        if(asmError==0){
          return;
        }
        else{
            printf("Assemble Error at %d line.\n",asmError);
            assembleDelete();
            symbolDelete();
            return;
        }
    }

    orHistoryAdd(o);
    printf("Successfully assemble %s\n",co.first);
    sSaved->link=sPresent->link;
    sPresent->link=NULL;
}

void orSymbol(){ // for print symbol table
    symb* point=sSaved->link;

    if(point==NULL){
        printf("There's no symbol table. Please assemble first\n");
        return;
    }

    while(1){
        printf("\t%s\t%04X\n",point->state, point->loc);

        point=point->link;

        if(point==NULL)
          break;
    }
}

int asmMake(char file[30]){
    FILE* afp=fopen(file, "r");
    char state[50];
    char mnem[50];
    char addr[50];
    char str[100];
    int loc=0;
    int line=5;
    int split;
    int form;
    int add=1;

    if(afp==NULL){
        printf("Error: there's no such file\n");
        return 0;
    }

    while(!feof(afp)){
        fgets(str, 100, afp);
        str[strlen(str)-1]='\0';

        strcpy(state, "\t");
        strcpy(mnem, "\t");
        strcpy(addr, "\t");

        split=asmSplit(str, state, mnem, addr);

        if(split==0){
            assembleAdd(loc, state, mnem, addr);
            line+=5;
            continue;
        }
        else if(split<0)
          continue;
        else{
            form=formSelect(mnem, addr);// find the opcode
        }
        //printf("%d\n",form);

        if(form<0)
          return line; //if no opcode found.

        if(split==2 && line>5){ // if there is symbol
          add=symbolAdd(loc, state);
        }

        if(add==0)
          return line; // there is same name

        assembleAdd(loc, state, mnem, addr);
        loc+=form;
        line+=5;
    }

    fclose(afp);
    return -1;
}

int formSelect(char mnem[50], char addr[50]){
    hash* find;
    int length=2;

    if(mnem[0]=='+')
      return 4;

    find=opcodeFind(hTable[(int)mnem[0]%20].link, mnem);

    if(find==NULL){
        if(strcmp(mnem, "BASE")==0){
            return 0;
        }
        else if(strcmp(mnem, "BYTE")==0){
            if(addr[0]=='C'){
                return strlen(addr)-3;
            }
            else if(addr[0]=='X'){
                length=strlen(addr)-3;
                return length/2+length%2;
            }
            else{
                return -1;
            }

        }
        else if(strcmp(mnem, "WORD")==0){
            return 3;
        }
        else if(strcmp(mnem, "RESB")==0){
            return atoi(addr);
        }
        else if(strcmp(mnem, "RESW")==0){
            return atoi(addr)*3;
        }
        else if(strcmp(mnem, "START")==0){
            return 0;
        }
        else if(strcmp(mnem, "END")==0){
            return 0;
        }

        else{
            return -1;
        }

    }

    else{
        return (int)(find->form[0]-48);
    }
    
}

int asmSplit(char str[100], char state[50], char mnem[50], char addr[50]){
    char* ptr;
    int i=0;
    char temp=str[0];

    if(temp=='.'){
        strcpy(state, str);
        return 0;
    }

    else if(temp==' '){
        ptr=strtok(str," ");
        if(ptr!=NULL)
          strcpy(mnem, ptr);
        else
          return -1;

        ptr=strtok(NULL, " ");
        if(ptr!=NULL)
          strcpy(addr, ptr);

        ptr=strtok(NULL," ");

        if(ptr!=NULL){
            strcat(addr, ptr);
        }

        return 1;
    }

    else{
        ptr=strtok(str," ");
        if(ptr!=NULL)
          strcpy(state, ptr);
        else
          return -1;

        ptr=strtok(NULL," ");
        if(ptr!=NULL)
          strcpy(mnem, ptr);

        ptr=strtok(NULL," ");
        if(ptr!=NULL)
          strcpy(addr, ptr);

        return 2;
    }
}

void assembleAdd(int loc, char state[50], char mnem[50], char addr[50]){
    assem* new=(assem*)malloc(sizeof(assem));

    new->loc=loc;
    strcpy(new->state, state);
    strcpy(new->mnem, mnem);
    strcpy(new->addr, addr);
    
    new->link=Aed->link;
    Aed->link=new;
    Aed=new; //link to the history list
}

int symbolAdd(int loc, char state[50]){
    symb* new=(symb*)malloc(sizeof(symb));
    symb* point=sPresent;
    int flag;

    new->loc=loc;
    strcpy(new->state, state);

    while(1){
        if(point->link==NULL){
            new->link=point->link;
            point->link=new;
            break;
        }

        flag=strcmp(point->link->state, new->state);

        if(flag==0){
            free(new);
            return 0;
        }
        else if(flag<0){
            point=point->link;
        }
        else{
            new->link=point->link;
            point->link=new;
            break;
        }
    }

    return 1;
}

int lstObjMake(char file[30]){
    FILE* lst;
    FILE* obj;
    hash* op;
    assem* point=Ast->link;
    char ojcode[10];
    char name[20];
    char objLine[100]="\0";
    int objStart=0;
    int objCount=0;
    int line=5;
    int isOjcode;

    file[strlen(file)-4]='\0';

    strcpy(name, file);
    strcat(name, ".lst");

    lst=fopen(name,"w");
    
    strcpy(name, file);
    strcat(name, ".obj");

    obj=fopen(name,"w"); // to make lst, obj file

    fprintf(obj, "H%s\t%06X%06X\n",Ast->link->state, Ast->link->loc, Aed->loc);
    fprintf(lst,"%d\t%04X\t%s\t%s\t%s\n",line, point->loc, point->state, point->mnem, point->addr);
    line+=5;
    point=point->link;


    while(1){
        if(point==NULL){
            printf("Error: there is no END\n");
            return line;
        }

        if(strcmp(point->mnem, "END")==0){
            fprintf(lst,"%d\t\t\t%s\t%s\t%s\n",line, point->state, point->mnem, point->addr);
            // print M in obj file

            fprintf(obj, "E%06X\n", Ast->link->loc);
            break;
        }

        isOjcode=ojcodeMake(point, ojcode);

        if(isOjcode<0){
            fprintf(lst,"%d\t%04X\t%s\t%s\t%s\n",line, point->loc, point->state, point->mnem, point->addr);
        }

        else if(isOjcode>0){
            if(objCount==0){
                objStart=point->loc;
            }

            fprintf(lst,"%d\t%04X\t%s\t%s\t%s\t%s\n",line, point->loc, point->state, point->mnem, point->addr, ojcode);

            if(objCount>25){
                objCount+=strlen(ojcode)/2;
                fprintf(obj,"T%06X%02X",objStart, objCount);
                fprintf(obj, "%s\n",objLine);
                objCount=0;
                strcpy(objLine,"\0");
            }
            else{
                objCount+=strlen(ojcode)/2;
                strcat(objLine, ojcode);
            }
        }
        else{
            fprintf(lst,"%d\t%04X\t%s\t%s\t%s\n",line, point->loc, point->state, point->mnem, point->addr);

            objCount+=strlen(ojcode)/2;
            fprintf(obj,"T%06X%02X",objStart, objCount);
            fprintf(obj, "%s\n",objLine);
            objCount=0;
            strcpy(objLine,"\0");
        }

        line+=5;
        point=point->link;
    }

    fclose(lst);
    fclose(obj);
    return 0;
}

int ojcodeMake(assem* point, char oj[10]){
    strcpy(oj,"000000");
    return 1;
}

void assembleDelete(){
}

void symbolDelete(){
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

  sPresent=(symb*)malloc(sizeof(symb));
  sSaved=(symb*)malloc(sizeof(symb));
  sPresent->link=NULL;
  sSaved->link=NULL;

  Ast=(assem*)malloc(sizeof(assem));
  Ast->link=NULL;
  Aed=Ast;

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
      else if(strcmp(co.order,"type")==0){
          orType(co, savein);
      }
      else if(strcmp(co.order,"assemble")==0){
          orAssemble(co, savein);
      }
      else if(strcmp(co.order,"symbol")==0){
          orHistoryAdd(savein);
          orSymbol();
      }

      else{ // undefined command 
          printf("Error: wrong command\n");
      }
      
  }
}
