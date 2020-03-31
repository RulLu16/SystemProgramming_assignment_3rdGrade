#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int MAXADDR=1048576;
const int MAXVALUE=256;

//structs for linked list
typedef struct _His{
    char order[200];
    struct _His* link;
}His; // struct for history

typedef struct _Command{
    char order[200];
    char first[10];
    char second[10];
    char third[10];
}command; // struct for user command

typedef struct _Hash{
    char op[10];
    char mnem[10];
    char form[10];
    struct _Hash* link;
}hash; //struct for hash table

// global var, init
His* st;
His* ed;
unsigned char memory[65536][16]={0,};
int duAddr=0;
hash* hTable;

//functions

command splitInput(char[]);
command initCommand();
void orHelp();
void orDir();
void orQuit();
void orHistory();
void orHistoryAdd(char[]);
void orDump(command, char[]);
void orDumpStart(command, char[]);
void orDumpStartEnd(command, char[]);
void orDumpSelector(command, char[]); 
void orDumpPrint(int, int);
int orCheckHex(char[]); // return 0 when it is not hex
void orEdit(command, char[]);
void orFill(command, char[]);
void orReset();
void hashMake();
void hashAdd(char[], char[], char[], int);
int hashFind();
void orOpcode(command, char[]);
void orOpcodeList();
