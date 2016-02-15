#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "../common/sprite.h"
#include "../common/rsw.h"

CSprite *sprite;

//void CheckActVerAndSpeed(char *fname);
void check(char *fname);
void CheckActVersion(char *fname);
void CheckRSWParse(char *fname);
void CheckEquipExtOffs(char *fname);
void CheckSprVersion(char *fname);
void CheckFlatImage(char *fname);
void LoopFindFile(char *str);

int main(int argc, char **argv)
{
  LoopFindFile("D:\\home\\data\\sprite\\¸ó½ºÅÍ");

  return 0;
}

void LoopFindFile(char *str)
{
  struct _finddata_t c_file;
  long hFile;
  char fname[FILENAME_MAX];
  char str2[FILENAME_MAX];

  sprintf(str2, "%s\\*.*", str);
  if( (hFile = (long)_findfirst(str2, &c_file )) == -1L ) {
    ;
  } else {
    sprintf(fname, "%s\\%s", str, c_file.name);
    if (c_file.attrib != _A_SUBDIR) {
      check(fname);
    } else {
      if (c_file.name[0] != '.') {
        LoopFindFile(fname);
      }
    }
    while( _findnext( hFile, &c_file ) == 0 ) {
      sprintf(fname, "%s\\%s", str, c_file.name);
      if (c_file.attrib != _A_SUBDIR) {
        check(fname);
      } else {
        if (c_file.name[0] != '.') {
          LoopFindFile(fname);
        }
      }
    }
    _findclose( hFile );
  }
}

//void CheckActVerAndSpeed(char *fname);
void check(char *fname)
{
  if (strnicmp(&fname[strlen(fname)-3], "act", 3) != 0)  return;

  BYTE *actdat;
  int flen;
  FILE *fp;
  if ((fp = fopen(fname, "rb")) == NULL) {
    printf("%s cannot open.\n", fname);
    return;
  }
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  actdat = new BYTE[flen];
  fread(actdat, 1, flen, fp);
  fclose(fp);

  CACT *act1;
  act1 = new CACT(actdat);
  printf("%04X,%s,%f,%f,%f,%f,%f\n",
         act1->GetVersion(), fname,
         act1->GetInterval(0), act1->GetInterval(7), act1->GetInterval(15),
         act1->GetInterval(23), act1->GetInterval(31));
  delete act1;
  delete actdat;
}


void CheckActVersion(char *fname)
{
  if (strnicmp(&fname[strlen(fname)-3], "act", 3) != 0)  return;

  BYTE *actdat;
  int flen;
  FILE *fp;
  if ((fp = fopen(fname, "rb")) == NULL) {
    printf("%s cannot open.\n", fname);
    return;
  }
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  actdat = new BYTE[flen];
  fread(actdat, 1, flen, fp);
  fclose(fp);

  CACT *act1;
  act1 = new CACT(actdat);
  printf("%04X : %s\n", act1->GetVersion(), fname);
  delete act1;
  delete actdat;

}

void CheckRSWParse(char *fname)
{
  if (strnicmp(&fname[strlen(fname)-3], "rsw", 3) != 0)  return;

  FILE *fp;
  BYTE *dat;
  int flen;

  fp = fopen(fname, "rb");
  if (fp == NULL)  return;

  printf("%s : ", fname);

  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = new BYTE[flen];
  fread(dat, 1, flen, fp);
  fclose(fp);

  CRSW *rsw1 = new CRSW(dat, flen);
  /*
  if (rsw1->parseOK) {
    char fn2[FILENAME_MAX];
    sprintf(fn2, "%s.unv", fname);
    rsw1->WriteAsText(fn2);
  }
  */
  printf("\n");
  delete rsw1;


  delete dat;
}

void CheckEquipExtOffs(char *fname)
{
  if (strnicmp(&fname[strlen(fname)-3], "act", 3) != 0)  return;

  BYTE *actdat;
  int flen;
  FILE *fp;
  if ((fp = fopen(fname, "rb")) == NULL) {
    printf("%s cannot open.\n", fname);
    return;
  }
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  actdat = (BYTE *)malloc(flen);
  fread(actdat, 1, flen, fp);
  fclose(fp);

  CACT *act1;
  act1 = new CACT(actdat);
  printf("%s\n", fname);
  int np;
  for (int i=0; i<12*8; i++) {
    np = act1->aall.aa[i].numOfPat;
    for (int j=0; j<np; j++) {
      printf("%3d %3d : ", act1->aall.aa[i].aaap[j].ExtXoffs, act1->aall.aa[i].aaap[j].ExtYoffs);
    }
    printf("\n");
  }


  delete act1;
  free(actdat);
}

void CheckSprVersion(char *fname)
{
  FILE *fp;
  fp = fopen(fname, "rb");
  DWORD ver;
  fread(&ver, 1, sizeof(ver), fp);
  fclose(fp);

  if (ver >> 16 != 0x0101) {
    printf("%04X %s\n", ver >> 16, fname);
  }
}


void CheckFlatImage(char *fname)
{
  char sprfname[FILENAME_MAX];
  strncpy(sprfname, fname, strlen(fname)-4);
  sprfname[strlen(fname)-4] = '\0';
  strcat(sprfname, ".spr");

  BYTE *actdat;
  BYTE *sprdat;
  int flen;
  FILE *fp;
  fp = fopen(fname, "rb");
  if (fp == NULL) {
    printf("%s can't open\n", fname);
    return;
  }
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  actdat = (BYTE *)malloc(flen);
  fread(actdat, 1, flen, fp);
  fclose(fp);

  fp = fopen(sprfname, "rb");
  if (fp == NULL) {
    printf("%s can't open\n", sprfname);
    free(actdat);
    return;
  }
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  sprdat = (BYTE *)malloc(flen);
  fread(sprdat, 1, flen, fp);
  fclose(fp);
  if (*(sprdat+2) != 5) {
    free(actdat);
    free(sprdat);
    return;
  }

  int aw, ah, no, sw, sh;
  sprite = new CSprite(actdat, sprdat, flen);
  for (int i=0; i<sprite->act->aall.ah.numOfAct; i++) {
    for (int j=0; j<((sprite->act->aall.aa)+i)->numOfPat; j++) {
      aw = ((((sprite->act->aall.aa)+i)->aaap->aps5)+j)->sprW;
      ah = ((((sprite->act->aall.aa)+i)->aaap->aps5)+j)->sprH;
      no = ((((sprite->act->aall.aa)+i)->aaap->aps5)+j)->sprNo;
      if ( ((((sprite->act->aall.aa)+i)->aaap->aps5)+j)->spType == 1) {
        no += sprite->spr->numPalImg;
      }
      sw = sprite->spr->imgs[no].w;
      sh = sprite->spr->imgs[no].h;
      if (!((aw == sw) &&  (ah == sh))) {
        printf("%s - A:%d, P:%d\n", fname, i, j);
      }
    }
  }
  
  delete sprite;
  free(sprdat);
  free(actdat);
}
