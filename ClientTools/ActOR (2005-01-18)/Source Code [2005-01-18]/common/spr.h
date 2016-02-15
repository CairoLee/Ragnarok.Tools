#ifndef __MY_SPR__
#define __MY_SPR__

#include "global.h"

#ifndef WIN32
typedef struct tagBITMAPFILEHEADER { 
  WORD    bfType; 
  DWORD   bfSize; 
  WORD    bfReserved1; 
  WORD    bfReserved2; 
  DWORD   bfOffBits; 
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BITMAPINFOHEADER;
#endif

typedef struct {
  int w, h;
  DWORD *dat;
  BYTE *paldat;
} AnySprite;

class CSPR
{
private:
  bool ParseSprData(BYTE *dat, int filelength);
  void Set1ImageCompressed(BYTE *dat, int no);
  void Set1ImgaePalFlat(BYTE *dat, int no);
  void Set1ImageFlat(BYTE *dat, WORD width, WORD height, int no);

  void WriteBmp256(int no, char *fn);
  void WriteTga32(int no, char *fn);

  void Compress0RunLen(BYTE *cdat, int *clen, int no);
  void WriteSprReorderRGBA(BYTE *cdat, int no);

  void FindUnusedPal(bool *u);

  void SwapImage(int idA, int idB);
  void CopyImage(AnySprite *to, AnySprite *from);

public:
  CSPR(BYTE *dat, int len);
  virtual ~CSPR(void);

  int numOfImg;
  int numPalImg;
  int numFlatImg;
  AnySprite *imgs;
  DWORD *pal;
  int GetNumPalImage(void);
  int GetNumFlatImage(void);
  int GetNumImage(void);
  int GetImageWidth(int id);
  int GetImageHeight(int id);
  DWORD *GetImage(int id);
  void GetMaxWidthHeight(int *w, int *h);
  bool AddImage(int w, int h, DWORD *odat, BYTE *pdat, bool isPal, bool countUp);
  void WriteBmp256Tga32(char *basefn, int no, bool onlyone);
  void WriteSpr(char *fname);
  void ChangeImg(int no, int w, int h, DWORD *ndat, BYTE *pdat, bool isPal);

  void SetSameColorUnusedPal(DWORD BBGGRR);

  void InsertImage(int id, int w, int h, DWORD *odat, BYTE *pdat, bool isPal);
  void DelImage(int id);
};

#endif // __MY_SPR__
