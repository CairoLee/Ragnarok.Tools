#include <stdlib.h>
#include <string.h>

#include "main.h"

int main(int argc, char **argv)
{
  MY_FLAGS flag;

  ParseOption(argc, argv, &flag);
  CheckOptionIfExit(&flag);

  BYTE *dat = NULL;
  if (ReadACT(&dat, &flag) == false) {
    printf("%s can not read.\n");
    exit(-1);
  }

  CACT *act1 = new CACT(dat);
  free(dat);

  switch (flag.mode) {
  case CHANGE_MODE_MAG:
    ProcMag(act1, &flag);
    break;
  case CHANGE_MODE_SAT:
    ProcSat(act1, &flag);
    break;
  case CHANGE_MODE_INS_ACT8:
    ProcInsertAct8(act1, &flag);
    break;
  case CHANGE_MODE_DEL_ACT8:
    ProcDeleteAct8(act1, &flag);
    break;
  }

  act1->WriteAct(flag.filename);  

  return 0;
}

void ProcInsertAct8(CACT *act1, MY_FLAGS *flag)
{
  act1->InsertNullAct8(flag->A * 8);
}

void ProcDeleteAct8(CACT *act1, MY_FLAGS *flag)
{
  act1->DeleteAct8(flag->A * 8);
}

void ProcMag(CACT *act1, MY_FLAGS *flag)
{
  float oldxmag, oldymag;
  int oldxoffs, oldyoffs;
  for (int i=0; i<act1->GetNumAction(); i++) {
    for (int j=0; j<act1->GetNumPattern(i); j++) {
      for (int k=0; k<act1->GetNumSprites(i, j); k++) {
        act1->GetOffsPoint(&oldxoffs, &oldyoffs, i, j, k);
        oldxmag = act1->GetXMagValue(i, j, k);
        oldymag = act1->GetYMagValue(i, j, k);

        act1->SetXOffsValue(i, j, k, (int)(oldxoffs * flag->xmag));
        act1->SetYOffsValue(i, j, k, (int)(oldyoffs * flag->ymag));
        act1->SetXMagValue(i, j, k, oldxmag * flag->xmag);
        act1->SetYMagValue(i, j, k, oldymag * flag->ymag);
      }
    }
  }
}

void ProcSat(CACT *act1, MY_FLAGS *flag)
{
  DWORD oldABGR, newABGR;
  for (int i=0; i<act1->GetNumAction(); i++) {
    for (int j=0; j<act1->GetNumPattern(i); j++) {
      for (int k=0; k<act1->GetNumSprites(i, j); k++) {
        oldABGR = act1->GetABGRValue(i, j, k);
        newABGR = ProcSat_Main(oldABGR, flag);

        act1->SetABGRValue(i, j, k, newABGR);
      }
    }
  }
}

DWORD ProcSat_Main(DWORD oldABGR, MY_FLAGS *flag)
{
  if (flag->A >= 0) {
    oldABGR = (oldABGR & 0x00FFFFFF) | ((BYTE)flag->A << 24);
  }
  if (flag->B >= 0) {
    oldABGR = (oldABGR & 0xFF00FFFF) | ((BYTE)flag->B << 16);
  }
  if (flag->G >= 0) {
    oldABGR = (oldABGR & 0xFFFF00FF) | ((BYTE)flag->G << 8);
  }
  if (flag->R >= 0) {
    oldABGR = (oldABGR & 0xFFFFFF00) | (BYTE)flag->R; 
  }

  return oldABGR;
}

bool ReadACT(BYTE **dat, MY_FLAGS *flag)
{
  int flen;
  FILE *fp;
  fp = fopen(flag->filename, "rb");
  if (fp == NULL)  return false;
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  *dat = (BYTE *)malloc(flen);
  fread(*dat, 1, flen, fp);
  fclose(fp);

  return true;
}

void CheckOptionIfExit(MY_FLAGS *flag)
{
  switch (flag->mode) {
  case CHANGE_MODE_MAG:
    if (flag->xmag == 0 || flag->ymag == 0) {
      printf("x or y multiplier is ZERO.\nProgram terminate.\n");
      exit(-1);
    }
    break;
  case CHANGE_MODE_SAT:
    if (flag->A > 255) {
      printf("A value is over 255.\nReset A value is 255.\n");
      flag->A = 255;
    } else if (flag->A == 0) {
      printf("A value is ZERO. Is this correct?\n");
    }
    if (flag->B > 255) {
      printf("B value is over 255.\nReset B value is 255.\n");
      flag->B = 255;
    } else if (flag->B == 0) {
      printf("B value is ZERO. Is this correct?\n");
    }
    if (flag->G > 255) {
      printf("G value is over 255.\nReset G value is 255.\n");
      flag->G = 255;
    } else if (flag->G == 0) {
      printf("G value is ZERO. Is this correct?\n");
    }
    if (flag->R > 255) {
      printf("R value is over 255.\nReset R value is 255.\n");
      flag->R = 255;
    } else if (flag->R == 0) {
      printf("R value is ZERO. Is this correct?\n");
    }
    break;
  case CHANGE_MODE_INS_ACT8:
    break;
  case CHANGE_MODE_DEL_ACT8:
    break;
  }
}

void ParseOption(int argc, char **argv, MY_FLAGS *flag)
{
  char fnbuf[FILENAME_MAX];

  int c;
  extern char *optarg;
  extern int optind;
  int errflg = 0;

  if (argc == 1) PrintUsageExit(THIS_APP_NAME);

  InitFlags(flag);
  while ((c = my_getopt(argc, argv, "cmi:d:")) != -1) {
    switch (c) {
    case 'c':
      flag->mode = CHANGE_MODE_SAT;
      break;
    case 'm':
      flag->mode = CHANGE_MODE_MAG;
      break;
    case 'i':
      flag->mode = CHANGE_MODE_INS_ACT8;
      flag->A = atoi(optarg);
      if (optarg[0] != '0' && flag->A == 0) {
        PrintUsageExit(THIS_APP_NAME);
      }
      break;
    case 'd':
      flag->mode = CHANGE_MODE_DEL_ACT8;
      flag->A = atoi(optarg);
      if (optarg[0] != '0' && flag->A == 0) {
        PrintUsageExit(THIS_APP_NAME);
      }
      break;
    default:
      errflg++;
      break;
    }
  }

#ifdef WIN32
  GetFullPathName(argv[optind], sizeof(fnbuf), fnbuf, NULL);
#else
  if (argv[optind][0] != '/') {
    char fnbuf2[FILENAME_MAX];
    GETCWD(fnbuf2, sizeof(fnbuf2));
    SNPRINTF(fnbuf, sizeof(fnbuf), "%s/%s", fnbuf2, argv[optind]);
  } else {
    strcpy(fnbuf, argv[optind]);
  }
#endif

  strcpy(flag->filename, fnbuf);

  switch (flag->mode) {
  case CHANGE_MODE_MAG:
    if (argc <= optind+2)  PrintUsageExit(argv[0]);
    flag->xmag = (float)atof(argv[optind+1]);
    flag->ymag = (float)atof(argv[optind+2]);
    break;
  case CHANGE_MODE_SAT:
    if (argc <= optind+4)  PrintUsageExit(argv[0]);
    flag->A = atoi(argv[optind+1]);
    flag->B = atoi(argv[optind+2]);
    flag->G = atoi(argv[optind+3]);
    flag->R = atoi(argv[optind+4]);
    break;
  }
}

void InitFlags(MY_FLAGS *flag)
{
  flag->filename[0] = '\0';
  flag->mode = CHANGE_MODE_UNDEF;
}

void PrintUsageExit(char *pname)
{
  printf("****************WARNING*****************\n");
  printf("* THIS PROGRAM OVERWRITE YOUR ACT FILE *\n");
  printf("****************************************\n");
  printf("Usage1: %s -[m|c] actfile ...\n", pname);
  printf("Usage2: %s -[i|d] No actfile ...\n", pname);
  printf("Flags:\n");
  printf("  -m\tChange magnification all sprites.\n");
  printf("     example: %s -m aaa.act 2e+1 2.5\n", pname);
  printf("     result : All sprites widths are multiplied 20.\n");
  printf("              Heights are multiplied 2.5.\n");
  printf("              Positions are approximately correct. :-P\n");
  printf("              Not xmag=20,ymag=2.5.\n");
  printf("              If initial xmag=2, finally xmag=40\n");
  printf("  -c\tChange saturation all sprites.\n");
  printf("    example1: %s -c aaa.act 124 125 126 127\n", pname);
  printf("    result  : All sprites saturation are setted 7C7D7E7F. (AABBGGRR)\n");
  printf("              Command line parameters are NOT HEXADECIMAL.\n");
  printf("    example2: %s -c aaa.act 127 -1 -1 -1\n", pname);
  printf("    result  : Only change transmissibity.\n");
  printf("              So if you set negative value,\n");
  printf("              each saturaion(A,B,G,R) is kept original value.\n");
  printf("  -i or d  Insert or Delete action.\n");
  printf("    example1: %s -i -1 foo.act\n", pname);
  printf("    result  : Add null (8 direction) action.\n");
  printf("    example2: %s -i 0 foo.act\n", pname);
  printf("    result  : Insert null action at first.\n");
  printf("    example3: %s -d 0 foo.act\n", pname);
  printf("    result  : Delete first (8 direction) action.\n");
  printf("    example4: %s -d -1 foo.act\n", pname);
  printf("    result  : Nothing done.\n");
  printf("              If you want to delete last action,\n");
  printf("              you must count action, and indicate.\n");
  printf("    CAUTION: First is 0, Second is 1, ...\n");
  printf("\n");

  exit(-1);
}
