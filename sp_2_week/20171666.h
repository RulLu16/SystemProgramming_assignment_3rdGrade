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
    char first[30];
    char second[30];
    char third[30];
}command; // struct for user command

typedef struct _Hash{
    char op[50];
    char mnem[20];
    char form[30];
    struct _Hash* link;
}hash; //struct for hash table

typedef struct _Assem{
    int loc;
    int length;
    int ojcode;
    char state[50];
    char mnem[50];
    char addr[50];
    struct _Assem* link;
}assem; //struct for saving asm file

typedef struct _Symb{
    int loc;
    char state[50];
    struct _Symb* link;
}symb; //struct for symbol table

typedef struct _ObjPrint{
    int length;
    int ojcode;
}objPrint; // struct for obj file print

// global var, init
His* st;
His* ed;
unsigned char memory[65536][16]={0,};
int duAddr=0;
int base=0;
hash* hTable;
symb* sPresent;
symb* sSaved;
assem* Ast;
assem* Aed;

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
void orOpcode(command, char[]);
hash* opcodeFind(hash*, char[]); // no opcode then return null
void orOpcodeList();
void orType(command, char[]);
void orAssemble(command, char[]);
void orSymbol();

/*==============================================
  for pass 1
  ==============================================*/
int asmMake(char[]); // 0 for no file, integer for error line, -1 for no error
int formSelect(char[], char[]);
int asmSplit(char[], char[], char[], char[]);
void assembleAdd(int, char[], char[], char[]);
int symbolAdd(int, char[]);

/*==============================================
  for pass 2
  ==============================================*/
int lstObjMake(char []);
int ojcodeMake(assem*); // error return 1;
int registerFind(char);
symb* symbolFind(char[]); // no symbol then return null
void assembleDelete();
void symbolDelete();
