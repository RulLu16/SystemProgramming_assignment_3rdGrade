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

// global var, init
His* st;
His* ed;
char memory[65536][16]={0,};
int duAddr=0;

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
