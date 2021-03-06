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

void orType(command co, char o[200]){ // for print file
   char temp[100];
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
        } // if the file name is a directory name. print error
    }

    tfp=fopen(co.first,"r");
    
    if(tfp==NULL){
        printf("Error: there is no such file\n");
        closedir(tdir);
        return;
    } // if ther is no file

    orHistoryAdd(o);

    while(fgets(temp, sizeof(temp),tfp)!=NULL){
        printf("%s",temp);
    } // print file contents
    printf("\n");

    fclose(tfp);
    closedir(tdir);  
}

void orAssemble(command co, char o[200]){ // for assemble filename
    int asmError; // activate when error occurs in symbol and assemble file
    int objError; // activate when error occurs in makeing object code

    asmError=asmMake(co.first); 
    // make asm linked list and symbol linked list

    if(asmError<0){
        objError=lstObjMake(co.first); // make lst and obj files

        if(objError>0){ // if there is error in lst and obj file
            printf("Assemble Error at %d line.\n",objError);
            assembleDelete(); // init assemble linked list
            symbolDelete(sPresent); // init symbol table
            return;
        }
      }

    else{
        if(asmError==0){
          return; // error: there is no such file
        }
        else{
            printf("Assemble Error at %d line.\n",asmError);
            assembleDelete();
            symbolDelete(sPresent);
            return;
        }
    }

    orHistoryAdd(o);
    printf("Successfully assemble %s\n",co.first);

    sSaved->link=sPresent->link; // save symbol table in 'sSaved' linked list
    sPresent->link=NULL; // init the sPresent
    assembleDelete();
}

void orSymbol(){ // for print symbol table
    symb* point=sSaved->link;

    if(point==NULL){
        printf("There's no symbol table. Please assemble first\n");
        return;
    } // there is no symbol table

    while(1){
        printf("\t%s\t%04X\n",point->state, point->loc);

        point=point->link;

        if(point==NULL)
          break;
    } // print symbol table
}

/*==================================================================
  for pass 1
  ==================================================================*/

int asmMake(char file[30]){ 
    // for make assemble linked list and symbol linked list. 
    //if error occurs, return error line

    FILE* afp=fopen(file, "r");
    char state[50];
    char mnem[50];
    char addr[50];
    char str[100];
    int loc;
    int line=5;
    int split;
    int form;
    int add=1;

    if(afp==NULL){
        printf("Error: there's no such file\n");
        return 0;
    } // there is no file in present directory

    while(!feof(afp)){
        fgets(str, 100, afp); // get one line in asm file
        str[strlen(str)-1]='\0';

        strcpy(state, " ");
        strcpy(mnem, " ");
        strcpy(addr, " "); // init state, mnem, addr

        split=asmSplit(str, state, mnem, addr); // split the one line

        if(split==0){
            assembleAdd(loc, state, mnem, addr);
            line+=5;
            continue;
        } // if the line is just comment
        else if(split<0)
          continue; // if the line is empty line
        else{
            if(strcmp(mnem, "START")==0)
              loc=strtol(addr, NULL, 16);
              // if it is start line, set the start location

            form=formSelect(mnem, addr);// find the opcode
        } //normal line

        if(form<0)
          return line; //if no opcode found.

        if(split==2 && line>5){ // if there is symbol
                add=symbolAdd(loc, state);
        }

        if(add==0)
          return line; // if there is same name, error.

        assembleAdd(loc, state, mnem, addr); // add to assemble list
        loc+=form;
        line+=5;
    }

    fclose(afp);
    return -1;
}

int formSelect(char mnem[50], char addr[50]){ 
    // for find format for calculate location counter

    hash* find;
    int length=2;

    if(mnem[0]=='+')
      return 4; // format 4

    find=opcodeFind(hTable[(int)mnem[0]%20].link, mnem); 

    if(find==NULL){ // no opcode in hash table
        if(strcmp(mnem, "BASE")==0){
            return 0;
        } // mnemonic is BASE
        else if(strcmp(mnem, "BYTE")==0){ // byte constant
            if(addr[0]=='C'){
                return strlen(addr)-3;
            } // if it is char type byte constant
            else if(addr[0]=='X'){
                length=strlen(addr)-3;
                return length/2+length%2;
            } // if it is hex type byte constant
            else{
                return -1;
            }

        }
        else if(strcmp(mnem, "WORD")==0){
            return 3;
        } // word constant
        else if(strcmp(mnem, "RESB")==0){
            return atoi(addr);
        } // byte variable
        else if(strcmp(mnem, "RESW")==0){
            return atoi(addr)*3;
        } // word variable
        else if(strcmp(mnem, "START")==0){
            return 0;
        } // start line
        else if(strcmp(mnem, "END")==0){
            return 0;
        } // end line

        else{
            return -1;
        }

    }

    else{
        return (int)(find->form[0]-48);
    } // there is opcode. so return that format
    
}

int asmSplit(char str[100], char state[50], char mnem[50], char addr[50]){ 
    // for split the asm file line

    char* ptr;
    int i=0;
    char temp=str[0];

    if(temp=='.'){
        strcpy(state, str);
        return 0;
    } // comment line

    else if(temp==' '){ // the line that don't have symbol 
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
            strcat(addr, " ");
            strcat(addr, ptr);
        }

        return 1;
    }

    else{ // the line that has symbol
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
    // for add new node to assemble table

    assem* new=(assem*)malloc(sizeof(assem));

    new->loc=loc;
    new->length=0;
    new->ojcode=0;
    strcpy(new->state, state);
    strcpy(new->mnem, mnem);
    strcpy(new->addr, addr); // make new node and insert data
    
    new->link=Aed->link;
    Aed->link=new;
    Aed=new; //link to assemble list
}

int symbolAdd(int loc, char state[50]){
    // for add new node to symbol table

    symb* new=(symb*)malloc(sizeof(symb));
    symb* point=sPresent;
    int flag;

    new->loc=loc;
    strcpy(new->state, state); // make new node and insert data

    while(1){
        if(point->link==NULL){
            new->link=point->link;
            point->link=new;
            break;
        } // reach to the end of symbol list

        flag=strcmp(point->link->state, new->state); // if there is a node that has same name

        if(flag==0){ // is same name, return error
            free(new);
            return 0;
        }
        else if(flag<0){
            point=point->link;
        } // new is bigger than present, move to the next link
        else{
            new->link=point->link;
            point->link=new;
            break;
        } // new is smaller, link to that position
    } // for link new node to symbol list in ascending order by using insert sort

    return 1;
}

/*===========================================================
  for pass 2
  ===========================================================*/

int lstObjMake(char file[30]){
    FILE* lst;
    FILE* obj;
    assem* point=Ast->link->link;
    objPrint objLine[50]; // array for obj T line
    char name[50];
    int i;
    int objCount=0;
    int objStart=0; // start address in object file
    int line=10;
    int length=0;
    int isOjcode; // flag for is there object code
    int* modify; // array for obj M line
    int modifyCount=0;

    while(1){ // make object code in each line.
        if(point==NULL){
            printf("Error: there is no END\n");
            return line;
        } // ther is no end line in asm file

        if(strcmp(point->mnem, "END")==0){
            break;
        }
        // end state

        isOjcode=ojcodeMake(point); // make object code

        if(isOjcode==1)
          return line; // error occurs in making object code

        point=point->link;
        line+=5;
    } 

    line=5;
    point=Ast->link;
    file[strlen(file)-4]='\0';

    strcpy(name, file);
    strcat(name, ".lst");

    lst=fopen(name,"w"); // open lst file
    
    strcpy(name, file);
    strcat(name, ".obj");

    obj=fopen(name,"w"); // open obj file

    fprintf(obj, "H%-6s%06X%06X\n",Ast->link->state, Ast->link->loc, Aed->loc-Ast->link->loc);
    fprintf(lst,"%d\t%04X\t%s\t%s\t%s\n",line, point->loc, point->state, point->mnem, point->addr);
    // print first line of lst, obj file
    
    line+=5;
    point=point->link;
    while(1){
        if(strcmp(point->mnem, "END")==0){
            fprintf(lst,"%d\t\t%s\t%s\t%s\n",line, point->state, point->mnem, point->addr);
            break;
        } // end line

        if(point->length==0){ // if there is no object code
            if(strcmp(point->mnem, "BASE")==0 || point->state[0]=='.')
              fprintf(lst,"%d\t \t%s\t%s\t%s\n",line, point->state, point->mnem, point->addr);
            // base or comment line. so don't need to print loc
            else
              fprintf(lst,"%d\t%04X\t%s\t%s\t%s\n",line,point->loc, point->state, point->mnem, point->addr);
        }
        else{
            if(strlen(point->addr)<8) // addr length is not over 8, which are tab size
               fprintf(lst,"%d\t%04X\t%s\t%s\t%s\t\t",line, point->loc, point->state, point->mnem, point->addr);
            else
              fprintf(lst,"%d\t%04X\t%s\t%s\t%s\t",line, point->loc, point->state, point->mnem, point->addr);
            // print line, loc, state, mnem, addr

            switch(point->length){
              case 2:
                fprintf(lst, "%02X\n",point->ojcode);
                break;
              case 4:
                fprintf(lst, "%04X\n",point->ojcode);
                break;
              case 6:
                fprintf(lst, "%06X\n",point->ojcode);
                break;
              case 8:
                fprintf(lst, "%08X\n",point->ojcode);
                break;
            } // print object code in lst file
        }

        point=point->link;
        line+=5;
    } //make lst file

    point=Ast->link;
    modify=(int*)malloc(sizeof(int)*(line/5)); // malloc the modify array

    while(1){
        if(strcmp(point->mnem, "END")==0){ // meet the end line
            fprintf(obj, "T%06X%02X",objStart, length/2);
            for(i=0;i<objCount;i++){
                switch(objLine[i].length){
                  case 2:
                    fprintf(obj, "%02X",objLine[i].ojcode);
                    break;
                  case 4:
                    fprintf(obj, "%04X", objLine[i].ojcode);
                    break;
                  case 6:
                    fprintf(obj, "%06X", objLine[i].ojcode);
                    break;
                  case 8:
                    fprintf(obj, "%08X", objLine[i].ojcode);
                    break;
                }
            } // print object code before reach the end line
            fprintf(obj,"\n");

            for(i=0;i<modifyCount;i++){
                fprintf(obj,"M%06X%02X\n",modify[i],5);
            } // print M(modify) line in obj file

            fprintf(obj,"E%06X\n",Ast->link->loc); // print end line in obj
            break;
        }

        if(point->length>0){
            if(length==0)
              objStart=point->loc;
            length+=point->length;
            objLine[objCount].length=point->length;
            objLine[objCount].ojcode=point->ojcode;
            objCount++;
        } // if there is object code, add to the objLine array

        if(point->length==8){
            if(point->addr[0]=='#'){
                if(point->addr[1]<48 || point->addr[1]>57){
                    modify[modifyCount]=point->loc+1;
                    modifyCount++;
                } // if #symbol_name. not #decimal_number
            }
            else{
                modify[modifyCount]=point->loc+1;
                modifyCount++;
            }
        } // if there is 4 format object code, so need to modify, add to modify array

        if(length>54 || (((strcmp(point->mnem, "RESB")==0 || strcmp(point->mnem, "RESW")==0)) && length>0)){
            // if objLine size is over the certain length, or meets the
            // variable line, print obj one line in obj file

            fprintf(obj, "T%06X%02X",objStart, length/2);
            for(i=0;i<objCount;i++){
                switch(objLine[i].length){
                  case 2:
                    fprintf(obj, "%02X",objLine[i].ojcode);
                    break;
                  case 4:
                    fprintf(obj, "%04X", objLine[i].ojcode);
                    break;
                  case 6:
                    fprintf(obj, "%06X", objLine[i].ojcode);
                    break;
                  case 8:
                    fprintf(obj, "%08X", objLine[i].ojcode);
                    break;
                }
            }
            fprintf(obj,"\n");
            objCount=0;
            length=0;
        }
        
        point=point->link;
    } //make obj file
   
    fclose(lst);
    fclose(obj);
    return 0;
}

int ojcodeMake(assem* point){
    hash* opco;
    symb* symState;
    int fourFlag=0;
    int key;
    int temp=0;
    int n, i, x, b, p, e;
    int disp;
    int oj=0;
    char ptr[30];
    char* ptemp;
    char* search;

    if(strcmp(point->mnem, "RESB")==0 || strcmp(point->mnem, "RESW")==0){
        return 0;
    } // it is byte or word variable.
    else if(strcmp(point->mnem, "BYTE")==0){ // it is byte constant
        if(point->addr[0]=='C'){ // char type constant
            temp=2;

            while(1){
                if(point->addr[temp]=='\'')
                  break;
                
                oj+=(int)point->addr[temp];
                oj*=0x100;
                temp++;
            } // get Ascii code of C'__'

            oj/=0x100;
            point->ojcode=oj;
            point->length=(temp-2)*2; 

        }
        else if(point->addr[0]=='X'){ // hex type constant
            temp=2;

            while(1){
                if(point->addr[temp]=='\'')
                  break;

                ptr[0]=point->addr[temp];
                ptr[1]='\0';

                oj+=strtol(ptr, NULL ,16);
                oj*=0x10;
                temp++;
            } // get decimal number of X'__'

            oj/=0x10;  
            point->ojcode=oj;
            point->length=((temp-2)/2+(temp-2)%2)*2;
        }
        return 0;
    }
    else if(strcmp(point->mnem, "WORD")==0){ // word constant
        oj=atoi(point->addr);
        point->ojcode=oj;
        point->length=6; // just get decimal number
        return 0;
    }
    else if(strcmp(point->mnem, "BASE")==0){ // BASE for user message
        symState=symbolFind(point->addr, sPresent);

        if(symState==NULL){
            return 1;
        } // no addr symbol, error

        base=symState->loc; // set base register
        return 0;
    } 
    else if(point->state[0]=='.'){
        return 0;
    } // comments line

    /*============================================
      for real object code.
      ============================================*/

    x=b=p=e=0;
    n=i=1; // init nixbpe
    search=strchr(point->addr, ','); // if there is a, b format addr

    if(point->mnem[0]=='+'){
        fourFlag=1;
      key=point->mnem[1]%20;
      ptemp=point->mnem;      
      opco=opcodeFind(hTable[key].link, ptemp+1);
    } // + in mnem so it is 4 format. set flag and find opcode
    else{
      key=point->mnem[0]%20;
      opco=opcodeFind(hTable[key].link, point->mnem);
    }
    // find opcode

    if(point->addr[0]=='@'){
        i=0;
        ptemp=point->addr;
        symState=symbolFind(ptemp+1, sPresent);
    } // indirect addressing
    else if(point->addr[0]=='#'){
        n=0;
        ptemp=point->addr;
        symState=symbolFind(ptemp+1, sPresent);
    } // immediate addressing
    else
      symState=symbolFind(point->addr, sPresent);

    if(search!=NULL && *(search+2)=='X'){
        x=1;
        strcpy(ptr, point->addr);
        search=strchr(ptr, ',');
        *search='\0';

        symState=symbolFind(ptr, sPresent);
    } // if use X register

    if(opco->form[0]=='1'){ // format 1
        oj=strtol(opco->op, NULL, 16);
        point->ojcode=oj;
        point->length=2;
        return 0;
    }
    else if(opco->form[0]=='2'){ // format 2
        oj=strtol(opco->op, NULL, 16);
        oj*=0x10;
        oj+=registerFind(point->addr[0]); // get register number
        oj*=0x10;

        if(point->addr[1]==','){ // if there is second register
            oj+=registerFind(point->addr[3]);
        }        

        point->ojcode=oj;
        point->length=4;
        return 0;
    }
    else if(fourFlag!=1 && opco->form[0]=='3'){ // format 3
        oj=strtol(opco->op, NULL, 16)+n*2+i*1; // get opcode and n, i
        oj*=0x10;

        if(symState==NULL){ // if can't find symbol loc
            if(point->addr[0]=='#'){ // immediate addressing
                ptemp=point->addr;
                if(point->addr[1]=='0')
                  disp=0; // if it is #0
                else{
                    disp=atoi(ptemp+1); // if it is #decimal_number

                    if(disp==0) 
                      return 1; // else no symbol loc, error
                }
            }
            else if(point->addr[0]==' '){
                disp=0;
            } // no addr
            else
              return 1; // can't find symbol loc, error        
        }
        else{
            disp=symState->loc;

            if(disp-point->link->loc>2047){
                b=1;
                disp-=base;
            } // over the disp range
            else if(disp-point->link->loc< -2048 /*&& disp-point->link->loc+4096<0*/){
                b=1;
                disp-=base;
            } // over the disp range
            else{
                p=1;
                disp-=point->link->loc;
            } // take disp

            if(disp<0){
                disp+=4096;
            } // 2's complement 
        }

        oj+=x*8+b*4+p*2+e*1;
        oj*=0x1000;
        oj+=disp;

        point->ojcode=oj;
        point->length=6;
        return 0;
    }
    else if(fourFlag==1){ // format 4
        e=1;

        oj=strtol(opco->op, NULL, 16)+n*2+i*1; // get opcode and n, i

        if(symState==NULL){
            if(point->addr[0]=='#'){ // immediate addressing
                ptemp=point->addr;
                if(point->addr[1]=='0') // #0
                  disp=0;
                else{
                    disp=atoi(ptemp+1); //#decimal_number

                    if(disp==0) 
                      return 1; // can't find symbol, error
                }
            }
            else
              return 1; // no symbol, error
        }
        else{
            disp=symState->loc;
        }

        oj*=0x10;
        oj+=x*8+b*4+p*2+e*1;
        oj*=0x100000;
        oj+=disp;

        point->ojcode=oj;
        point->length=8;

        return 0;
    }

    return 1;
}

int registerFind(char reg){ // for get register number
    switch(reg){
      case 'A':
        return 0;
      case 'X':
        return 1;
      case 'L':
        return 2;
      case 'B':
        return 3;
      case 'S':
        return 4;
      case 'T':
        return 5;
      case 'F':
        return 6;
    }
}

symb* symbolFind(char state[50], symb* findS){ // for find symbol loc
    symb* point=findS->link;

    while(1){
        if(point==NULL)
          break;

        if(strcmp(point->state, state)==0){
            return point;
        } // find symbol

        point=point->link;
    }

    return NULL; // else return NULL

}

void assembleDelete(){ // for delete assemble list
    assem* present=Ast->link;
    assem* next;

    if(present==NULL)
      return;

    while(1){
        next=present->link;

        if(next==NULL){
            free(present);
            break;
        }

        free(present);
        present=next;
    } // free the assemble list

    Ast->link=NULL;
    Aed=Ast; // init the assemble list
}

void symbolDelete(symb* delS){ // for delete symbol list
    symb* present=delS->link;
    symb* next;

    if(present==NULL)
      return;

    while(1){
        next=present->link;

        if(next==NULL){
            free(present);
            break;
        }

        free(present);
        present=next;
    } // free the symbol list

    delS->link=NULL; // init the sPresent symbol list
}

void orProgAddr(command co){ // for set program start point
    int addr=strtol(co.first, NULL, 16);

    if(addr<MAXADDR)
      program_address=addr;

    else
      printf("Error: wrong address\n");

    return;
}

void orLoader(command co, char o[200]){ // for link and load program
    symb* tempFind;
    loadFile* point;

    program_length=0;
    current_length=0;
    symbolDelete(linkingSymbol);
    loadFileDelete();
    end_flag=0; // init data


    /*====================================
      pass 1
      ====================================*/

    printf("control\tsymbol\taddress\tlength\n");
    printf("section\tname\n");
    printf("--------------------------------\n");

    if(strcmp(co.first,"-")!=0){ // link first file in pass1
        makeLinkSymb(co.first);
    } 
    else{
        printf("Error: there is no filename\n");
        symbolDelete(linkingSymbol);
        return;
    } // if there is no file

    if(strcmp(co.second,"-")!=0){ // link second file in pass1
        makeLinkSymb(co.second);
    }
    if(strcmp(co.third,"-")!=0){ // link third file in pass1
        makeLinkSymb(co.third);
    } 

    if(errorDetect==1){ // if error occurs
        errorDetect=0;
        return;
    }
    orHistoryAdd(o);
    
    printf("--------------------------------\n");
    printf("            total length %04X\n",program_length);


    /*======================================
      pass 2
      ======================================*/

    point=Lst->link;
    while(point!=NULL){
        if(point->line[0]=='H'){ // H line 
            tempFind=symbolFind(getSubstring(1,6,point->line),linkingSymbol);
            referArr[1]=tempFind->loc; // save 01 reference number
        }
        else if(point->line[0]=='R'){ // R line
            sectionR(point->line);
            if(errorDetect==1){ // if error occurs
                errorDetect=0;
                printf("Error: no such symbols. Load more file.\n");
                return;
            }
        }
        else if(point->line[0]=='T'){ // T line
            sectionT(point->line);
        }
        else if(point->line[0]=='M'){ // M line
            sectionM(point->line);
        }
        point=point->link;
    } // load memory
}

void makeLinkSymb(char fname[30]){ // for make linking symbol list
    FILE* linkF=fopen(fname,"r");
    char temp[200];

    if(linkF==NULL){ // if there is no file
        printf("Error: there is no such file\n");
        errorDetect=1;
        return;
    }
    
    while(!feof(linkF)){
        fgets(temp, 200, linkF);
        loadFileAdd(temp);

        if(temp[0]=='H'){ // H line
            sectionH(temp);
        }
        else if(temp[0]=='D'){ // D line
            sectionD(temp);
        }     
        else if(temp[0]=='E'){ // E line
            program_length+=current_length;
        }
    }

    fclose(linkF);
    return;
}

void addLinkSymb(int loc, char state[50]){ // for add linking symbol
    symb* new=(symb*)malloc(sizeof(symb));

    new->loc=loc;
    strcpy(new->state, state);

    new->link=linkingSymbol->link;
    linkingSymbol->link=new;

    return;
}

void loadFileAdd(char line[200]){ // for add loadfile
    loadFile* new=(loadFile*)malloc(sizeof(loadFile));

    strcpy(new->line, line);
    
    new->link=Led->link;
    Led->link=new;
    Led=new; //link to obj file list
}

void loadFileDelete(){ // for delete loadfile list
    loadFile* present=Lst->link;
    loadFile* next;

    if(present==NULL)
      return;

    while(1){
        next=present->link;

        if(next==NULL){
            free(present);
            break;
        }

        free(present);
        present=next;
    } // free the loadfile list

    Lst->link=NULL;
    Led=Lst; // init the loadfile list
}

void sectionH(char fline[200]){ // H line
    char* sub;
    char name[50];
    int location;

    sub=getSubstring(1,6,fline);
    strcpy(name, sub); // get program name

    sub=getSubstring(7,12,fline);
    location=strtol(sub, NULL, 16);
    location+=program_length+program_address; // get start address

    sub=getSubstring(13,18,fline);
    current_length=strtol(sub, NULL, 16); // get current program length

    addLinkSymb(location, name); // add to linking symbol list
    printf("%s\t\t  %04X\t %04X\n",name,location,current_length);

    return;
}

void sectionD(char fline[200]){ // D line
    char* sub;
    char name[50];
    char deleteSpace[50];
    int location;
    int delIdx=0;
    int size=strlen(fline);
    int index=1;

    while(index<size-1){

      sub=getSubstring(index,index+5,fline);
      strcpy(name, sub); // get definition name
      index+=6;

      sub=getSubstring(index,index+5,fline);
      location=strtol(sub,NULL, 16);
      location+=program_address+program_length; // get definition loc

      addLinkSymb(location, name); // add to linking symbol list

      for(int i=0;i<strlen(name);i++){
        if(name[i]==' ')
          continue;
        deleteSpace[delIdx]=name[i];
        delIdx++;
      }
      deleteSpace[delIdx]='\0'; 
      delIdx=0; // delete space from name. it is for print.

      printf("\t%6s\t  %04X\n",deleteSpace,location);
      index+=6;
    }

    return;
}

void sectionR(char fline[200]){ // R line
    int index=1;
    int referNum;
    int size=strlen(fline);
    char name[50];
    char* sub;
    symb* finding;

    while(index<size-1){
        
        sub=getSubstring(index, index+1, fline);
        referNum=strtol(sub,NULL,16); // get reference number
        index+=2;

        if(index+5>size-2){ // if reference name is longer than file end
            sub=getSubstring(index, size-2, fline);
            for(int i=size-1-index;i<6;i++){
                strcat(sub," ");
            } // concat " " to the end and get reference name
        }
        else{
          sub=getSubstring(index, index+5, fline);
        } // get reference name
        strcpy(name, sub);
        index+=6;

        finding=symbolFind(name, linkingSymbol);
        if(finding==NULL){ // if there is no reference name in list
            errorDetect=1;
            return;
        }

        referArr[referNum]=finding->loc; // save in referArr
    }

    return;    
}

void sectionT(char fline[200]){ // T line
    int memoryIndex;
    int lineLength=0;
    int content=0;
    char* sub;

    sub=getSubstring(1,6,fline);
    memoryIndex=referArr[1]+strtol(sub, NULL, 16); 
    // get start loc of T line

    sub=getSubstring(7,8,fline);
    lineLength=strtol(sub, NULL, 16);
    // get T line length

    for(int i=9;i<=9+(lineLength*2);i+=2){
        sub=getSubstring(i,i+1,fline);
        content=strtol(sub, NULL, 16);
        memory[memoryIndex/16][memoryIndex%16]=content;
        memoryIndex++;
    } // get content and load to memory

    return;
}

void sectionM(char fline[200]){ // M line
    int memoryIndex;
    int modifyLen;
    int content=0;
    int ratio;
    char* sub;

    sub=getSubstring(1,6,fline);
    memoryIndex=referArr[1]+strtol(sub, NULL, 16);
    // get modify address

    sub=getSubstring(7,8,fline);
    modifyLen=strtol(sub, NULL ,16);
    // get modify length (half byte)
  
    for(int i=memoryIndex;i<memoryIndex+(modifyLen/2)+(modifyLen%2);i++){
        content+=(int)memory[i/16][i%16];
        content*=0x100;
    } // get memory content
    content/=0x100;
    if(modifyLen%2==0){
        if(content/0x100000>=0x8){
            content-=0x1000000;
        }
    }
    else{
        if((content%0x100000)/0x10000>=0x8){
            content-=0x100000;
        }
    } // if it is negative number, use 2's complement

    sub=getSubstring(10,11,fline); // get reference number
       
    switch(fline[9]){
      case '+':
        content+=referArr[strtol(sub, NULL, 16)];
        break;
      case '-':
        content-=referArr[strtol(sub, NULL, 16)];
        break;
    } // calculate modification
    if(content<0){
        if(modifyLen%2==0)
          content+=0x1000000;
        else
          content+=0x100000;
    } // if it is negative number, use 2's complement

    ratio=0x10000;
    for(int i=memoryIndex;i<memoryIndex+(modifyLen/2)+(modifyLen%2);i++){
        memory[i/16][i%16]=content/ratio;
        ratio/=0x100;
    } // load to memory
}

char* getSubstring(int start, int end, char str[200]){ // for get substring
    char* result=(char*)malloc(sizeof(char)*100);
    int index=0;

    for(int i=start;i<=end;i++){
        result[index]=str[i];
        index++;
    } // make substring from start to end
    result[index]='\0';

    return result;
}

void orBp(command co, char o[200]){ // for set break point to debug
    int position=0;

    if(strcmp(co.first, "-")==0){
        // print bp position
        printBp();
        orHistoryAdd(o);
    }
    else if(strcmp(co.first, "clear")==0){
        // clear all bp
        printf("             [ok] clear all breakpoints\n");
        deleteBp();
        orHistoryAdd(o);
    }
    else{
       position=strtol(co.first, NULL, 16); // get bp position
       if(position<=(program_length+program_address) && position>=program_address){ 
           // if address is in range of program
           printf("             [ok] create breakpoint %X\n",position);
           addBp(position); // add to bp list
           orHistoryAdd(o);
       }
       else{ // error occurs
           printf("Error: wrong address.\n");
           return;
       }
    }
}

void printBp(){ // for print bp
    breakP* present=bpList->link;

    printf("             breakpoint\n");
    printf("             ----------\n");

    while(1){
      if(present==NULL){
          break;
      }

      printf("             %X\n",present->loc);
      present=present->link;
    }
}

void addBp(int position){ // for add bp
    breakP* present=bpList;
    breakP* new=(breakP*)malloc(sizeof(breakP));

    new->loc=position;

    while(1){
        if(present->link==NULL){
            new->link=present->link;
            present->link=new;
            break;
        } // reach to the end of bp list

        if(position==present->link->loc){ // is same bp, just end program
            return;
        }    
        else if(position > present->link->loc){
            present=present->link;
        } // new is bigger than present, move to the next link
        else{
            new->link=present->link;
            present->link=new;
            break;
        } // new is smaller, link to that position
    } 
}

void deleteBp(){ // for delete bp list
    breakP* present=bpList->link;
    breakP* next;

    if(present==NULL)
      return;

    while(1){
        next=present->link;

        if(next==NULL){
            free(present);
            break;
        }

        free(present);
        present=next;
    } // free the bpList

    bpList->loc=-1;
    bpList->link=NULL; // init the bpList
}

void orRun(){ // for order run
    if(end_flag==0){
        end_flag=1;
        initRegister();
    } // when first run

    while(reg[8]<program_length+program_address){ // until end program
        if(isStop()==1 && stop_flag==0){ // if reach to bp, end run
            printRegister();
            stop_flag=1;
            printf("\t     Stop at checkpoint[%X]\n",reg[8]);
            return;
        }

        else {
            disAssemble(reg[8]); // disassemble start
            stop_flag = 0;
        }
    }

    printRegister();
    printf("\t     End program\n");
    end_flag=0;

    return;
}

void initRegister(){ // for init registers
    for(int i=0;i<10;i++){
        reg[i]=0;
    }
    reg[8]=program_address;
    reg[2]=program_length;

    return;
}

void printRegister(){ // for print registers
    printf("A : %06X   X : %06X\n",reg[0], reg[1]);
    printf("L : %06X  PC : %06X\n",reg[2], reg[8]);
    printf("B : %06X   S : %06X\n",reg[3], reg[4]);
    printf("T : %06X\n",reg[5]);

    return;
}

void disAssemble(int pc){ // for get opcode
    int opcode,ni;
    int temp;

    opcode=(int)memory[reg[8]/16][reg[8]%16];
    
    for(int i=0;i<=3;i++){ 
        temp=(opcode-i)%0x10;
        if(temp==0 || temp==4 || temp==8 || temp==12){
            ni=i; // get ni
            opcode-=i; // get opcode
            break;
        }
    }
    executeInstruction(opcode, ni, pc); // execute instructions
}

void executeInstruction(int op, int ni, int pc){ // for execute instructions
    int disp,result;
    int format=0;
    int ratio=0x10000;
    int xbpe=memory[(pc+1)/16][(pc+1)%16]/0x10;

    switch(op){
      //STL
      case 0x14:
        opStore(2,pc,xbpe);    
        break;
      //LDB
      case 0x68:
        opLoad(3,pc,xbpe,ni);
        break;
      //JSUB
      case 0x48:
        if(xbpe%2==0){
            reg[2] = reg[8]+3;
            disp=getDisp(3,pc,xbpe); // get disp
            if (ni == 2) {
                disp = calculateDisp(disp, 3, xbpe);
            }
        }
        else{
            reg[2] = reg[8]+4;
            disp=getDisp(4,pc,xbpe);
        }
        reg[8] = disp;
        break;
      //LDA
      case 0x00:
        opLoad(0,pc,xbpe,ni);
        break;
      //COMP
      case 0x28:
        if(xbpe%2==0){
            disp=getDisp(3,pc,xbpe);
            disp = calculateDisp(disp, ni, xbpe);
            format=3;
        }
        else{
            disp=getDisp(4,pc,xbpe);
            format=4;
        } // get disp and its content

        if(reg[0]>disp)
          cc=0;
        else if(reg[0]==disp)
          cc=1;
        else
          cc=2;
        reg[8]+=format;
        break;
      //JEQ
      case 0x30:
        opCondJump(1,pc,xbpe,ni);
        break;
      //J
      case 0x3C:
        if(xbpe%2==0){
            disp=getDisp(3,pc,xbpe);
            if (ni == 2) {
                disp = calculateDisp(disp, 3, xbpe);
            }
        }
        else{
            disp=getDisp(4,pc,xbpe);
        } // get disp and its content
        reg[8]=disp;
        break;
      //STA
      case 0x0C:
        opStore(0,pc,xbpe);
        break;
      //CLEAR
      case 0xB4:
        reg[memory[(pc+1)/16][(pc+1)%16]/0x100]=0;
        reg[8]+=2;
        break;
      //LDT
      case 0x74:
        opLoad(5,pc,xbpe,ni);
        break;
      //TD
      case 0xE0:
        if(xbpe%2==0){
            format=3;
        }
        else{
            format=4;
        }
        cc=2;
        reg[8]+=format;
        break;
      //RD
      case 0xD8:
        if(xbpe%2==0){
            format=3;
        }
        else{
            format=4;
        }
        rd_flag=1; // set rd flag
        reg[8]+=format;
        break;
      //COMPR
      case 0xA0:
        if(rd_flag==1){ // if rd_flag is set, cc is =
            rd_flag=0;
            cc=1;
        }
        else{
            int first=memory[(pc+1)/16][(pc+1)%16]/0x10;
            int second=memory[(pc+1)/16][(pc+1)%16]%0x10;
            if(reg[first]>reg[second])
              cc=0;
            else if(reg[first]==reg[second])
              cc=1;
            else
              cc=-1;
        }
        reg[8] += 2;
        break;
      //STCH
      case 0x54:
        if(xbpe%2==0){
            disp=getDisp(3,pc,xbpe);
            format=3;
        }
        else{
            disp=getDisp(4,pc,xbpe);
            format=4;
        }
        memory[disp/16][disp%16]=reg[0]%0x100; //store A's rightmost byte
        reg[8]+=format;
        break;
      //TIXR
      case 0xB8:
        reg[1]++;
        int first = reg[1];
        int second=reg[memory[(pc+1)/16][(pc+1)%16]/0x10];
        if(first>second)
          cc=0;
        else if(first==second)
          cc=1;
        else
          cc=2;
        reg[8]+=2;
        break;
      //JLT
      case 0x38:
        opCondJump(2,pc,xbpe,ni);
        break;
      //STX
      case 0x10:
        opStore(1,pc,xbpe);
        break;
      //RSUB
      case 0x4C:
        reg[8]=reg[2];
        break;
      //LDCH
      case 0x50:
        if(xbpe%2==0){
            disp=getDisp(3,pc,xbpe);
            disp = calculateDisp(disp, ni, xbpe);
            format=3;
        }
        else{
            disp=getDisp(4,pc,xbpe);
            format=4;
        }
        int temp=reg[0]%0x100;
        disp%=0x100;
        reg[0]+=disp-temp; // load disp's rightmost byte
        reg[8]+=format;
        break;
      //WD
      case 0xDC:
          if (xbpe % 2 == 0) {
              format = 3;
          }
          else {
              format = 4;
          }
          reg[8] += format;
          break;

      default:
          reg[8] += 3;
    }

    return;
}

void opStore(int regNum, int pc, int xbpe){ // for store opcode
    int disp, ratio;
    int format;

    if(xbpe%2==0){
        format = 3;
        disp=getDisp(3,pc,xbpe);
    }
    else{
        disp=getDisp(4,pc,xbpe);
        format=4;
    } // get disp

    ratio=0x10000;
    for(int i=0;i<3;i++){
        memory[(disp+i)/16][(disp+i)%16]=reg[regNum]/ratio;
        ratio/=0x100;
    } // load register content to the disp memory
    reg[8] += format; 

    return;
}

void opLoad(int regNum, int pc, int xbpe, int ni){ // for load opcode
    int disp, format;

    if(xbpe%2==0){
        disp=getDisp(3,pc,xbpe);
        disp=calculateDisp(disp, ni, xbpe);
        format=3;
    }
    else{
        disp=getDisp(4,pc,xbpe);
        format=4;
    } // get disp and its content
    reg[regNum] = disp; //save to the register
    reg[8]+=format;

    return;
}

void opCondJump(int cond, int pc, int xbpe, int ni){ // for conditional jump opcode
    int disp, format;
    if(xbpe%2==0){
        disp=getDisp(3,pc,xbpe);
        format = 3;
        if (ni == 2) {
            disp = calculateDisp(disp, 3, xbpe);
        }
    }
    else{
        disp=getDisp(4,pc,xbpe);
        format = 4;
    } // get disp and its content

    if(cc==cond)
      reg[8] = disp;
    else 
      reg[8] += format;
        
    return;
}

int getDisp(int format, int pc, int xbpe){ // for get disp
    int disp=0;
    int temp, ratio;

    disp=memory[(pc+1)/16][(pc+1)%16]%0x10;
    for(int i=2;i<format;i++){
        disp*=0x100;
        disp+=memory[(pc+i)/16][(pc+i)%16];
    } // get disp in memory

    if(format==3 && disp/0x100 >= 0x8){
        if (xbpe > 1) {
            disp -= 0x1000;
        }
    } // if disp is negetive, use 2's complement

    pc += format;
    switch(xbpe){
      case 2:
        disp+=pc;
        break;
      case 4:
        disp+=reg[3];
        break;
      case 10:
        disp+=pc+reg[1];
        break;
      case 12:
        disp+=reg[3]+reg[1];
        break;
    } // calculate disp using pc or base register

    return disp;
}

int calculateDisp(int disp, int ni, int xbpe){ // to calculate disp content
    int result;

    if(xbpe== 10 || xbpe==12){
        result=memory[(disp)/16][(disp)%16];
        return result;
    }

    switch(ni){
      case 1: // immediate addressing
        return disp;
      case 2: // indirect addressing
        result=getValue(disp);
        result=getValue(result);
        return result;
      case 3: // simple addressing
        result=getValue(disp);
        return result;
    }

    return 0;
}

int getValue(int disp){ // to get value
    int result=0;

    for(int i=0;i<3;i++){
        result+=memory[(disp+i)/16][(disp+i)%16];
        result*=0x100;
    } // use disp, get content of memory
    result/=0x100;

    return result;
}

int isStop(){ // for check bp
    breakP* point=bpList->link;

    while(point!=NULL){
        if(point->loc==reg[8])
          return 1; // if pc register == bp

        point=point->link;
    }

    return 0;
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

  linkingSymbol=(symb*)malloc(sizeof(symb));
  linkingSymbol->link=NULL;

  Lst=(loadFile*)malloc(sizeof(loadFile));
  Lst->link=NULL;
  Led=Lst;

  bpList=(breakP*)malloc(sizeof(breakP));
  bpList->loc=-1;
  bpList->link=NULL;

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
      else if(strcmp(co.order,"type")==0){ // if command is type
          orType(co, savein);
      }
      else if(strcmp(co.order,"assemble")==0){ // if command is assemble
          orAssemble(co, savein);
      }
      else if(strcmp(co.order,"symbol")==0){ // if command is symbol
          orHistoryAdd(savein);
          orSymbol();
      }
      else if(strcmp(co.order,"progaddr")==0){ // if command is progaddr
          orHistoryAdd(savein);
          orProgAddr(co);
      }
      else if(strcmp(co.order,"loader")==0){ // if command is loader
          orLoader(co,savein);
      }
      else if(strcmp(co.order,"bp")==0){ // if command is bp
          orBp(co, savein);
      }
      else if(strcmp(co.order,"run")==0){ // if command is run
          orHistoryAdd(savein);
          orRun();
      }
      else{ // undefined command 
          printf("Error: wrong command\n");
      }
  }
}
