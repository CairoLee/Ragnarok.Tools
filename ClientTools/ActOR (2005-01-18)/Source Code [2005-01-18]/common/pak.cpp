#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pak.h"

#ifdef USE_ZLIB
#include <zlib.h>
#define UNCOMPRESS uncompress
#else
#define UNCOMPRESS fpuncompress
#endif

CPAK::CPAK(void)
{
  NumEntry = NumPAK = 0;
  Entries = NULL;
  tdata = NULL;
  patchOpend = false;
  appendOpend = false;
  pakfname.clear();
  pakversion.clear();

#ifndef USE_ZLIB
  hLib = LoadLibrary("cps.dll");
  fpuncompress = (LPFN_UNCOMPRESS)GetProcAddress(hLib, "uncompress");
#endif

}

CPAK::~CPAK(void)
{
  if (patchOpend)  fclose(fpPatch);
  if (appendOpend)  fclose(fpAppend);
  if (Entries)  free(Entries);
  if (tdata)  free(tdata);
  NumEntry = 0;

#ifndef USE_ZLIB
  FreeLibrary(hLib);
#endif
}

bool CPAK::AddPakFile(const char *filename)
{
  FILE *fp;
  bool pak;

  char suffix[4];
  strcpy(suffix, &filename[strlen(filename)-3]);
  if (STRCASECMP(suffix, "pak") == 0) {
    pak = true;
  } else {
    pak = false;
  }

  if (!PakFileOpen(&fp, filename, pak)) return false;
  if (!EntriesAlloc(fp, pak)) return false;

  EntryRead(fp, pak);

  if (pak) {
    pakversion.push_back(0xFFFFFFFF);
  } else {
    pakversion.push_back(GetGRFVersion(fp));
  }
  pakfname.push_back(filename);

  fclose(fp);
  NumPAK++;

  qsort(Entries, NumEntry, sizeof(FILELIST), FListCompareFuncFN);

  return true;
}

bool CPAK::Uncomp2tmp(const char *filename)
{
  FILELIST *ent;
  BYTE *sdata;

  ent = SearchEntry(filename);
  if (ent == NULL) {
    tdata = NULL;
    tlen = 0;
    return false;
  }
  //if (tdata)  free(tdata);
  delete tdata;
  tlen = ent->declen;
  sdata = (BYTE *)malloc(ent->srclen_aligned);
  tdata = (BYTE *)malloc(tlen);
  if (sdata == NULL || tdata == NULL) {
    printf("can not memory allocate. At %s\n", __LINE__);
    return false;
  }

  FILE *fp;
  fp = fopen(pakfname[ent->pakidx].c_str(), "rb");
  fseek(fp, ent->srcpos, SEEK_SET);
  fread(sdata, 1, ent->srclen_aligned, fp);
  fclose(fp);

  if ( (pakversion[ent->pakidx] == 0xFFFFFFFF)
    || (pakversion[ent->pakidx] == 0) ){ // pak or beta1 grf
    DecodeLZSS(tdata, ent->declen, sdata, ent->srclen);
    free(sdata);
    return true;
  }

  if (ent->type != 1 || (pakversion[ent->pakidx] & 0x0000FF00) == 0x100) {
    BYTE cyc = GetDESCycle(ent);
    DecodeDES(sdata, ent->srclen_aligned, cyc == 0, cyc);
  }

  DWORD ret;
#ifdef USE_ZLIB
  DWORD realdestlen = ent->declen;
#else
  DWORD realdestlen = ent->declen + 0x188;
#endif
  ret = UNCOMPRESS(tdata, &realdestlen, sdata, ent->srclen_aligned);
  free(sdata);
  //if (realdestlen != ent->declen) {
  if (ret != Z_OK) {
    printf("uncompress error. file %s, Zret = %d\n", filename, ret);
    return false;
  }

  return true;
}

bool CPAK::PakFileOpen(FILE **fp, const char *filename, bool pak)
{
  if (*fp = fopen(filename, "rb")) {
    if (pak == false) {
      char mom[15];
      fread(mom, 1, sizeof(mom), *fp);
      if (strncmp(mom, "Master of Magic", 15) != 0) {
        fseek(*fp, -1, SEEK_END);
        BYTE ver;
        fread(&ver, 1, 1, *fp);
        if (ver == 0x12) { // beta1 grf.  This check is too loose. 
          return true;
        }
        printf("file is not .grf file.\n");
        fclose(*fp);
        *fp = NULL;
        return false;
      }
    }
    return true;
  } else {
    printf("file %s not found.\n", filename);
    *fp = NULL;
    return false;
  }
}

bool CPAK::EntriesAlloc(FILE *fp, bool pak)
{
  DWORD nument, tmpd;

  if (pak) {
    fseek(fp, -5, SEEK_END);
    fread(&nument, 1, sizeof(nument), fp);
    nument++;
  } else {
    BYTE ver;
    fseek(fp, 0, SEEK_SET);
    fread(&ver, 1, 1, fp);
    if (ver == 'M') { // 'M' is a head of "Master of Magic"
      fseek(fp, 0x22, SEEK_SET);
      fread(&tmpd, 1, sizeof(tmpd), fp);
      fread(&nument, 1, sizeof(nument), fp);
      nument -= tmpd;
    } else {  // beta1 grf
      fseek(fp, -5, SEEK_END);
      fread(&nument, 1, sizeof(nument), fp);
      nument >>= 16;
      nument++;
    }
  }

  Entries = (FILELIST *)realloc(Entries, sizeof(FILELIST) * (NumEntry+nument));

  if (Entries) {
    return true;
  } else {
    printf("can not memory allocate. At %s\n", __LINE__);
    return false;
  }
}

void CPAK::EntryRead(FILE *fp, bool pak)
{
  if (pak) {
    EntryReadPak(fp, false);
  } else {
    switch (GetGRFVersion(fp) & 0x0000FF00) {
    case 0x0000:
      EntryReadPak(fp, true);
      break;
    case 0x0100:
      EntryReadV1(fp);
      break;
    case 0x0200:
      EntryReadV2(fp);
      break;
    }
  }
}

void CPAK::EntryReadPak(FILE *fp, bool grf)
{
  fseek(fp, -9, SEEK_END);
  DWORD startEnt, lenEnt;
  fread(&startEnt, 1, sizeof(startEnt), fp);
  lenEnt = ftell(fp) - 4 - startEnt;
  BYTE *dat;
  dat = (BYTE *)malloc(lenEnt);
  fseek(fp, startEnt, SEEK_SET);
  fread(dat, 1, lenEnt, fp);

  ParseIndexPak(dat, lenEnt, grf);

  free(dat);
}

void CPAK::EntryReadV1(FILE *fp)
{
  DWORD grfsize;
  fseek(fp, 0, SEEK_END);
  grfsize = ftell(fp);

  DWORD idxsize;
  fseek(fp, 0x1E, SEEK_SET);
  fread(&idxsize, 1, sizeof(idxsize), fp);
  fseek(fp, idxsize+0x2E, SEEK_SET);
  idxsize = grfsize - ftell(fp);

  BYTE *idxdat;
  idxdat = (BYTE *)malloc(idxsize);
  if (idxdat == NULL) {
    printf("can not memory allocate. At %s\n", __LINE__);
    return;
  }
  fread(idxdat, 1, idxsize, fp);

  ParseIndexV1(idxdat, idxsize);

  free(idxdat);
}

void CPAK::EntryReadV2(FILE *fp)
{
  JumpIndexFP(fp);

  DWORD srclen, destlen;
  BYTE *srcdat, *destdat;
  fread(&srclen, 1, sizeof(DWORD), fp);
  fread(&destlen, 1, sizeof(DWORD), fp);
  srcdat = (BYTE *)malloc(srclen);
  destdat = (BYTE *)malloc(destlen);
  if (srcdat == NULL || destdat == NULL) {
    printf("can not memory allocate. At %s\n", __LINE__);
    return;
  }
  fread(srcdat, 1, srclen, fp);

  DWORD ret;
#ifdef USE_ZLIB
  DWORD realdestlen = destlen;
#else
  DWORD realdestlen = destlen + 0x188;
#endif
  ret = UNCOMPRESS(destdat, &realdestlen, srcdat, srclen);
  if (ret != 0/*Z_OK*/) {
    printf("index uncompress failed.\n");
    exit(-1);
  }
  free(srcdat);
  
  ParseIndexV2(destdat, destlen);

  free(destdat);
}

void CPAK::ParseIndexPak(BYTE *dat, int datlen, bool grf)
{
  BYTE *endp = dat + datlen;
  FILELIST ent;
  BYTE lenFName;

  while (dat < endp) {
    lenFName = *dat;  dat++;
    ent.type = *dat;  dat++;
    ent.srcpos = *(DWORD *)dat;  dat += 4;
    ent.srclen = ent.srclen_aligned = *(DWORD *)dat;  dat += 4;
    ent.declen = *(DWORD *)dat;  dat += 4;
    strncpy(ent.fn, (const char *)dat, lenFName+1);
    if (grf) {
      DecodeFilenameB1(ent.fn, lenFName);
    }
    dat += lenFName + 1;
    ent.pakidx = NumPAK;
    EntryAdd(&ent);
  }
}

void CPAK::ParseIndexV1(BYTE *dat, int datlen)
{
  BYTE *endp = dat + datlen;
  FILELIST ent;
  DWORD datoffs;
  char fname[ENTRY_FILENAME_LEN];

  while (dat < endp) {
    datoffs = GETLONG(dat)+4;
    strncpy(fname, DecodeFilename((char *)(dat+6), GETLONG(dat)-6), sizeof(fname));
    if (strlen(fname) >= sizeof(ent.fn) - 1) {
      printf("grf entry filename too long. strlen(\"%s\") > %d\n",
              fname, sizeof(ent.fn));
      exit(-1);
    }
    strncpy(ent.fn, fname, sizeof(ent.fn));
    ent.srclen = GETLONG(dat+datoffs) - GETLONG(dat+datoffs+8) - 715;
    ent.srclen_aligned = GETLONG(dat+datoffs+4) - 37579;
    ent.declen = GETLONG(dat+datoffs+8);
    ent.type = *(dat+datoffs+12);
    if (ent.type == 0) {  // if directory, change ver2 directory
      ent.type = 2;
    }
    ent.srcpos = GETLONG(dat+datoffs+13) + 0x2E;
    ent.pakidx = NumPAK;
    dat += datoffs + 4*3 + 1 + 4;
    EntryAdd(&ent);
  }
}

void CPAK::ParseIndexV2(BYTE *dat, int datlen)
{
  BYTE *endp = dat + datlen;
  char fname[ENTRY_FILENAME_LEN];
  FILELIST ent;

  while (dat < endp) {
    strncpy(fname, (const char *)dat, sizeof(fname));
    if (strlen(fname) >= sizeof(ent.fn) - 1) {
      printf("grf entry filename too long. strlen(\"%s\") > %d\n",
        ent.fn, sizeof(ent.fn));
      exit(-1);
    }
    dat += strlen(fname)+1;
    strncpy(ent.fn, fname, sizeof(ent.fn));
    ent.srclen = GETLONG(dat);  dat += 4;
    ent.srclen_aligned = GETLONG(dat);  dat += 4;
    ent.declen = GETLONG(dat);  dat += 4;
    ent.type = *dat;  dat += 1;
    ent.srcpos = GETLONG(dat)+0x2E;  dat += 4;
    ent.pakidx = NumPAK;
    EntryAdd(&ent);
  }
}

void CPAK::EntryAdd(FILELIST *flist)
{
#ifdef DOUBLECHECK
  int idx = GetExistEntry(flist->fn);
  if (idx >= 0) {
    EntryCopy(Entries+idx, flist);
    return;
  } else {
    EntryCopy(Entries+NumEntry, flist);
    NumEntry++;
  }
#else
  EntryCopy(Entries+NumEntry, flist);
  NumEntry++;
#endif
}

int CPAK::GetExistEntry(char *fn)
{
  for (int i=0; i<NumEntry; i++) {
    if (strcmp(fn, (Entries+i)->fn) == 0) {
      return i;
    }
  }
  return -1;
}

void CPAK::EntryCopy(FILELIST *to, const FILELIST *from)
{
  to->srclen = from->srclen;
  to->srclen_aligned = from->srclen_aligned;
  to->declen = from->declen;
  to->srcpos = from->srcpos;
  to->type = from->type;
  strncpy(to->fn, from->fn, sizeof(to->fn));
  to->pakidx = from->pakidx;
}

char *CPAK::DecodeFilenameB1(char *str, int len)
{
  BYTE c;
  for (int i=0; i<len; i++) {
    c = *(str+i);
    if (c == 0)  break;
    *(str+i) = (c & 0xF0) >> 4 | (c & 0x0F) << 4;
  }

  return str;
}

char *CPAK::DecodeFilename(char *estr, int len)
{
  for (int i=0; i<len; i+=8) {
    NibbleSwap((BYTE *)estr+i, 8);
    BitConvert((BYTE *)estr+i, BitSwapTable1);
    BitConvert4((BYTE *)estr+i);
    BitConvert((BYTE *)estr+i, BitSwapTable2);
  }
  return estr;
}

#ifdef VIOLATION_CODE
DWORD CPAK::FindEmptyArea(DWORD size)
{
  DATAAREA *darea = NULL;
  int numarea;

  numarea = GetDataArea(&darea);
  if (darea->start != 0x2E)
    if (darea->start - 0x2E < size) return 0x2E;

  for (int i=0; i<numarea-1; i++) {
    if ((darea+i+1)->start - (darea+i)->end > size) {
      return (darea+i)->end + 1;
    }
  }
  
  free(darea);

  return 0;
}

char *CPAK::EncodeFilename(char *dstr, int len)
{
  for (int i=0; i<len; i+=8) {
    BitConvert((BYTE *)dstr+i, BitSwapTable1);
    BitConvert4((BYTE *)dstr+i);
    BitConvert((BYTE *)dstr+i, BitSwapTable2);
    NibbleSwap((BYTE *)dstr+i, 8);
  }
  return dstr;
}
#endif // VIOLATION_CODE

void CPAK::DecodeDES(BYTE *dat, int len, bool type, BYTE cycle)
{
  int cnt=0;
  BYTE tmp[8], a;

  if (cycle < 3) cycle = 3;
  else if (cycle < 5) cycle++;
  else if (cycle < 7) cycle += 9;
  else cycle += 15;

  for (int i = 0; i*8 < len; i++, dat += 8) {
    if (i < 20 || (type == false  && i % cycle == 0)) {
      BitConvert(dat, BitSwapTable1);
      BitConvert4(dat);
      BitConvert(dat, BitSwapTable2);
    } else {
      if (cnt == 7 && type == 0) {
        *(DWORD *)tmp = *(DWORD *)dat;
        *(DWORD *)(tmp+4) = *(DWORD *)(dat+4);
        cnt = 0;
        dat[0] = tmp[3];
        dat[1] = tmp[4];
        dat[2] = tmp[6];
        dat[3] = tmp[0];
        dat[4] = tmp[1];
        dat[5] = tmp[2];
        dat[6] = tmp[5];
        a = tmp[7];
        if (a == 0x00) a = 0x2B;
        else if (a == 0x2B)  a = 0x00;
        else if (a == 0x01)  a = 0x68;
        else if (a == 0x68)  a = 0x01;
        else if (a == 0x48)  a = 0x77;
        else if (a == 0x77)  a = 0x48;
        else if (a == 0x60)  a = 0xFF;
        else if (a == 0xFF)  a = 0x60;
        else if (a == 0x6C)  a = 0x80;
        else if (a == 0x80)  a = 0x6C;
        else if (a == 0xB9)  a = 0xC0;
        else if (a == 0xC0)  a = 0xB9;
        else if (a == 0xEB)  a = 0xFE;
        else if (a == 0xFE)  a = 0xEB;
        dat[7] = a;
      }
      cnt++;
    }
  }
}

#ifdef VIOLATION_CODE
void CPAK::EncodeDES(BYTE *dat, int len, bool type, BYTE cycle)
{
  int cnt=0;
  BYTE tmp[8], a;

  if (cycle < 3) cycle = 3;
  else if (cycle < 5) cycle++;
  else if (cycle < 7) cycle += 9;
  else cycle += 15;

  for (int i = 0; i*8 < len; i++, dat += 8) {
    if (i < 20 || (type == false  && i % cycle == 0)) {
      BitConvert(dat, BitSwapTable1);
      BitConvert4(dat);
      BitConvert(dat, BitSwapTable2);
    } else {
      if (cnt == 7 && type == 0) {
        *(DWORD *)tmp = *(DWORD *)dat;
        *(DWORD *)(tmp+4) = *(DWORD *)(dat+4);
        cnt = 0;
        /*
        dat[0] = tmp[3];
        dat[1] = tmp[4];
        dat[2] = tmp[6];
        dat[3] = tmp[0];
        dat[4] = tmp[1];
        dat[5] = tmp[2];
        dat[6] = tmp[5];
        */
        dat[0] = tmp[3];
        dat[1] = tmp[4];
        dat[2] = tmp[5];
        dat[3] = tmp[0];
        dat[4] = tmp[1];
        dat[5] = tmp[6];
        dat[6] = tmp[2];
        a = tmp[7];
        if (a == 0x00) a = 0x2B;
        else if (a == 0x2B)  a = 0x00;
        else if (a == 0x01)  a = 0x68;
        else if (a == 0x68)  a = 0x01;
        else if (a == 0x48)  a = 0x77;
        else if (a == 0x77)  a = 0x48;
        else if (a == 0x60)  a = 0xFF;
        else if (a == 0xFF)  a = 0x60;
        else if (a == 0x6C)  a = 0x80;
        else if (a == 0x80)  a = 0x6C;
        else if (a == 0xB9)  a = 0xC0;
        else if (a == 0xC0)  a = 0xB9;
        else if (a == 0xEB)  a = 0xFE;
        else if (a == 0xFE)  a = 0xEB;
        dat[7] = a;
      }
      cnt++;
    }
  }
}
#endif // VIOLATION_CODE

void CPAK::NibbleSwap(BYTE *src, int len)
{
  for (; 0<len; len--, src++) {
    *src = (*src >> 4) | (*src << 4);
  }
}

void CPAK::BitConvert(BYTE *src, char *BitSwapTable)
{
  int prm;
  BYTE tmp[8];
  *(DWORD *)tmp = *(DWORD *)(tmp+4) = 0;
  for (int i=0; i!=64; i++) {
    prm = BitSwapTable[i] - 1;
    if (src[(prm >> 3) & 7] & BitMaskTable[prm & 7]) {
      tmp[(i >> 3) & 7] |= BitMaskTable[i & 7];
    }
  }
  *(DWORD *)src = *(DWORD *)tmp;
  *(DWORD *)(src+4) = *(DWORD *)(tmp+4);
}

void CPAK::BitConvert4(BYTE *src)
{
  BYTE tmp[8];
  tmp[0] = ((src[7] << 5) | (src[4] >> 3)) & 0x3f;
  tmp[1] = ((src[4] << 1) | (src[5] >> 7)) & 0x3f;
  tmp[2] = ((src[4] << 5) | (src[5] >> 3)) & 0x3f;
  tmp[3] = ((src[5] << 1) | (src[6] >> 7)) & 0x3f;
  tmp[4] = ((src[5] << 5) | (src[6] >> 3)) & 0x3f;
  tmp[5] = ((src[6] << 1) | (src[7] >> 7)) & 0x3f;
  tmp[6] = ((src[6] << 5) | (src[7] >> 3)) & 0x3f;
  tmp[7] = ((src[7] << 1) | (src[4] >> 7)) & 0x3f;

  for (int i=0; i!=4; i++) {
    tmp[i] = (NibbleData[i][tmp[i*2]] & 0xf0) |
      (NibbleData[i][tmp[i*2+1]] & 0x0f);
  }

  int prm;
  *(DWORD *)(tmp+4) = 0;
  for (int i=0; i!=32; i++) {
    prm = BitSwapTable3[i] - 1;
    if (tmp[prm >> 3] & BitMaskTable[prm & 7]) {
      tmp[(i >> 3) + 4] |= BitMaskTable[i & 7];
    }
  }
  *(DWORD *)src ^= *(DWORD *)(tmp+4);
}

void CPAK::JumpIndexFP(FILE *fp)
{
  DWORD indexoffs; 
  fseek(fp, 0x1E, SEEK_SET);
  fread(&indexoffs, 1, sizeof(DWORD), fp);
  fseek(fp, indexoffs+0x2E, SEEK_SET);
}

DWORD CPAK::GetGRFVersion(FILE *fp)
{
  DWORD ver;

  fseek(fp, 0, SEEK_SET);
  fread(&ver, 1, sizeof(ver), fp);
  if (ver == (('t' << 24) | ('s' << 16) | ('a' << 8) | 'M')) {
    fseek(fp, 0x2A, SEEK_SET);
    fread(&ver, 1, sizeof(ver), fp);
    fseek(fp, 0x00, SEEK_SET);
  } else {  // beta1 grf
    ver = 0;
  }

  return ver;  
}

FILELIST *CPAK::SearchEntry(const char *filename)
{
  FILELIST ent;
  FILELIST *first, *ret;

  strcpy(ent.fn, filename);

  first = (FILELIST *)bsearch(&ent, Entries, NumEntry, sizeof(FILELIST), FListCompareFuncFN);
  ret = first;

#ifndef DOUBLECHECK
  if (NumPAK != 1) {
    //backward
    int i;
    if (first > Entries) {
      i = -1;
      while (strcmp(first->fn, (first+i)->fn) == 0) {
        i--;
      }
      i++;
    } else {
      i = 0;
    }
    //forward
    int j = 0;
    if (first < Entries+(NumEntry-1)) {
      j = 1;
      while (strcmp(first->fn, (first+j)->fn) == 0) {
        j++;
      }
      j--;
    } else {
      j = 0;
    }

    int idx = -1;
    for (int k=i; k<=j; k++) {
      if ((first+k)->pakidx > idx) {
        idx = (first+k)->pakidx;
        ret = first+k;
      }
    }
  }
#endif // end of NOT DOUBLECHECK

  return ret;
}

BYTE CPAK::GetSuffixType(const char *filename)
{
  static char suffix[] = {".act.gat.gnd.str"};

  for (int i=0; i<16; i+=4) {
    if (STRNCASECMP(strrchr(filename, '.'), suffix+i, 4) == 0) return 0;
  }

  return 1;
}

BYTE CPAK::GetDESCycle(FILELIST *ent)
{
  switch (pakversion[ent->pakidx] & 0x0000FF00) {
  case 0x0100:
    if (GetSuffixType(ent->fn) == 0)  return 0;
    break;
  case 0x0200:
    if (ent->type == 5) return 0;
    break;
  }

  int digit=1;
  for (unsigned int i=10; i <= ent->srclen; i*=10) {
    digit++;
  }

  return digit;
}

#ifdef VIOLATION_CODE
int CPAK::GetDataArea(DATAAREA **darea)
{
  FILELIST *sEntry;
  int numarea = 0;
  int entries_expDir = 0;

  sEntry = (FILELIST *)malloc(NumEntry * sizeof(FILELIST));
  for (int i=0; i<NumEntry; i++) {
    if ((Entries+i)->type == 2) continue;
    memcpy(sEntry+entries_expDir, Entries+i, sizeof(FILELIST));
    entries_expDir++;
  }
  qsort(sEntry, entries_expDir, sizeof(FILELIST), FListCompareFuncSP);

  DWORD start, end;
  for (int i=0; i<entries_expDir; i++) {
    start = (sEntry+i)->srcpos;
    end = start + (sEntry+i)->srclen_aligned;
    while (end == (sEntry+i+1)->srcpos) {
      i++;
      if (i == entries_expDir) break;
      end += (sEntry+i)->srclen_aligned;
    }
    end -= 1;
    *darea = (DATAAREA *)realloc(*darea, sizeof(DATAAREA) * (numarea+1));
    (*darea+numarea)->start = start;
    (*darea+numarea)->end = end;
    numarea++;
    //printf("%08x - %08x\n", start, end);
  }
  free(sEntry);

  return numarea;
}

void CPAK::AppendEntries(FILELIST *ent)
{
  FILELIST *aent;
  bool isAppend = false;

  aent = (FILELIST *)bsearch(ent, Entries, NumEntry, sizeof(FILELIST), FListCompareFuncFN);
  if (aent == NULL) {
    Entries = (FILELIST *)realloc(Entries, sizeof(FILELIST) * (NumEntry+1));
    aent = Entries+NumEntry;
    NumEntry++;
    isAppend = true;
  }
        
  strncpy(aent->fn, ent->fn, ENTRY_FILENAME_LEN);
  aent->srclen = ent->srclen;
  aent->srclen_aligned = ent->srclen_aligned;
  aent->declen = ent->declen;
  aent->type = ent->type;
  aent->srcpos = ent->srcpos;
  qsort(Entries, NumEntry, sizeof(FILELIST), FListCompareFuncFN);
}

void CPAK::ReBuildFileList(BYTE *dat, int *len, int *clen)
{
  switch (pakversion[0] & 0x0000FF00) {
  case 0x0100:
    ReBuildFileListV1(dat, len);
    break;
  case 0x0200:
    ReBuildFileListV2(dat, len, clen);
    break;
  }
}

void CPAK::ReBuildFileListV1(BYTE *dat, int *len)
{
  int lenfn, lenfn_ali;
  char encfn[ENTRY_FILENAME_LEN];
  DWORD tmpd;
  int c=0;
  int rest;

  for (int i=0; i<NumEntry; i++) {
    lenfn = (int)strlen((Entries+i)->fn);
    lenfn_ali = lenfn+1 + (8 - ((lenfn+1) % 8));
    for (int j=0; j<ENTRY_FILENAME_LEN; j+=4) {
      *(DWORD *)&encfn[j] = 0;
    }
    strcpy(encfn, (Entries+i)->fn);
    rest = lenfn_ali%8;
    if (rest != 0) {
      rest = 8-rest;
    }
    EncodeFilename(encfn, lenfn_ali+rest);
    *(DWORD *)(dat+c) = lenfn_ali+rest+2+4;          c += 4;
    //*(WORD *)(dat+c) = 0;                     c += 2;
    *(WORD *)(dat+c) = 0x0029;                     c += 2;
    memcpy(dat+c, encfn, lenfn_ali+rest+4);          c += lenfn_ali+rest+4;
    tmpd = (Entries+i)->srclen + (Entries+i)->declen + 715;
    *(DWORD *)(dat+c) = tmpd;                 c += 4;
    tmpd = (Entries+i)->srclen_aligned + 37579;
    *(DWORD *)(dat+c) = tmpd;                 c += 4;
    *(DWORD *)(dat+c) = (Entries+i)->declen;  c += 4;
    *(dat+c) = (Entries+i)->type;             c++;
    *(DWORD *)(dat+c) = (Entries+i)->srcpos - 0x2E;  c += 4;
  }
  *len = c;
}

void CPAK::ReBuildFileListV2(BYTE *dat, int *len, int *clen)
{
  BYTE *odat;
  odat = (BYTE *)malloc(*len);
  int c = 0;

  for (int i=0; i<NumEntry; i++) {
    strcpy((char *)(odat+c), (Entries+i)->fn);
    c += (int)strlen((Entries+i)->fn) + 1;
    *(DWORD *)(odat+c) = (Entries+i)->srclen;          c += 4;
    *(DWORD *)(odat+c) = (Entries+i)->srclen_aligned;  c += 4;
    *(DWORD *)(odat+c) = (Entries+i)->declen;          c += 4;
    *(odat+c) = (Entries+i)->type;                     c++;
    *(DWORD *)(odat+c) = (Entries+i)->srcpos - 0x2E;           c += 4;
  }
  compress(dat, (uLongf *)clen, odat, c);
  *len = c;

  free(odat);
}

bool CPAK::MakePatchOpen(const char *filename, WORD version)
{
  if ((fpPatch = fopen(filename, "wb")) == NULL) {
    return false;
  } else {
    patchOpend = true;
    patchVer = version;
    filesPatch.clear();
    fprintf(fpPatch, "Master of Magic");
    for (BYTE i=0; i<0x0f; i++) {
      fwrite(&i, 1, 1, fpPatch);
    }
    DWORD tmp=0;
    fwrite(&tmp, 1, sizeof(tmp), fpPatch);  //entry
    fwrite(&tmp, 1, sizeof(tmp), fpPatch);  //num1
    fwrite(&tmp, 1, sizeof(tmp), fpPatch);  //num2
    tmp = version;
    fwrite(&tmp, 1, sizeof(tmp), fpPatch);  //version
    posPatchDat = 0;
    return true;
  }
}

void CPAK::MakePatchAdd(const char *filename, BYTE *dat, int len)
{
  switch (patchVer & 0xFF00) {
  case 0x100:
    MakePatchAddV1(filename, dat, len);
    break;
  case 0x200:
    MakePatchAddV2(filename, dat, len);
    break;
  }
}

void CPAK::MakePatchAddV1(const char *filename, BYTE *dat, int len)
{
  FILELIST tmpFL;
  BYTE *cdat;
  int clen, clen_ali;

  cdat = (BYTE *)malloc(len);
  CompressDataV1(filename, cdat, &clen_ali, &clen, dat, len, true);
  fwrite(cdat, 1, clen_ali, fpPatch);
  free(cdat);

  if (strlen(filename) > ENTRY_FILENAME_LEN) {
    printf("WARNING: Filename %s is long. Change ENTRY_FILENAME_LEN\n", filename);
  }
  strncpy(tmpFL.fn, filename, ENTRY_FILENAME_LEN);
  tmpFL.declen = len;
  tmpFL.srclen = clen;
  tmpFL.srclen_aligned = clen_ali;
  tmpFL.srcpos = posPatchDat;
  tmpFL.type = 1;
  filesPatch.push_back(tmpFL);
  posPatchDat += clen_ali;
}

void CPAK::MakePatchAddV2(const char *filename, BYTE *dat, int len)
{
  FILELIST tmpFL;
  BYTE *cdat;
  int clen;

  cdat = (BYTE *)malloc(len);
  compress(cdat, (uLongf *)&clen, dat, len);
  fwrite(cdat, 1, clen, fpPatch);
  free(cdat);

  if (strlen(filename) > ENTRY_FILENAME_LEN) {
    printf("WARNING: Filename %s is long. Change ENTRY_FILENAME_LEN\n", filename);
  }
  strncpy(tmpFL.fn, filename, ENTRY_FILENAME_LEN);
  tmpFL.declen = len;
  tmpFL.srclen = clen;
  tmpFL.srclen_aligned = clen;
  tmpFL.srcpos = posPatchDat;
  tmpFL.type = 1;
  filesPatch.push_back(tmpFL);
  posPatchDat += clen;
}

void CPAK::CompressDataV1(const char *filename, BYTE *dest, int *dlen_ali, int *dlen, BYTE *src, int srclen, bool patch)
{
  compress(dest, (uLongf *)dlen, src, srclen);

  int digit;
  if (GetSuffixType(filename) == 0) {
    digit = 0;
  } else {
    digit=1;
    for (int i=10; i <= *dlen; i*=10) {
      digit++;
    }
  }
 
  *dlen_ali = *dlen + (8 - (*dlen % 8));
  EncodeDES(dest, *dlen_ali, digit == 0, digit);
}

void CPAK::MakePatchClose(void)
{
  switch (patchVer & 0xFF00) {
  case 0x100:
    MakePatchCloseV1();
    break;
  case 0x200:
    MakePatchCloseV2();
    break;
  }
}

void CPAK::MakePatchCloseV1(void)
{
  if (patchOpend == false)  return;

  char encfn[ENTRY_FILENAME_LEN];
  int lenfn, lenfn_ali;
  DWORD tmpd;
  WORD tmpw;
  DWORD entlen = 0;
  int rest;

  for (int i=0; i<(int)filesPatch.size(); i++) {
    lenfn = (int)strlen(filesPatch[i].fn);
    lenfn_ali = lenfn+1 + (8 - ((lenfn+1) % 8));
    for (int j=0; j<ENTRY_FILENAME_LEN; j+=4) {
      *(DWORD *)&encfn[j] = 0;
    }
    strcpy(encfn, filesPatch[i].fn);
    rest = lenfn_ali%8;
    if (rest != 0) {
      rest = 8 - rest;
    }
    EncodeFilename(encfn, lenfn_ali + rest);
    tmpd = lenfn_ali+rest+4+2;
    fwrite(&tmpd, 1, sizeof(tmpd), fpPatch);
    tmpw = 0x29;
    fwrite(&tmpw, 1, sizeof(tmpw), fpPatch);
    fwrite(encfn, 1, lenfn_ali+rest+4, fpPatch);
    tmpd = filesPatch[i].srclen + filesPatch[i].declen + 715;
    fwrite(&tmpd, 1, sizeof(tmpd), fpPatch);
    tmpd = filesPatch[i].srclen_aligned + 37579;
    fwrite(&tmpd, 1, sizeof(tmpd), fpPatch);
    fwrite(&(filesPatch[i].declen), 1, sizeof(DWORD), fpPatch);
    fwrite(&(filesPatch[i].type), 1, sizeof(BYTE), fpPatch);
    fwrite(&(filesPatch[i].srcpos), 1, sizeof(DWORD), fpPatch);
    entlen += 4+2+lenfn_ali+4+4+4+1+4;
  }

  fseek(fpPatch, 0x1E, SEEK_SET);
  fwrite(&posPatchDat, 1, sizeof(entlen), fpPatch);
  DWORD tmp;
  tmp = (DWORD)filesPatch.size();
  tmp += 7;
  tmp *= 0x7C0;
  fwrite(&tmp, 1, sizeof(tmp), fpPatch);
  tmp += (DWORD)filesPatch.size() + 7;
  fwrite(&tmp, 1, sizeof(tmp), fpPatch);
  

  fclose(fpPatch);
  patchOpend = false;
}

void CPAK::MakePatchCloseV2(void)
{
  if (patchOpend == false) return;

  BYTE *ent, *cent;
  int c = 0;
  DWORD entlen = (DWORD)filesPatch.size() * sizeof(FILELIST);
  DWORD centlen = entlen;

  ent = (BYTE *)malloc(entlen);
  cent = (BYTE *)malloc(entlen);
  for (int i=0; i<(int)filesPatch.size(); i++) {
    strcpy((char *)(ent+c), filesPatch[i].fn);
    c += (int)strlen(filesPatch[i].fn)+1;
    *(DWORD *)(ent+c) = filesPatch[i].srclen;
    c += 4;
    *(DWORD *)(ent+c) = filesPatch[i].srclen_aligned;
    c += 4;
    *(DWORD *)(ent+c) = filesPatch[i].declen;
    c += 4;
    *(ent+c) = filesPatch[i].type;
    c++;
    *(DWORD *)(ent+c) = filesPatch[i].srcpos;
    c += 4;
  }
  compress(cent, &centlen, ent, c);
  fwrite(&centlen, 1, sizeof(centlen), fpPatch);
  fwrite(&c, 1, sizeof(entlen), fpPatch);
  fwrite(cent, 1, centlen, fpPatch);
  free(ent);
  free(cent);

  fseek(fpPatch, 0x1E, SEEK_SET);
  fwrite(&posPatchDat, 1, sizeof(posPatchDat), fpPatch);
  fseek(fpPatch, 4, SEEK_CUR);
  DWORD tmp;
  tmp = (DWORD)filesPatch.size() + 7;
  fwrite(&tmp, 1, sizeof(tmp), fpPatch);

  fclose(fpPatch);
  patchOpend = false;
}

bool CPAK::AppendtoGRF(const char *patchfname)
{
  CPAK ppak;

  if (AppendtoGRFOpen(patchfname, &ppak) == false)  return false;
  if (ppak.NumEntry < 1) return true;
  for (int i=0; i<ppak.NumEntry; i++) {
    if (((ppak.Entries)+i)->type == 2) continue;
    ppak.Uncomp2tmp( ((ppak.Entries)+i)->fn );
    AppendtoGRFAppend(((ppak.Entries)+i)->fn, ppak.tdata, ppak.tlen);
  }
  AppendtoGRFClose();

  return true;
}

bool CPAK::AppendtoGRFOpen(const char *patchfname, CPAK *ppak)
{
  if (NumPAK > 1) return false;

  if (ppak->AddPakFile(patchfname) == false) return false;

  fpAppend = fopen(pakfname[0].c_str(), "r+b");
  appendOpend = true;
  if ((pakversion[0] & 0x0000FF00) == 0x0100) {
    appendMovIdx = false;
  } else {
    appendMovIdx = true;
  }

  return true;
}

void CPAK::AppendtoGRFAppend(const char *filename, BYTE *dat, int len)
{
  BYTE *cdat;
  int clen = len, clen_ali;
  cdat = (BYTE *)malloc(len);
  switch ( pakversion[0] & 0x0000FF00 ) {
  case 0x0100:
    CompressDataV1(filename, cdat, &clen, &clen_ali, dat, len, false);
    break;
  case 0x0200:
    compress(cdat, (uLongf *)&clen, dat, len);
    clen_ali = clen;
    break;
  default:
    free(cdat);
    return;
  }

  DWORD writeoffs;
  writeoffs = FindEmptyArea(clen_ali);
  
  if (writeoffs == 0) { // add last
    DWORD idxoffs;
    if (appendMovIdx) {
      fseek(fpAppend, 0, SEEK_END);
      idxoffs = ftell(fpAppend);
    } else {
      fseek(fpAppend, 0x1E, SEEK_SET);
      fread(&idxoffs, 1, sizeof(idxoffs), fpAppend);
      appendMovIdx = true;
    }
    writeoffs = idxoffs;
  }

  fseek(fpAppend, writeoffs, SEEK_SET);
  fwrite(cdat, 1, clen_ali, fpAppend);

  FILELIST tmpFL;
  strncpy(tmpFL.fn, filename, ENTRY_FILENAME_LEN);
  tmpFL.srclen = clen;
  tmpFL.srclen_aligned = clen_ali;
  tmpFL.declen = len;
  tmpFL.srcpos = writeoffs;
  if ((pakversion[0] & 0x0000FF00) == 0x100) {
    if (GetSuffixType(filename) == 0) {
      tmpFL.type = 5;
    } else {
      tmpFL.type = 3;
    }
  } else {
    tmpFL.type = 1;
  }

  AppendEntries(&tmpFL);
}

void CPAK::AppendtoGRFClose(void)
{
  if (appendOpend == false) return;

  BYTE *fent;
  int fentlen, fentclen;

  fentlen = fentclen = NumEntry * sizeof(FILELIST);
  fent = (BYTE *)malloc(fentlen);
  ReBuildFileList(fent, &fentlen, &fentclen);
  switch (pakversion[0] & 0x0000FF00) {
  case 0x0100:
    AppendtoGRFCloseV1(fent, fentlen);
    break;
  case 0x0200:
    AppendtoGRFCloseV2(fent, fentlen, fentclen);
    break;
  }
  free(fent);

  fclose(fpAppend);
  appendOpend = false;
}

void CPAK::AppendtoGRFCloseV1(BYTE *fent, int fentlen)
{
  if (appendMovIdx) {
    DWORD idxoffs;
    fseek(fpAppend, 0, SEEK_END);
    idxoffs = ftell(fpAppend) - 0x2E;
    fseek(fpAppend, 0x1E, SEEK_SET);
    fwrite(&idxoffs, 1, sizeof(idxoffs), fpAppend);
    fseek(fpAppend, 0, SEEK_END);
    fwrite(fent, 1, fentlen, fpAppend);
  } else {
    int idx;
    fseek(fpAppend, 0x1E, SEEK_SET);
    fread(&idx, 1, sizeof(idx), fpAppend);
    int grfsize;
    fseek(fpAppend, 0, SEEK_END);
    grfsize = ftell(fpAppend);
    if (fentlen < grfsize-idx) {
      idx = grfsize-fentlen - 0x2E;
      fseek(fpAppend, 0x1E, SEEK_SET);
      fwrite(&idx, 1, sizeof(idx), fpAppend);
    }
    fseek(fpAppend, idx, SEEK_SET);
    fwrite(fent, 1, fentlen, fpAppend);
  }

  DWORD num1, num2;
  fseek(fpAppend, 0x22, SEEK_SET);
  num1 = (NumEntry+7) * 0x7C0;
  fwrite(&num1, 1, sizeof(num1), fpAppend);
  num2 = NumEntry + num1 + 7;
  fwrite(&num2, 1, sizeof(num2), fpAppend);
}

void CPAK::AppendtoGRFCloseV2(BYTE *fent, int fentlen, int fentclen)
{
  DWORD writeoffs;
  writeoffs = FindEmptyArea(fentclen+8);

  if (writeoffs == 0) { // add last
    DWORD idxoffs;
    fseek(fpAppend, 0, SEEK_END);
    idxoffs = ftell(fpAppend);
    writeoffs = idxoffs;
  }
  writeoffs -= 0x2E;

  fseek(fpAppend, 0x1E, SEEK_SET);
  fwrite(&writeoffs, 1, sizeof(DWORD), fpAppend);

  writeoffs += 0x2E;
  fseek(fpAppend, writeoffs, SEEK_SET);
  fwrite(&fentclen, 1, sizeof(fentclen), fpAppend);
  fwrite(&fentlen, 1, sizeof(fentlen), fpAppend);
  fwrite(fent, 1, fentclen, fpAppend);

  DWORD num1, num2;
  fseek(fpAppend, 0x22, SEEK_SET);
  fread(&num1, 1, sizeof(num1), fpAppend);
  num2 = NumEntry + num1 + 7;
  fseek(fpAppend, 0, SEEK_CUR);
  fwrite(&num2, 1, sizeof(num2), fpAppend);
}

void CPAK::SubtractGrf(char *fname, CPAK *pak, int level)
{
  int lenA, lenB;
  for (int i=NumEntry-1; i>=0; i--) {
    if (Entries[i].type == 2)  continue;  //directory
    pak->Uncomp2tmp(Entries[i].fn);
    if (pak->tlen != 0) {
      if (level == 2) {
        Uncomp2tmp( Entries[i].fn );
        lenA = tlen;
        lenB = pak->tlen;
        if (CompareData(tdata, pak->tdata, lenA, lenB) == false)  continue;
      }
      printf("%s is deleted.\n", Entries[i].fn);
      SwapEntry(i, NumEntry-1);
      NumEntry--;
    }
  }
  appendMovIdx = true;
  if ((fpAppend = fopen(fname, "r+b")) == NULL) return;
  appendOpend = true;
  AppendtoGRFClose();
}

bool CPAK::CompareData(BYTE *datA, BYTE *datB, int lenA, int lenB)
{
  bool flag = true;
  if (lenA != lenB)  return false;
  for (int k=0; k<lenA/4; k++) {
    if (*(DWORD *)(datA+k*4) != *(DWORD *)(datB+k*4)) {
      flag = false;
      break;
    }
  }
  if (flag == false)  return false;
  for (int k=0; k<lenA%4; k++) {
    if (datA[(lenA/4)*4+k] != datB[(lenA/4)*4+k]) {
      flag = false;
      break;
    }
  }
  return flag;
}

void CPAK::SwapEntry(int a, int b)
{
  FILELIST tmp;

  CopyEntry(&tmp, Entries+a);
  CopyEntry(Entries+a, Entries+b);
  CopyEntry(Entries+b, &tmp);  
}

void CPAK::CopyEntry(FILELIST *a, FILELIST *b)
{
  strcpy(a->fn, b->fn);
  a->srclen = b->srclen;
  a->srclen_aligned = b->srclen_aligned;
  a->declen = b->declen;
  a->type = b->type;
  a->srcpos = b->srcpos;
  a->pakidx = b->pakidx;
}
#endif // VIOLATION_CODE

void CPAK::DecodeLZSS(BYTE *ddat, int declen, BYTE *cdat, int clen)
{
  BYTE *endp = cdat + clen;
  BYTE *enddp = ddat + declen;
  BYTE ctrl;
  DWORD dist;
  BYTE len;
  while (cdat < endp && ddat < enddp) {
    ctrl = *cdat;  cdat++;
    for (int i=0; i<8; i++) {
      if (ctrl & 1) {
        dist = *cdat;  cdat++;
        len = *cdat;  cdat++;
        dist += (len & 0x0F) << 8;
        len >>= 4;
        len += 2;
        if (ddat+len > enddp) {
          len -= (BYTE)((ddat+len) - enddp + 1);
        }
        for (int j=0; j<len; j++) {
          *(ddat+j) = *(ddat-dist+j);
        }
        ddat += len;
        if (ddat >= enddp) return;
      } else {
        *ddat = *cdat;
        ddat++;
        cdat++;
        if (ddat >= enddp) return;
      }
      ctrl >>= 1;
    }
  }
}

int FListCompareFuncFN(const void *a, const void *b)
{
  return strcmp(((FILELIST *)a)->fn, ((FILELIST *)b)->fn);
}

int FListCompareFuncSP(const void *a, const void *b)
{
  DWORD sa, sb;

  sa = ((FILELIST *)a)->srcpos;
  sb = ((FILELIST *)b)->srcpos;

  if (sa < sb) {
    return -1;
  } else if (sa > sb) {
    return 1;
  } else {
    return 0;
  }
}

