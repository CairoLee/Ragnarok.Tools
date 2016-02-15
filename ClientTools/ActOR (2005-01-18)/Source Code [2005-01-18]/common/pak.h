#ifndef __MY_GR_H__
#define __MY_GR_H__

#pragma pack (1)
#pragma warning (disable:4103)  // for pragma pack 1

#include <string>
#include <vector>
using namespace std;

#ifdef WIN32
#define USE_ZLIB
//if NOT define USE_ZLIB, use cps.dll
#else
#define USE_ZLIB
#endif

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

#include "global.h"

//#define DOUBLECHECK

#define GETLONG(c) (*(DWORD *)(c))

#define ENTRY_FILENAME_LEN 112

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#ifdef WIN32
typedef int (__cdecl *LPFN_UNCOMPRESS)(BYTE *dest, DWORD *destLen, const BYTE *source, DWORD sourceLen);
#endif

typedef struct {
  char fn[ENTRY_FILENAME_LEN];  // file name
  DWORD srclen;             // compressed size
  DWORD srclen_aligned;     // for DES
  DWORD declen;             // decode size
  BYTE type;                // 0: dir_v1, 1: files_v1, only uncomp_v2, 2: dir_v2, 3: DESc+uncomp, 5: DES0+uncomp
  DWORD srcpos;             // file position
  BYTE pakidx;              // grf index
} FILELIST;

typedef struct {
  DWORD start;
  DWORD end;
} DATAAREA;

class CPAK
{
 private:
  vector<string> pakfname;

#ifndef USE_ZLIB
  HMODULE hLib;
  LPFN_UNCOMPRESS fpuncompress;
#endif

  bool patchOpend;
  WORD patchVer;
  FILE *fpPatch;
  DWORD posPatchDat;
  vector<FILELIST> filesPatch;

  bool appendOpend;
  FILE *fpAppend;
  DWORD posAppendDat;
  bool appendMovIdx;

  bool PakFileOpen(FILE **fp, const char *filename, bool pak);
  bool EntriesAlloc(FILE *fp, bool pak);
  void EntryRead(FILE *fp, bool pak);
  void EntryReadPak(FILE *fp, bool grf);
  void EntryReadV1(FILE *fp);
  void EntryReadV2(FILE *fp);
  void ParseIndexPak(BYTE *dat, int datlen, bool grf);
  void ParseIndexV1(BYTE *dat, int datlen);
  void ParseIndexV2(BYTE *dat, int datlen);
  void EntryAdd(FILELIST *flist);
  int GetExistEntry(char *fn);  // NOT exist return -1 else return index
  void EntryCopy(FILELIST *to, const FILELIST *from);
  char *DecodeFilenameB1(char *str, int len);
  char *DecodeFilename(char *estr, int len);
  void DecodeDES(BYTE *dat, int len, bool type, BYTE cycle);
  void NibbleSwap(BYTE *src, int len);
  void BitConvert(BYTE *src, char *BitSwapTable);
  void BitConvert4(BYTE *src);
#ifdef VIOLATION_CODE
  char *EncodeFilename(char *dstr, int len);
  void EncodeDES(BYTE *dat, int len, bool type, BYTE cycle);
#endif // VIOLATION_CODE
  void JumpIndexFP(FILE *fp);
  DWORD GetGRFVersion(FILE *fp);
  BYTE GetDESCycle(FILELIST *ent);
  BYTE GetSuffixType(const char *filename);

#ifdef VIOLATION_CODE
  void MakePatchAddV1(const char *filename, BYTE *dat, int len);
  void MakePatchAddV2(const char *filename, BYTE *dat, int len);
  void CompressDataV1(const char *filename, BYTE *dest, int *dlen_ali, int *dlen, BYTE *src, int srclen, bool patch);
  void MakePatchCloseV1(void);
  void MakePatchCloseV2(void);

  int GetDataArea(DATAAREA **darea);
  DWORD FindEmptyArea(DWORD size);
  void AppendEntries(FILELIST *ent);
  void ReBuildFileList(BYTE *dat, int *len, int *clen);
  void ReBuildFileListV1(BYTE *dat, int *len);
  void ReBuildFileListV2(BYTE *dat, int *len, int *clen);
  void AppendtoGRFCloseV1(BYTE *fent, int fentlen);
  void AppendtoGRFCloseV2(BYTE *fent, int fentlen, int fentclen);

  bool CompareData(BYTE *datA, BYTE *datB, int lenA, int lenB);
  void SwapEntry(int a, int b);
  void CopyEntry(FILELIST *a, FILELIST *b);
#endif // VIOLATION_CODE

  void DecodeLZSS(BYTE *ddat, int declen, BYTE *cdat, int clen);

 public:
  CPAK(void);
  virtual ~CPAK(void);
  
  int NumEntry;
  int NumPAK;
  vector<DWORD> pakversion;
  FILELIST *Entries;
  BYTE *tdata;
  DWORD tlen;

  FILELIST *SearchEntry(const char *filename);

  bool AddPakFile(const char *filename);
  bool Uncomp2tmp(const char *filename);

#ifdef VIOLATION_CODE
  bool MakePatchOpen(const char *filename, WORD version);
  void MakePatchAdd(const char *filename, BYTE *dat, int len);
  void MakePatchClose(void);

  bool AppendtoGRF(const char *patchfname);
  bool AppendtoGRFOpen(const char *patchfname, CPAK *ppak);
  void AppendtoGRFAppend(const char *filename, BYTE *dat, int len);
  void AppendtoGRFClose(void);

  void SubtractGrf(char *fname, CPAK *pak, int level);
#endif // VIOLATION_CODE
};

extern "C" {
  int FListCompareFuncFN(const void *a, const void *b);
  int FListCompareFuncSP(const void *a, const void *b);
}

static unsigned char BitMaskTable[8] = {
  0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static char     BitSwapTable1[64] = {
  58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
};

static char     BitSwapTable2[64] = {
  40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
};

static char     BitSwapTable3[32] = {
  16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
  2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};

static unsigned char NibbleData[4][64]={
  {
    0xef, 0x03, 0x41, 0xfd, 0xd8, 0x74, 0x1e, 0x47,  0x26, 0xef, 0xfb, 0x22, 0xb3, 0xd8, 0x84, 0x1e,
    0x39, 0xac, 0xa7, 0x60, 0x62, 0xc1, 0xcd, 0xba,  0x5c, 0x96, 0x90, 0x59, 0x05, 0x3b, 0x7a, 0x85,
    0x40, 0xfd, 0x1e, 0xc8, 0xe7, 0x8a, 0x8b, 0x21,  0xda, 0x43, 0x64, 0x9f, 0x2d, 0x14, 0xb1, 0x72,
    0xf5, 0x5b, 0xc8, 0xb6, 0x9c, 0x37, 0x76, 0xec,  0x39, 0xa0, 0xa3, 0x05, 0x52, 0x6e, 0x0f, 0xd9,
  }, {
    0xa7, 0xdd, 0x0d, 0x78, 0x9e, 0x0b, 0xe3, 0x95,  0x60, 0x36, 0x36, 0x4f, 0xf9, 0x60, 0x5a, 0xa3,
    0x11, 0x24, 0xd2, 0x87, 0xc8, 0x52, 0x75, 0xec,  0xbb, 0xc1, 0x4c, 0xba, 0x24, 0xfe, 0x8f, 0x19,
    0xda, 0x13, 0x66, 0xaf, 0x49, 0xd0, 0x90, 0x06,  0x8c, 0x6a, 0xfb, 0x91, 0x37, 0x8d, 0x0d, 0x78,
    0xbf, 0x49, 0x11, 0xf4, 0x23, 0xe5, 0xce, 0x3b,  0x55, 0xbc, 0xa2, 0x57, 0xe8, 0x22, 0x74, 0xce,
  }, {
    0x2c, 0xea, 0xc1, 0xbf, 0x4a, 0x24, 0x1f, 0xc2,  0x79, 0x47, 0xa2, 0x7c, 0xb6, 0xd9, 0x68, 0x15,
    0x80, 0x56, 0x5d, 0x01, 0x33, 0xfd, 0xf4, 0xae,  0xde, 0x30, 0x07, 0x9b, 0xe5, 0x83, 0x9b, 0x68,
    0x49, 0xb4, 0x2e, 0x83, 0x1f, 0xc2, 0xb5, 0x7c,  0xa2, 0x19, 0xd8, 0xe5, 0x7c, 0x2f, 0x83, 0xda,
    0xf7, 0x6b, 0x90, 0xfe, 0xc4, 0x01, 0x5a, 0x97,  0x61, 0xa6, 0x3d, 0x40, 0x0b, 0x58, 0xe6, 0x3d,
  }, {
    0x4d, 0xd1, 0xb2, 0x0f, 0x28, 0xbd, 0xe4, 0x78,  0xf6, 0x4a, 0x0f, 0x93, 0x8b, 0x17, 0xd1, 0xa4,
    0x3a, 0xec, 0xc9, 0x35, 0x93, 0x56, 0x7e, 0xcb,  0x55, 0x20, 0xa0, 0xfe, 0x6c, 0x89, 0x17, 0x62,
    0x17, 0x62, 0x4b, 0xb1, 0xb4, 0xde, 0xd1, 0x87,  0xc9, 0x14, 0x3c, 0x4a, 0x7e, 0xa8, 0xe2, 0x7d,
    0xa0, 0x9f, 0xf6, 0x5c, 0x6a, 0x09, 0x8d, 0xf0,  0x0f, 0xe3, 0x53, 0x25, 0x95, 0x36, 0x28, 0xcb,
  }
};

#endif // __MY_GR_H__
