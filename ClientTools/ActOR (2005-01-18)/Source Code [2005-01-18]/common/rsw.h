#include <stdio.h>

#include "global.h"

#define MAX_RSW_NODE 5460

typedef struct {
  char magic[4];
  WORD version;
  char ini_filename[40];
  char gnd_filename[40];
  char gat_filename[40];
  char scr_filename[40];
  DWORD xxx1[5];
  DWORD xxx2[3];
  BYTE xxxb[12][2];
  DWORD xxx3[5];
} RswHeader;

typedef struct {
  char objname[40];
  DWORD xxx1;
  float xxxf;
  DWORD xxx2;
  BYTE xxx[0xB8];
  float mul_xyz[3];
} RswType1;

typedef struct {
  BYTE xxx[0x6C];
} RswType2;

typedef struct {
  BYTE xxx[0xC0];
} RswType3;

typedef struct {
  BYTE xxx[0x74];
} RswType4;

typedef struct {
  DWORD type;
  union {
    RswType1 *rt1;
    RswType2 *rt2;
    RswType3 *rt3;
    RswType4 *rt4;
  };
} RswObj;

typedef struct {
  float xyz[3];
} RswNode;


class CRSW
{
private:
  RswHeader header;
  int numofobj;
  RswObj *objs;
  RswNode *nodes;

  bool ParseData(BYTE *dat, int len);
  void WriteAsText_Header(FILE *fp);

public:
  CRSW(BYTE *dat, int len);
  virtual ~CRSW(void);

  bool parseOK;

  void WriteAsText(char *fn);
  void WriteDelType1(char *fn);

  void WriteNumOfType(void);
};

