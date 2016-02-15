#ifndef __MY_MAIN__
#define __MY_MAIN__

#define THIS_APP_NAME "actCA"

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../common/global.h"
#include "../common/getopt.h"
#include "../common/act.h"

#define CHANGE_MODE_UNDEF -1
#define CHANGE_MODE_MAG 0
#define CHANGE_MODE_SAT 1
#define CHANGE_MODE_INS_ACT8 2
#define CHANGE_MODE_DEL_ACT8 3

typedef struct {
  int mode;
  char filename[FILENAME_MAX];
  float xmag;
  float ymag;
  int A; // A: alpha value, or act8 insert(or delete)-point
  int B, G, R;
} MY_FLAGS;

void ParseOption(int argc, char **argv, MY_FLAGS *flag);
void InitFlags(MY_FLAGS *flag);
void PrintUsageExit(char *pname);
void CheckOptionIfExit(MY_FLAGS *flag);

bool ReadACT(BYTE **dat, MY_FLAGS *flag);

void ProcMag(CACT *act1, MY_FLAGS *flag);
void ProcSat(CACT *act1, MY_FLAGS *flag);
DWORD ProcSat_Main(DWORD oldABGR, MY_FLAGS *flag);
void ProcInsertAct8(CACT *act1, MY_FLAGS *flag);
void ProcDeleteAct8(CACT *act1, MY_FLAGS *flag);

#endif // __MY_MAIN__
