#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "act.h"

CACT::CACT(BYTE *dat)
{
  aall.aa = NULL;
  aall.interval = NULL;
  aall.numOfSound = 0;
  aall.fnameSnd = NULL;
  modified = false;

  ParseActData(dat);
}

CACT::~CACT(void)
{
  for (int i=0; i<aall.ah.numOfAct; i++) {
    for (int j=0; j<aall.aa[i].numOfPat; j++) {
      delete aall.aa[i].aaap[j].aps1;
    }
  }
  delete aall.aa;

  for (int i=0; i<aall.numOfSound; i++) {
    delete aall.fnameSnd[i];
    aall.fnameSnd[i] = NULL;
  }
  delete aall.fnameSnd;
  delete aall.interval;
}

bool CACT::ParseActData(BYTE *dat)
{
  if (!ParseActData_Header(dat))  return false;
  dat += sizeof(ActHeader);
  try {
    aall.aa = new AnyAction[aall.ah.numOfAct];
  } catch (...) {
    return false;
  }
  for (int i=0; i<aall.ah.numOfAct; i++) {
    ParseActData_AnyAct(&dat, i);
  }

  ParseActData_Sound(&dat);
  try {
    aall.interval = new float[aall.ah.numOfAct];
  } catch (...) {
    return false;
  }
  ParseActData_Interval(dat);
  
  return true;
}

bool CACT::ParseActData_Header(BYTE *dat)
{
  memcpy(&aall.ah, dat, sizeof(ActHeader));

  if (aall.ah.magic != 0x4341) return false;

  return true;
}

void CACT::ParseActData_AnyAct(BYTE **dat, int no)
{
  DWORD numpat;
  int version;

  numpat = *(DWORD *)(*dat);
  (*dat) += 4;
  aall.aa[no].numOfPat = numpat;
  try {
    aall.aa[no].aaap = new AnyActAnyPat[numpat];
  } catch (...) {
    return;
  }
  version = aall.ah.version & 0xFFFF;
  for (unsigned int i=0; i<numpat; i++) {
    memcpy(&aall.aa[no].aaap[i].ph, *dat, sizeof(PatHeader));
    (*dat) += sizeof(PatHeader);
    aall.aa[no].aaap[i].numOfSpr = *(DWORD *)(*dat);
    (*dat) += sizeof(DWORD);
    MemAllocAnyPatAnySpr(no, i, version);
    for (unsigned int j=0; j<aall.aa[no].aaap[i].numOfSpr; j++) {
      ParseActData_AnyPatAnySpr(dat, no, i, j, version);
    }

    if (version == 0x0101) {
      aall.aa[no].aaap[i].sndNo = 0;
      aall.aa[no].aaap[i].numxxx = 0;
      aall.aa[no].aaap[i].Ext1 = 0;
      aall.aa[no].aaap[i].ExtXoffs = 0;
      aall.aa[no].aaap[i].ExtYoffs = 0;
      aall.aa[no].aaap[i].terminate = 0;
      continue;
    }

    aall.aa[no].aaap[i].sndNo = *(int *)(*dat);
    (*dat) += 4;

    if (version == 0x0201) {
      aall.aa[no].aaap[i].numxxx = 0;
    } else {
      aall.aa[no].aaap[i].numxxx = *(DWORD *)(*dat);
      (*dat) += 4;
      if (aall.aa[no].aaap[i].numxxx == 1) {
        aall.aa[no].aaap[i].Ext1 = *(DWORD *)(*dat);
        (*dat) += 4;
        aall.aa[no].aaap[i].ExtXoffs = *(int *)(*dat);
        (*dat) += 4;
        aall.aa[no].aaap[i].ExtYoffs = *(int *)(*dat);
        (*dat) += 4;
        aall.aa[no].aaap[i].terminate = *(DWORD *)(*dat);
        (*dat) += 4;
      } else {
        aall.aa[no].aaap[i].Ext1 = 0;
        aall.aa[no].aaap[i].ExtXoffs = 0;
        aall.aa[no].aaap[i].ExtYoffs = 0;
        aall.aa[no].aaap[i].terminate = 0;
      }
    }
  }
}

void CACT::MemAllocAnyPatAnySpr(int no, int spr, int version)
{
  try {
    switch (version) {
      case 0x0101:
        aall.aa[no].aaap[spr].aps0 = new AnyPatSprV0101[aall.aa[no].aaap[spr].numOfSpr];
        break;
      case 0x0201:
      case 0x0202:
      case 0x0203:
        aall.aa[no].aaap[spr].aps1 = new AnyPatSprV0201[aall.aa[no].aaap[spr].numOfSpr];
        break;
      case 0x0204:
        aall.aa[no].aaap[spr].aps4 = new AnyPatSprV0204[aall.aa[no].aaap[spr].numOfSpr];
        break;
      case 0x0205:
        aall.aa[no].aaap[spr].aps5 = new AnyPatSprV0205[aall.aa[no].aaap[spr].numOfSpr];
        break;
      default:
        return;
    }
  } catch (...) {
    aall.aa[no].aaap[spr].aps1 = NULL;
    return;
  }
}

void CACT::ParseActData_AnyPatAnySpr(BYTE **dat, int actNo, int patNo, int sprNo, int version)
{
  switch (version) {
    case 0x0101:
      memcpy(&aall.aa[actNo].aaap[patNo].aps0[sprNo], *dat, sizeof(AnyPatSprV0101));
      *dat += sizeof(AnyPatSprV0101);
      break;
    case 0x0201:
    case 0x0202:
    case 0x0203:
      memcpy(&aall.aa[actNo].aaap[patNo].aps1[sprNo], *dat, sizeof(AnyPatSprV0201));
      *dat += sizeof(AnyPatSprV0201);
      break;
    case 0x0204:
      memcpy(&aall.aa[actNo].aaap[patNo].aps4[sprNo], *dat, sizeof(AnyPatSprV0204));
      *dat += sizeof(AnyPatSprV0204);
      break;
    case 0x0205:
      memcpy(&aall.aa[actNo].aaap[patNo].aps5[sprNo], *dat, sizeof(AnyPatSprV0205));
      *dat += sizeof(AnyPatSprV0205);
      break;
  }
}

void CACT::ParseActData_Sound(BYTE **dat)
{
  if ((aall.ah.version & 0xFFFF) < 0x0200) {
    aall.numOfSound = 0;
    aall.fnameSnd = NULL;
    return;
  }
  aall.numOfSound = *(DWORD *)*dat;
  *dat += 4;
  if (aall.numOfSound > 0) {
    try {
      aall.fnameSnd = new char* [aall.numOfSound];
    } catch (...) {
      return;
    }
    for (int i=0; i<aall.numOfSound; i++) {
      try {
        aall.fnameSnd[i] = new char[40];
      } catch (...) {
        return;
      }
      memcpy(aall.fnameSnd[i], *dat, 40);
      *dat += 40;
    }
  } else {
    aall.fnameSnd = NULL;
  }
}

void CACT::ParseActData_Interval(BYTE *dat)
{
  if ((aall.ah.version&0xFFFF) < 0x0202) return;
  memcpy(aall.interval, dat, sizeof(float)*aall.ah.numOfAct);
}

void CACT::WriteAct(char *fname)
{
  FILE *fp;

  if ((fp = fopen(fname, "wb")) == NULL)  return;
  fwrite(&aall.ah, 1, sizeof(ActHeader), fp);  // Header
  int version = aall.ah.version & 0xFFFF;
  int numPat, numSpr;
  for (int i=0; i<aall.ah.numOfAct; i++) {
    numPat = aall.aa[i].numOfPat;
    fwrite(&numPat, 1, sizeof(numPat), fp);
    for (int j=0; j<numPat; j++) {
      fwrite(&aall.aa[i].aaap[j].ph, 1, sizeof(PatHeader), fp);
      numSpr = aall.aa[i].aaap[j].numOfSpr;
      fwrite(&numSpr, 1, sizeof(DWORD), fp);
      for (int k=0; k<numSpr; k++) {
        switch (version) {
        case 0x0101:
          fwrite(&aall.aa[i].aaap[j].aps0[k], 1, sizeof(AnyPatSprV0101), fp);
          break;
        case 0x0201:
        case 0x0202:
        case 0x0203:
          fwrite(&aall.aa[i].aaap[j].aps1[k], 1, sizeof(AnyPatSprV0201), fp);
          break;
        case 0x0204:
          fwrite(&aall.aa[i].aaap[j].aps4[k], 1, sizeof(AnyPatSprV0204), fp);
          break;
        case 0x0205:
          fwrite(&aall.aa[i].aaap[j].aps5[k], 1, sizeof(AnyPatSprV0205), fp);
          break;
        }
      }
      if (version == 0x0101)  continue;
      fwrite(&aall.aa[i].aaap[j].sndNo, 1, sizeof(int), fp);
      //ext
      if (version == 0x0201)  continue;
      fwrite(&aall.aa[i].aaap[j].numxxx, 1, sizeof(DWORD), fp);
      for (unsigned int k=0; k<aall.aa[i].aaap[j].numxxx; k++) {
        fwrite(&aall.aa[i].aaap[j].Ext1, 1, sizeof(DWORD), fp);
        fwrite(&aall.aa[i].aaap[j].ExtXoffs, 1, sizeof(int), fp);
        fwrite(&aall.aa[i].aaap[j].ExtYoffs, 1, sizeof(int), fp);
        fwrite(&aall.aa[i].aaap[j].terminate, 1, sizeof(DWORD), fp);
      }
    }
  }
  if (version != 0x0101) {
    //sound
    int numS = aall.numOfSound;
    fwrite(&numS, 1, sizeof(int), fp);
    for (int i=0; i<numS; i++) {
      fwrite(aall.fnameSnd[i], 1, 40, fp);
    }
    if (version != 0x0201) {
      //xxx
      for (int i=0; i<aall.ah.numOfAct; i++) {
        fwrite(&aall.interval[i], 1, sizeof(float), fp);
      }
    }
  }
  fclose(fp);
}

void CACT::WriteActText(char *fname)
{
  FILE *fp;

  if ((fp = fopen(fname, "w")) == NULL)  return;
  //fwrite(&aall.ah, 1, sizeof(ActHeader), fp);  // Header
  ActHeader *atmp;
  atmp = &aall.ah;
  fprintf(fp, "%04X %04X %04X %04X %08X %08X\n", atmp->magic, atmp->version,
    atmp->numOfAct, atmp->xxx4, atmp->xxx5, atmp->xxx6);
  
  int version = aall.ah.version & 0xFFFF;
  int numPat, numSpr;
  PatHeader *ptmp;
  AnyPatSprV0101 *s0tmp;
  AnyPatSprV0201 *s1tmp;
  AnyPatSprV0204 *s4tmp;
  AnyPatSprV0205 *s5tmp;
  for (int i=0; i<aall.ah.numOfAct; i++) {
    numPat = aall.aa[i].numOfPat;
    //fwrite(&numPat, 1, sizeof(numPat), fp);
    fprintf(fp, "act No.%3d\nnum pattern %08X\n", i, numPat);
    for (int j=0; j<numPat; j++) {
      //fwrite(&aall.aa[i].aaap[j].ph, 1, sizeof(PatHeader), fp);
      ptmp = &aall.aa[i].aaap[j].ph;
      fprintf(fp, "%08X %08X %08X %08X %08X %08X %08X %08X\n", ptmp->pal1, ptmp->pal2,
        ptmp->xxx3, ptmp->xxx4, ptmp->xxx5, ptmp->xxx6, ptmp->xxx7, ptmp->xxx8);
      numSpr = aall.aa[i].aaap[j].numOfSpr;
      //fwrite(&numSpr, 1, sizeof(DWORD), fp);
      fprintf(fp, "%08X\n", numSpr);
      for (int k=0; k<numSpr; k++) {
        switch (version) {
        case 0x0101:
          //fwrite(&aall.aa[i].aaap[j].aps0[k], 1, sizeof(AnyPatSprV0101), fp);
          s0tmp = &aall.aa[i].aaap[j].aps0[k];
          fprintf(fp, "%08X %08X %08X %08X\n", s0tmp->xOffs, s0tmp->yOffs, s0tmp->sprNo, s0tmp->mirrorOn);
          break;
        case 0x0201:
        case 0x0202:
        case 0x0203:
          //fwrite(&aall.aa[i].aaap[j].aps1[k], 1, sizeof(AnyPatSprV0201), fp);
          s1tmp = &aall.aa[i].aaap[j].aps1[k];
          fprintf(fp, "%08X %08X %08X %08X %08X %08X %08X %08X\n", s1tmp->xOffs, s1tmp->yOffs,
            s1tmp->sprNo, s1tmp->mirrorOn, s1tmp->color, *(DWORD *)&s1tmp->xyMag, s1tmp->rot, s1tmp->spType);
          break;
        case 0x0204:
          //fwrite(&aall.aa[i].aaap[j].aps4[k], 1, sizeof(AnyPatSprV0204), fp);
          s4tmp = &aall.aa[i].aaap[j].aps4[k];
          fprintf(fp, "%08X %08X %08X %08X %08X %08X %08X %08X %08X\n", s4tmp->xOffs, s4tmp->yOffs,
            s4tmp->sprNo, s4tmp->mirrorOn, s4tmp->color, *(DWORD *)&s4tmp->xMag, *(DWORD *)&s4tmp->yMag,
            s4tmp->rot, s4tmp->spType);
          break;
        case 0x0205:
          //fwrite(&aall.aa[i].aaap[j].aps5[k], 1, sizeof(AnyPatSprV0205), fp);
          s5tmp = &aall.aa[i].aaap[j].aps5[k];
          fprintf(fp, "%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
            s5tmp->xOffs, s5tmp->yOffs, s5tmp->sprNo, s5tmp->mirrorOn, s5tmp->color,
            *(DWORD *)&s5tmp->xMag, *(DWORD *)&s5tmp->yMag, s5tmp->rot, s5tmp->spType, s5tmp->sprW, s5tmp->sprH);
          break;
        }
      }
      if (version == 0x0101)  continue;
      //fwrite(&aall.aa[i].aaap[j].xxx, 1, sizeof(DWORD), fp);
      fprintf(fp, "%08X\n", aall.aa[i].aaap[j].sndNo);
      //ext
      if (version == 0x0201)  continue;
      //fwrite(&aall.aa[i].aaap[j].numxxx, 1, sizeof(DWORD), fp);
      fprintf(fp, "%08X ", aall.aa[i].aaap[j].numxxx);
      for (unsigned int k=0; k<aall.aa[i].aaap[j].numxxx; k++) {
        //fwrite(&aall.aa[i].aaap[j].Ext1, 1, sizeof(DWORD), fp);
        //fwrite(&aall.aa[i].aaap[j].ExtXoffs, 1, sizeof(int), fp);
        //fwrite(&aall.aa[i].aaap[j].ExtYoffs, 1, sizeof(int), fp);
        //fwrite(&aall.aa[i].aaap[j].terminate, 1, sizeof(DWORD), fp);
        fprintf(fp, "%08X %08X %08X %08X", aall.aa[i].aaap[j].Ext1, aall.aa[i].aaap[j].ExtXoffs,
          aall.aa[i].aaap[j].ExtYoffs, aall.aa[i].aaap[j].terminate);
      }
      fprintf(fp, "\n\n");
    }
  }
  if (version != 0x0101) {
    //sound
    int numS = aall.numOfSound;
    //fwrite(&numS, 1, sizeof(int), fp);
    fprintf(fp, "%08X\n", numS);
    for (int i=0; i<numS; i++) {
      //fwrite(aall.buf[i], 1, 40, fp);
      fprintf(fp, "%s\n", aall.fnameSnd[i]);
    }
    if (version != 0x0201) {
      //xxx
      for (int i=0; i<aall.ah.numOfAct; i++) {
        //fwrite(&aall.xxx[i], 1, sizeof(float), fp);
        fprintf(fp, "%08X\n", *(DWORD *)&aall.interval[i]);
      }
    }
  }
  fclose(fp);
}

void CACT::SwapSpriteOrder(int actNo, int patNo, int spA, int spB)
{
  modified = true;
  switch (aall.ah.version & 0xFFFF) {
  case 0x0101:
    SwapSpriteOrderV0101(actNo, patNo, spA, spB);
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    SwapSpriteOrderV0201(actNo, patNo, spA, spB);
    break;
  case 0x0204:
    SwapSpriteOrderV0204(actNo, patNo, spA, spB);
    break;
  case 0x0205:
    SwapSpriteOrderV0205(actNo, patNo, spA, spB);
    break;
  }
}

INLINE void CACT::Swap(int *a, int *b)
{
  int itmp;

  itmp = *a;
  *a = *b;
  *b = itmp;
}

INLINE void CACT::Swap(DWORD *a, DWORD *b)
{
  DWORD dtmp;

  dtmp = *a;
  *a = *b;
  *b = dtmp;
}

INLINE void CACT::Swap(float *a, float *b)
{
  float ftmp;

  ftmp = *a;
  *a = *b;
  *b = ftmp;
}

INLINE void CACT::SwapSpriteOrderV0101(int actNo, int patNo, int spA, int spB)
{
  AnyPatSprV0101 *ap0;

  ap0 = ((aall.aa+actNo)->aaap + patNo)->aps0;
  Swap(&(ap0+spA)->xOffs, &(ap0+spB)->xOffs);
  Swap(&(ap0+spA)->yOffs, &(ap0+spB)->yOffs);
  Swap(&(ap0+spA)->sprNo, &(ap0+spB)->sprNo);
  Swap(&(ap0+spA)->mirrorOn, &(ap0+spB)->mirrorOn);
}

INLINE void CACT::SwapSpriteOrderV0201(int actNo, int patNo, int spA, int spB)
{
  AnyPatSprV0201 *ap1;

  ap1 = ((aall.aa+actNo)->aaap + patNo)->aps1;
  Swap(&(ap1+spA)->xOffs, &(ap1+spB)->xOffs);
  Swap(&(ap1+spA)->yOffs, &(ap1+spB)->yOffs);
  Swap(&(ap1+spA)->sprNo, &(ap1+spB)->sprNo);
  Swap(&(ap1+spA)->mirrorOn, &(ap1+spB)->mirrorOn);
  Swap(&(ap1+spA)->color, &(ap1+spB)->color);
  Swap(&(ap1+spA)->xyMag, &(ap1+spB)->xyMag);
  Swap(&(ap1+spA)->rot, &(ap1+spB)->rot);
  Swap(&(ap1+spA)->spType, &(ap1+spB)->spType);
}

INLINE void CACT::SwapSpriteOrderV0204(int actNo, int patNo, int spA, int spB)
{
  AnyPatSprV0204 *ap4;

  ap4 = ((aall.aa+actNo)->aaap + patNo)->aps4;
  Swap(&(ap4+spA)->xOffs, &(ap4+spB)->xOffs);
  Swap(&(ap4+spA)->yOffs, &(ap4+spB)->yOffs);
  Swap(&(ap4+spA)->sprNo, &(ap4+spB)->sprNo);
  Swap(&(ap4+spA)->mirrorOn, &(ap4+spB)->mirrorOn);
  Swap(&(ap4+spA)->color, &(ap4+spB)->color);
  Swap(&(ap4+spA)->xMag, &(ap4+spB)->xMag);
  Swap(&(ap4+spA)->yMag, &(ap4+spB)->yMag);
  Swap(&(ap4+spA)->rot, &(ap4+spB)->rot);
  Swap(&(ap4+spA)->spType, &(ap4+spB)->spType);
}

INLINE void CACT::SwapSpriteOrderV0205(int actNo, int patNo, int spA, int spB)
{
  AnyPatSprV0205 *ap5;

  ap5 = ((aall.aa+actNo)->aaap + patNo)->aps5;
  Swap(&(ap5+spA)->xOffs, &(ap5+spB)->xOffs);
  Swap(&(ap5+spA)->yOffs, &(ap5+spB)->yOffs);
  Swap(&(ap5+spA)->sprNo, &(ap5+spB)->sprNo);
  Swap(&(ap5+spA)->mirrorOn, &(ap5+spB)->mirrorOn);
  Swap(&(ap5+spA)->color, &(ap5+spB)->color);
  Swap(&(ap5+spA)->xMag, &(ap5+spB)->xMag);
  Swap(&(ap5+spA)->yMag, &(ap5+spB)->yMag);
  Swap(&(ap5+spA)->rot, &(ap5+spB)->rot);
  Swap(&(ap5+spA)->spType, &(ap5+spB)->spType);
  Swap(&(ap5+spA)->sprW, &(ap5+spB)->sprW);
  Swap(&(ap5+spA)->sprH, &(ap5+spB)->sprH);
}


void CACT::MoveSprite(int xOffs, int yOffs, int actNo, int patNo, int sprid)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    aall.aa[actNo].aaap[patNo].aps0[sprid].xOffs += xOffs;
    aall.aa[actNo].aaap[patNo].aps0[sprid].yOffs += yOffs;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprid].xOffs += xOffs;
    aall.aa[actNo].aaap[patNo].aps1[sprid].yOffs += yOffs;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprid].xOffs += xOffs;
    aall.aa[actNo].aaap[patNo].aps4[sprid].yOffs += yOffs;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprid].xOffs += xOffs;
    aall.aa[actNo].aaap[patNo].aps5[sprid].yOffs += yOffs;
    break;
  }
}

void CACT::MoveExtSprite(int xOffs, int yOffs, int actNo, int patNo)
{
  modified = true;
  aall.aa[actNo].aaap[patNo].ExtXoffs += xOffs;
  aall.aa[actNo].aaap[patNo].ExtYoffs += yOffs;
}

void CACT::ChangeMagSprite(float xMagRatio, float yMagRatio, int actNo, int patNo, int sprid)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  if (xMagRatio == 0 || yMagRatio == 0) return;
  switch (version) {
  /*
  case 0x0101:
    break;
  */
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprid].xyMag *= xMagRatio;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprid].xMag *= xMagRatio;
    aall.aa[actNo].aaap[patNo].aps4[sprid].yMag *= yMagRatio;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprid].xMag *= xMagRatio;
    aall.aa[actNo].aaap[patNo].aps5[sprid].yMag *= yMagRatio;
    break;
  }
}

void CACT::ChangeRotSprite(int diffRot, int actNo, int patNo, int sprid)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  /*
  case 0x0101:
    break;
  */
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprid].rot += diffRot;
    NormalizeRot(&aall.aa[actNo].aaap[patNo].aps1[sprid].rot);
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprid].rot += diffRot;
    NormalizeRot(&aall.aa[actNo].aaap[patNo].aps4[sprid].rot);
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprid].rot += diffRot;
    NormalizeRot(&aall.aa[actNo].aaap[patNo].aps5[sprid].rot);
    break;
  }
}

void CACT::NormalizeRot(DWORD *rot)
{
  int r = *rot;
  r %= 360;
  if (r < 0)  r += 360;
  *rot = r;
}

void CACT::AddSpr(int actNo, int patNo, int sprNo, int spType, int spWidth, int spHeight)
{
  modified = true;
  AnyActAnyPat *aaap1;
  aaap1 = &aall.aa[actNo].aaap[patNo];

  int version = aall.ah.version & 0xFFFF;
  switch (version) {
  case 0x0101:
    AddSprV0101(aaap1, sprNo, spType);
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    AddSprV0201(aaap1, sprNo, spType);
    break;
  case 0x0204:
    AddSprV0204(aaap1, sprNo, spType);
    break;
  case 0x0205:
    AddSprV0205(aaap1, sprNo, spType, spWidth, spHeight);
    break;
  }
}

void CACT::AddSprV0101(AnyActAnyPat *aaap1, int sprNo, int spType)
{
  AnyPatSprV0101 *aps;

  aaap1->numOfSpr++;
  aaap1->aps0 = (AnyPatSprV0101 *)realloc(aaap1->aps0, sizeof(AnyPatSprV0101) * aaap1->numOfSpr);
  aps = &aaap1->aps0[aaap1->numOfSpr-1];
  aps->xOffs = 0;
  aps->yOffs = 0;
  aps->sprNo = sprNo;
  aps->mirrorOn = 0;
}

void CACT::AddSprV0201(AnyActAnyPat *aaap1, int sprNo, int spType)
{
  AnyPatSprV0201 *aps;

  aaap1->numOfSpr++;
  aaap1->aps1 = (AnyPatSprV0201 *)realloc(aaap1->aps1, sizeof(AnyPatSprV0201) * aaap1->numOfSpr);
  aps = &aaap1->aps1[aaap1->numOfSpr-1];
  aps->xOffs = 0;
  aps->yOffs = 0;
  aps->sprNo = sprNo;
  aps->mirrorOn = 0;
  aps->color = 0xFFFFFFFF;
  aps->xyMag = 1.0;
  aps->rot = 0;
  aps->spType = spType;
}

void CACT::AddSprV0204(AnyActAnyPat *aaap1, int sprNo, int spType)
{
  AnyPatSprV0204 *aps;

  aaap1->numOfSpr++;
  aaap1->aps4 = (AnyPatSprV0204 *)realloc(aaap1->aps4, sizeof(AnyPatSprV0204) * aaap1->numOfSpr);
  aps = &aaap1->aps4[aaap1->numOfSpr-1];
  aps->xOffs = 0;
  aps->yOffs = 0;
  aps->sprNo = sprNo;
  aps->mirrorOn = 0;
  aps->color = 0xFFFFFFFF;
  aps->xMag = 1.0;
  aps->yMag = 1.0;
  aps->rot = 0;
  aps->spType = spType;
}

void CACT::AddSprV0205(AnyActAnyPat *aaap1, int sprNo, int spType, int spWidth, int spHeight)
{
  AnyPatSprV0205 *aps;

  aaap1->numOfSpr++;
  aaap1->aps5 = (AnyPatSprV0205 *)realloc(aaap1->aps5, sizeof(AnyPatSprV0205) * aaap1->numOfSpr);
  aps = &aaap1->aps5[aaap1->numOfSpr-1];
  aps->xOffs = 0;
  aps->yOffs = 0;
  aps->sprNo = sprNo;
  aps->mirrorOn = 0;
  aps->color = 0xFFFFFFFF;
  aps->xMag = 1.0;
  aps->yMag = 1.0;
  aps->rot = 0;
  aps->spType = spType;
  aps->sprW = spWidth;
  aps->sprH = spHeight;
}

void CACT::DelSpr(int actNo, int patNo, int sprId)
{
  modified = true;
  AnyActAnyPat *aaap1;
  aaap1 = &aall.aa[actNo].aaap[patNo];
  if (aaap1->numOfSpr == 1)  return;
  if ((DWORD)sprId >= aaap1->numOfSpr)  return;
  for (DWORD i=sprId; i<aaap1->numOfSpr-1; i++) {
    SwapSpriteOrder(actNo, patNo, i, i+1);
  }

  if (aaap1->numOfSpr > 0)  aaap1->numOfSpr--;
}

void CACT::AddPat(int actNo, int basePatNo)
{
  modified = true;
  int num = aall.aa[actNo].numOfPat;
  AnyActAnyPat *aaapn, *aaapb;
  aall.aa[actNo].aaap = (AnyActAnyPat *)realloc(aall.aa[actNo].aaap, sizeof(AnyActAnyPat) * (num+1));
  aall.aa[actNo].numOfPat++;
  aaapn = &aall.aa[actNo].aaap[num];
  aaapn->aps1 = NULL;
  aaapb = &aall.aa[actNo].aaap[basePatNo];
  CopyAnyActAnyPat(aaapn, aaapb);
}

void CACT::CopyAnyActAnyPat(AnyActAnyPat *to, AnyActAnyPat *from)
{
  memcpy(&to->ph, &from->ph, sizeof(PatHeader));
  to->numOfSpr = from->numOfSpr;
  free(to->aps1);
  switch (aall.ah.version & 0xFFFF) {
  case 0x0101:
    to->aps0 = (AnyPatSprV0101 *)malloc(sizeof(AnyPatSprV0101) * from->numOfSpr);
    memcpy(to->aps0, from->aps0, sizeof(AnyPatSprV0101) * from->numOfSpr);
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    to->aps1 = (AnyPatSprV0201 *)malloc(sizeof(AnyPatSprV0201) * from->numOfSpr);
    memcpy(to->aps1, from->aps1, sizeof(AnyPatSprV0201) * from->numOfSpr);
    break;
  case 0x0204:
    to->aps4 = (AnyPatSprV0204 *)malloc(sizeof(AnyPatSprV0204) * from->numOfSpr);
    memcpy(to->aps4, from->aps4, sizeof(AnyPatSprV0204) * from->numOfSpr);
    break;
  case 0x0205:
    to->aps5 = (AnyPatSprV0205 *)malloc(sizeof(AnyPatSprV0205) * from->numOfSpr);
    memcpy(to->aps5, from->aps5, sizeof(AnyPatSprV0205) * from->numOfSpr);
    break;
  }
  to->sndNo = from->sndNo;
  to->numxxx = from->numxxx;
  to->Ext1 = from->Ext1;
  to->ExtXoffs = from->ExtXoffs;
  to->ExtYoffs = from->ExtYoffs;
  to->terminate = from->terminate;
}

void CACT::SwapPat(int actNo, int patA, int patB)
{
  modified = true;

  AnyActAnyPat tmpaaap;
  tmpaaap.aps1 = NULL;

  CopyAnyActAnyPat(&tmpaaap, &aall.aa[actNo].aaap[patA]);
  CopyAnyActAnyPat(&aall.aa[actNo].aaap[patA], &aall.aa[actNo].aaap[patB]);
  CopyAnyActAnyPat(&aall.aa[actNo].aaap[patB], &tmpaaap);

  free(tmpaaap.aps1);
}

void CACT::DelPat(int actNo, int patNo)
{
  modified = true;
  int *num;
  num = &aall.aa[actNo].numOfPat;

  // Pat is deleted move to last
  for (int i=patNo; i<(*num-1); i++) {
    SwapPat(actNo, i, i+1);
  }
  (*num)--;
  // delete last
  free(aall.aa[actNo].aaap[*num].aps1);
  aall.aa[actNo].aaap = (AnyActAnyPat *)realloc(aall.aa[actNo].aaap, sizeof(AnyActAnyPat) * (*num));
}

void CACT::SlideSprNo(int aftThis, bool plus)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;
  int offs;
  if (plus) {
    offs = 1;
  } else {
    offs = -1;
  }
  for (int i=0; i<aall.ah.numOfAct; i++) {
    for (int j=0; j<aall.aa[i].numOfPat; j++) {
      switch (version) {
      case 0x0101:
        for (DWORD k=aftThis; k<aall.aa[i].aaap[j].numOfSpr; k++) {
          aall.aa[i].aaap[j].aps0[k].sprNo += offs;
          if (aall.aa[i].aaap[j].aps0[k].sprNo & 0x80000000) {
            aall.aa[i].aaap[j].aps0[k].sprNo = 0;
          }
        }
        break;
      case 0x0201:
      case 0x0202:
      case 0x0203:
        for (DWORD k=aftThis; k<aall.aa[i].aaap[j].numOfSpr; k++) {
          aall.aa[i].aaap[j].aps1[k].sprNo += offs;
          if (aall.aa[i].aaap[j].aps1[k].sprNo & 0x80000000) {
            aall.aa[i].aaap[j].aps1[k].sprNo = 0;
          }
        }
        break;
      case 0x0204:
        for (DWORD k=aftThis; k<aall.aa[i].aaap[j].numOfSpr; k++) {
          aall.aa[i].aaap[j].aps4[k].sprNo += offs;
          if (aall.aa[i].aaap[j].aps4[k].sprNo & 0x80000000) {
            aall.aa[i].aaap[j].aps4[k].sprNo = 0;
          }
        }
        break;
      case 0x0205:
        for (DWORD k=aftThis; k<aall.aa[i].aaap[j].numOfSpr; k++) {
          aall.aa[i].aaap[j].aps5[k].sprNo += offs;
          if (aall.aa[i].aaap[j].aps5[k].sprNo & 0x80000000) {
            aall.aa[i].aaap[j].aps5[k].sprNo = 0;
          }
        }
        break;
      }
    }
  }
}

int CACT::GetNumAction(void)
{
  return aall.ah.numOfAct;
}

int CACT::GetNumPattern(int actNo)
{
  return aall.aa[actNo].numOfPat;
}

int CACT::GetNumSprites(int actNo, int patNo)
{
  return aall.aa[actNo].aaap[patNo].numOfSpr;
}

bool CACT::GetModified(void)
{
  return modified;
}

void CACT::SetModified(void)
{
  modified = true;
}

WORD CACT::GetVersion(void)
{
  return aall.ah.version & 0xFFFF;
}

void CACT::GetOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo, int sprid)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    *xOffs = aall.aa[actNo].aaap[patNo].aps0[sprid].xOffs;
    *yOffs = aall.aa[actNo].aaap[patNo].aps0[sprid].yOffs;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    *xOffs = aall.aa[actNo].aaap[patNo].aps1[sprid].xOffs;
    *yOffs = aall.aa[actNo].aaap[patNo].aps1[sprid].yOffs;
    break;
  case 0x0204:
    *xOffs = aall.aa[actNo].aaap[patNo].aps4[sprid].xOffs;
    *yOffs = aall.aa[actNo].aaap[patNo].aps4[sprid].yOffs;
    break;
  case 0x0205:
    *xOffs = aall.aa[actNo].aaap[patNo].aps5[sprid].xOffs;
    *yOffs = aall.aa[actNo].aaap[patNo].aps5[sprid].yOffs;
    break;
  }
}

void CACT::GetSprNoValue(int actNo, int patNo, int sprId, int *sprNo)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    *sprNo = aall.aa[actNo].aaap[patNo].aps0[sprId].sprNo;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    *sprNo = aall.aa[actNo].aaap[patNo].aps1[sprId].sprNo;
    break;
  case 0x0204:
    *sprNo = aall.aa[actNo].aaap[patNo].aps4[sprId].sprNo;
    break;
  case 0x0205:
    *sprNo = aall.aa[actNo].aaap[patNo].aps5[sprId].sprNo;
    break;
  }
}

float CACT::GetXMagValue(int actNo, int patNo, int sprId)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aall.aa[actNo].aaap[patNo].aps1[sprId].xyMag;
  case 0x0204:
    return aall.aa[actNo].aaap[patNo].aps4[sprId].xMag;
  case 0x0205:
    return aall.aa[actNo].aaap[patNo].aps5[sprId].xMag;
  }
  return 1.0;
}

float CACT::GetYMagValue(int actNo, int patNo, int sprId)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aall.aa[actNo].aaap[patNo].aps1[sprId].xyMag;
  case 0x0204:
    return aall.aa[actNo].aaap[patNo].aps4[sprId].yMag;
  case 0x0205:
    return aall.aa[actNo].aaap[patNo].aps5[sprId].yMag;
  }
  return 1.0;
}

DWORD CACT::GetRotValue(int actNo, int patNo, int sprId)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aall.aa[actNo].aaap[patNo].aps1[sprId].rot;
  case 0x0204:
    return aall.aa[actNo].aaap[patNo].aps4[sprId].rot;
  case 0x0205:
    return aall.aa[actNo].aaap[patNo].aps5[sprId].rot;
  }
  return 0;
}

DWORD CACT::GetMirrorValue(int actNo, int patNo, int sprId)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    return aall.aa[actNo].aaap[patNo].aps0[sprId].mirrorOn;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aall.aa[actNo].aaap[patNo].aps1[sprId].mirrorOn;
  case 0x0204:
    return aall.aa[actNo].aaap[patNo].aps4[sprId].mirrorOn;
  case 0x0205:
    return aall.aa[actNo].aaap[patNo].aps5[sprId].mirrorOn;
    break;
  }
  return 0;
}

DWORD CACT::GetABGRValue(int actNo, int patNo, int sprId)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aall.aa[actNo].aaap[patNo].aps1[sprId].color;
  case 0x0204:
    return aall.aa[actNo].aaap[patNo].aps4[sprId].color;
  case 0x0205:
    return aall.aa[actNo].aaap[patNo].aps5[sprId].color;
  }
  return 0xFFFFFFFF;
}

void CACT::GetSprWHValue(int actNo, int patNo, int sprId, DWORD *sprW, DWORD *sprH)
{
  int version = aall.ah.version & 0xFFFF;
  if (version != 0x0205)  return;

  *sprW = aall.aa[actNo].aaap[patNo].aps5[sprId].sprW;
  *sprH = aall.aa[actNo].aaap[patNo].aps5[sprId].sprH;
}

void CACT::GetSpTypeValue(int actNo, int patNo, int sprId, int *spType)
{
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    *spType = 0;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    *spType = aall.aa[actNo].aaap[patNo].aps1[sprId].spType;
    break;
  case 0x0204:
    *spType = aall.aa[actNo].aaap[patNo].aps4[sprId].spType;
    break;
  case 0x0205:
    *spType = aall.aa[actNo].aaap[patNo].aps5[sprId].spType;
    break;
  }
}

void CACT::GetExtOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo)
{
  *xOffs = aall.aa[actNo].aaap[patNo].ExtXoffs;
  *yOffs = aall.aa[actNo].aaap[patNo].ExtYoffs;
}

void CACT::SetXOffsValue(int actNo, int patNo, int sprId, int val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    aall.aa[actNo].aaap[patNo].aps0[sprId].xOffs = val;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].xOffs = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].xOffs = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].xOffs = val;
    break;
  }
}

void CACT::SetYOffsValue(int actNo, int patNo, int sprId, int val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    aall.aa[actNo].aaap[patNo].aps0[sprId].yOffs = val;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].yOffs = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].yOffs = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].yOffs = val;
    break;
  }
}

void CACT::SetSprNoValue(int actNo, int patNo, int sprId, DWORD val, DWORD sprW, DWORD sprH)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    aall.aa[actNo].aaap[patNo].aps0[sprId].sprNo = val;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].sprNo = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].sprNo = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].sprNo = val;
    aall.aa[actNo].aaap[patNo].aps5[sprId].sprW = sprW;
    aall.aa[actNo].aaap[patNo].aps5[sprId].sprH = sprH;
    break;
  }
}

void CACT::SetXMagValue(int actNo, int patNo, int sprId, float val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].xyMag = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].xMag = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].xMag = val;
    break;
  }
}

void CACT::SetYMagValue(int actNo, int patNo, int sprId, float val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].xyMag = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].yMag = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].yMag = val;
    break;
  }
}

void CACT::SetRotValue(int actNo, int patNo, int sprId, DWORD val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].rot = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].rot = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].rot = val;
    break;
  }
}

void CACT::SetMirrorValue(int actNo, int patNo, int sprId, DWORD val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    aall.aa[actNo].aaap[patNo].aps0[sprId].mirrorOn = val;
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].mirrorOn = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].mirrorOn = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].mirrorOn = val;
    break;
  }
}

void CACT::SetABGRValue(int actNo, int patNo, int sprId, DWORD val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].color = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].color = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].color = val;
    break;
  }
}

void CACT::SetSpTypeValue(int actNo, int patNo, int sprId, DWORD val)
{
  modified = true;
  int version = aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0201:
  case 0x0202:
  case 0x0203:
    aall.aa[actNo].aaap[patNo].aps1[sprId].spType = val;
    break;
  case 0x0204:
    aall.aa[actNo].aaap[patNo].aps4[sprId].spType = val;
    break;
  case 0x0205:
    aall.aa[actNo].aaap[patNo].aps5[sprId].spType = val;
    break;
  }
}

void CACT::SetExtXValue(int actNo, int patNo, int val)
{
  modified = true;
  aall.aa[actNo].aaap[patNo].ExtXoffs = val;
}

void CACT::SetExtYValue(int actNo, int patNo, int val)
{
  modified = true;
  aall.aa[actNo].aaap[patNo].ExtYoffs = val;
}

void CACT::CopyAction(int destActNo, int srcActNo)
{
  int numPat = aall.aa[destActNo].numOfPat;
  // free destAct
  for (int i=0; i<numPat; i++) {
    delete aall.aa[destActNo].aaap[i].aps1;
  }

  modified = true;
  numPat = aall.aa[srcActNo].numOfPat;
  aall.aa[destActNo].numOfPat = numPat;
  aall.aa[destActNo].aaap = new AnyActAnyPat[ numPat ];
  int numSpr;
  for (int i=0; i<numPat; i++) {
    memcpy(&aall.aa[destActNo].aaap[i].ph, &aall.aa[srcActNo].aaap[i].ph, sizeof(PatHeader));
    numSpr = aall.aa[destActNo].aaap[i].numOfSpr = aall.aa[srcActNo].aaap[i].numOfSpr;
    MemAllocAnyPatAnySpr(destActNo, i, aall.ah.version & 0xFFFF);
    for (int j=0; j<numSpr; j++) {
      switch (aall.ah.version & 0xFFFF) {
      case 0x0101:
        memcpy(&aall.aa[destActNo].aaap[i].aps0[j],
               &aall.aa[srcActNo].aaap[i].aps0[j], sizeof(AnyPatSprV0101));
        break;
      case 0x0201:
      case 0x0202:
      case 0x0203:
        memcpy(&aall.aa[destActNo].aaap[i].aps1[j],
               &aall.aa[srcActNo].aaap[i].aps1[j], sizeof(AnyPatSprV0201));
        break;
      case 0x0204:
        memcpy(&aall.aa[destActNo].aaap[i].aps4[j],
               &aall.aa[srcActNo].aaap[i].aps4[j], sizeof(AnyPatSprV0204));
        break;
      case 0x0205:
        memcpy(&aall.aa[destActNo].aaap[i].aps5[j],
               &aall.aa[srcActNo].aaap[i].aps5[j], sizeof(AnyPatSprV0205));
        break;
      }
    }
    aall.aa[destActNo].aaap[i].sndNo = aall.aa[srcActNo].aaap[i].sndNo;
    aall.aa[destActNo].aaap[i].numxxx = aall.aa[srcActNo].aaap[i].numxxx;
    aall.aa[destActNo].aaap[i].Ext1 = aall.aa[srcActNo].aaap[i].Ext1;
    aall.aa[destActNo].aaap[i].ExtXoffs = aall.aa[srcActNo].aaap[i].ExtXoffs;
    aall.aa[destActNo].aaap[i].ExtYoffs = aall.aa[srcActNo].aaap[i].ExtYoffs;
    aall.aa[destActNo].aaap[i].terminate = aall.aa[srcActNo].aaap[i].terminate;
  }
}

float CACT::GetInterval(int actNo)
{
  if (actNo < 0 || actNo >= GetNumAction())  return FLT_MAX;

  return aall.interval[actNo] * 25;
}

void CACT::SetInterval(int actNo, float val)
{
  modified = true;
  aall.interval[actNo] = val / 25.f;
}

int CACT::GetNumSounds(void)
{
  return aall.numOfSound;
}

int CACT::GetSoundNo(int actNo, int patNo)
{
  return aall.aa[actNo].aaap[patNo].sndNo;
}

void CACT::SetSoundNo(int actNo, int patNo, int sndNo)
{
  modified = true;
  aall.aa[actNo].aaap[patNo].sndNo = sndNo;
}

char *CACT::GetSoundFN(int no)
{
  if (no < aall.numOfSound) {
    return aall.fnameSnd[no];
  } else {
    return NULL;
  }
}

bool CACT::AddSound(char *fname)
{
  modified = true;
  aall.numOfSound++;
  aall.fnameSnd = (char **)realloc(aall.fnameSnd, sizeof(char *) * aall.numOfSound);
  try {
    aall.fnameSnd[aall.numOfSound-1] = new char[40];
  } catch (...) {
    return false;
  }
  strncpy(aall.fnameSnd[aall.numOfSound-1], fname, 39);
  return true;
}

void CACT::SwapSound(int noA, int noB)
{
  modified = true;
  char tmp[40];
  strncpy(tmp, aall.fnameSnd[noA], 39);
  strncpy(aall.fnameSnd[noA], aall.fnameSnd[noB], 39);
  strncpy(aall.fnameSnd[noB], tmp, 39);
}

void CACT::DelSound(int no)
{
  modified = true;
  for (int i=no+1; i<aall.numOfSound; i++) {
    strncpy(aall.fnameSnd[i-1], aall.fnameSnd[i], 39);
  }
  delete aall.fnameSnd[aall.numOfSound-1];
  aall.numOfSound--;
}

int CACT::CorrectNoInsDelPoint(int no)
{
  int p;
                                                                                
  if (no < 0 || no >= GetNumAction()) {
    p = GetNumAction();
  } else {
    p = no;
  }
                                                                                
  return p;
}

void CACT::InsertNullAct8(int no)
{
  int ip = CorrectNoInsDelPoint(no);
  
  //main
  aall.aa = (AnyAction *)realloc(aall.aa, sizeof(AnyAction) * (aall.ah.numOfAct + 8));
  for (int i = aall.ah.numOfAct-1; i >= ip; i--) {
    aall.aa[i+8] = aall.aa[i];
  }
  for (int i = ip; i < ip+8; i++) {
    aall.aa[i].numOfPat = 1;
    aall.aa[i].aaap = (AnyActAnyPat *)calloc(1, sizeof(AnyActAnyPat));
    aall.aa[i].aaap->sndNo = -1;
    aall.aa[i].aaap->terminate = 0xFFFFFFFF;
  }
  //interval
  aall.interval = (float *)realloc(aall.interval, sizeof(float) * (aall.ah.numOfAct + 8));
  for (int i = aall.ah.numOfAct-1; i >= ip; i--) {
    aall.interval[i+8] = aall.interval[i];
  }
  for (int i = ip; i < ip+8; i++) {
    aall.interval[i] = 4.f;
  }

  aall.ah.numOfAct += 8;
}

void CACT::DeleteAct8(int no)
{
  int dp = CorrectNoInsDelPoint(no);

  if (dp > aall.ah.numOfAct-1 - 8)  return;
  
  //main
  for (int i = dp; i < aall.ah.numOfAct; i++) {
    aall.aa[i] = aall.aa[i+8];
  }
  aall.aa = (AnyAction *)realloc(aall.aa, sizeof(AnyAction) * (aall.ah.numOfAct - 8));
  //interval
  for (int i = dp; i < aall.ah.numOfAct; i++) {
    aall.interval[i] = aall.interval[i+8];
  }
  aall.interval = (float *)realloc(aall.interval, sizeof(float) * (aall.ah.numOfAct - 8));
  aall.ah.numOfAct -= 8;
}

