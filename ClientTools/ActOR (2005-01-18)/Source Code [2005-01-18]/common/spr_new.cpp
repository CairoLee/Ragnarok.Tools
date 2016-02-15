#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "spr.h"

CSPR::CSPR(BYTE *dat, int len)
{
  numOfImg = numPalImg= numFlatImg = 0;
  imgs = NULL;
  pal = (DWORD *)malloc(sizeof(DWORD)*256);
  if (len != 0) {
    ParseSprData(dat, len);
  }
}

CSPR::~CSPR(void)
{
  free(pal);
  if (imgs) {
    for (int i=0; i<numOfImg; i++) {
      free( (imgs+i)->dat );
      if (i==0) {
        free( (imgs+i)->paldat );
      }
    }
  }

  free(imgs);
}

bool CSPR::ParseSprData(BYTE *dat, int filelength)
{
  //magic check
  if (*(WORD *)dat != 0x5053) return false;
  dat += 2;

  //spr version
  WORD sprversion = *(WORD *)dat;
  dat += 2;

  //num of image
  numPalImg = *(WORD *)dat;
  dat += 2;
  if (sprversion == 0x0101) {
    numFlatImg = 0;
  } else {
    numFlatImg = *(WORD *)dat;
    dat += 2;
  }
  numOfImg = numPalImg + numFlatImg;

  try {
    imgs = (AnySprite *)malloc(sizeof(AnySprite) * numOfImg);
  } catch (...) {
    return false;
  }
  if (sprversion == 0x0101) {
    memcpy(pal, dat-6+filelength-sizeof(DWORD)*256, sizeof(DWORD)*256);  // lack numFlatImg
  } else {
    memcpy(pal, dat-8+filelength-sizeof(DWORD)*256, sizeof(DWORD)*256);
  }

  for (int i=0; i<numPalImg; i++) {
    if (sprversion == 0x0201) {
      Set1ImageCompressed(dat, i);
      dat += 6 + *(WORD *)(dat+4);  // 6 = word * 3;  width, height, length
    } else {
      Set1ImgaePalFlat(dat, i);
      dat += 4 + *(WORD *)dat * *(WORD *)(dat+2);  // 4 = word * 2;  width, height
    }
  }

  WORD flatimgwidth, flatimgheight;
  for (int i=0; i<numFlatImg; i++) {
    flatimgwidth = *(WORD *)dat;
    dat += 2;
    flatimgheight = *(WORD *)dat;
    dat += 2;
    Set1ImageFlat(dat, flatimgwidth, flatimgheight, numPalImg+i);
    dat += flatimgwidth * flatimgheight * sizeof(DWORD);
  }

  return true;
}

void CSPR::Set1ImageCompressed(BYTE *dat, int no)
{
  WORD width, height;

  width = *(WORD *)dat;
  dat += 2;
  height = *(WORD *)dat;
  dat += 2;
  (imgs+no)->w = width;
  (imgs+no)->h = height;

  try {
    (imgs+no)->dat = (DWORD *)malloc(sizeof(DWORD)*width*height);
    if (no == 0) {
      (imgs+no)->paldat = (BYTE *)malloc(sizeof(BYTE)*width*height);
    } else {
      (imgs+no)->paldat = imgs->paldat;
    }
  } catch (...) {
    return;
  }

  dat += 2; // compressed length

  bool zero = false;
  int restzero = 0;
  int p;
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      p = y*width+x;
      if (zero) {
        *((imgs+no)->dat + p) = 0;
        if (no == 0) {
          *((imgs+no)->paldat +p) = 0;
        }
        restzero--;
        if (restzero == 0) {
          zero = false;
        }
      } else {
        if (*dat == 0) {
          dat++;
          zero = true;
          restzero = *dat;
          dat++;
          *((imgs+no)->dat + p) = 00;
          if (no == 0) {
            *((imgs+no)->paldat + p) = 0;
          }
          restzero--;
          if (!restzero) zero = false;
        } else {
          *((imgs+no)->dat + p) = *(pal + *dat) | 0xFF000000;
          if (no == 0) {
            *((imgs+no)->paldat + p) = *dat;
          }
          dat++;
        }
      }
    }
  }
}

void CSPR::Set1ImgaePalFlat(BYTE *dat, int no)
{
  WORD width, height;

  width = *(WORD *)dat;
  dat += 2;
  height = *(WORD *)dat;
  dat += 2;
  (imgs+no)->w = width;
  (imgs+no)->h = height;

  try {
    (imgs+no)->dat = (DWORD *)malloc(sizeof(DWORD)*width*height);
    if (no == 0) {
      (imgs+no)->paldat = (BYTE *)malloc(sizeof(BYTE)*width*height);
    } else {
      (imgs+no)->paldat = imgs->paldat;
    }
  } catch (...) {
    return;
  }

  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      if (*dat == 0) {
        *((imgs+no)->dat + y*width+x) = 0;
        if (no == 0) {
          *((imgs+no)->paldat + y*width+x) = 0;
        }
      } else {
        *((imgs+no)->dat + y*width+x) = *(pal + *dat) | 0xFF000000;
        if (no == 0) {
          *((imgs+no)->paldat + y*width+x) = *dat;
        }
      }
      dat++;
    }
  }
}

void CSPR::Set1ImageFlat(BYTE *dat, WORD width, WORD height, int no)
{
  (imgs+no)->w = width;
  (imgs+no)->h = height;

  try {
    (imgs+no)->dat = (DWORD *)malloc(sizeof(DWORD)*width*height);
    (imgs+no)->paldat = NULL;
  } catch (...) {
    return;
  }

  BYTE a, r, g, b;
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      a = *dat;
      b = *(dat+1);
      g = *(dat+2);
      r = *(dat+3);
      *((imgs+no)->dat + (height-y-1)*width+x) = a << 24 | b << 16 | g << 8 | r;
      dat += 4;
    }
  }
}

void CSPR::GetMaxWidthHeight(int *w, int *h)
{
  if (numOfImg == 0) {
    *w = *h = 0;
    return;
  }

  int maxw, maxh;
  maxw = maxh = 0;
  for (int i=0; i<numPalImg; i++) {
    if (maxw < (imgs+i)->w)  maxw = (imgs+i)->w;
    if (maxh < (imgs+i)->h)  maxh = (imgs+i)->h;
  }
  for (int i=numPalImg; i < numPalImg+numFlatImg; i++) {
    if (maxw < (imgs+i)->w)  maxw = (imgs+i)->w;
    if (maxh < (imgs+i)->h)  maxh = (imgs+i)->h;
  }
  *w = maxw;
  *h = maxh;
}

bool CSPR::AddImage(int w, int h, DWORD *odat, BYTE *pdat, bool isPal, bool countUp)
{
  imgs = (AnySprite *)realloc(imgs, sizeof(AnySprite)*(numOfImg+1));
  (imgs+numOfImg)->w = w;
  (imgs+numOfImg)->h = h;
  (imgs+numOfImg)->dat = (DWORD *)malloc(sizeof(DWORD)*w*h);
  if (isPal) {
    (imgs+numOfImg)->paldat = imgs->paldat; //(BYTE *)malloc(sizeof(BYTE)*w*h);
  } else {
    (imgs+numOfImg)->paldat = NULL;
  }
  if ((imgs+numOfImg)->dat == NULL) {
    return false;
  } else {
    memcpy((imgs+numOfImg)->dat, odat, sizeof(DWORD)*w*h);
  }
  /*
  if (isPal) {
    memcpy((imgs+numOfImg)->paldat, pdat, sizeof(BYTE)*w*h);
  }
  */

  if (countUp) {
    if (isPal) {
      numPalImg++;
    } else {
      numFlatImg++;
    }
    numOfImg++;
  }
  return true;
}

void CSPR::WriteBmp256Tga32(char *basefn, int no, bool onlyone)
{
  if (onlyone) {
    if (no < numPalImg) {
      WriteBmp256(no, basefn);
    } else { //if (no-numPalImg+1 < numFlatImg) {
      WriteTga32(no, basefn);
    }
    return;
  }
  char buf[FILENAME_MAX];
  for (int i=0; i<numPalImg; i++) {
    sprintf(buf, "%s%03d.bmp", basefn, i);
    WriteBmp256(i, buf);
  }
  for (int i=numPalImg; i<numFlatImg+numPalImg; i++) {
    sprintf(buf, "%s%03d.tga", basefn, i);
    WriteTga32(i, buf);
  }
}

void CSPR::WriteBmp256(int no, char *fn)
{
  int w = imgs[no].w;
  int quot = w%4;
  if (quot != 0) {
    w += 4-quot;
  }
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;
  memset(&bfh, 0, sizeof(bfh));
  memset(&bih, 0, sizeof(bih));
  bfh.bfType = 'M' << 8 | 'B';
  bfh.bfOffBits = sizeof(bfh) + sizeof(bih) + 4*256;
  bfh.bfSize = bfh.bfOffBits + w * imgs[no].h;
  bih.biSize = sizeof(bih);
  bih.biWidth = imgs[no].w;
  bih.biHeight = imgs[no].h;
  bih.biPlanes = 1;
  bih.biBitCount = 8;
  bih.biXPelsPerMeter = 2834;
  bih.biYPelsPerMeter = 2834;

  FILE *fp;
  fp = fopen(fn, "wb");
  if (fp == NULL) return;
  fwrite(&bfh, 1, sizeof(bfh), fp);
  fwrite(&bih, 1, sizeof(bih), fp);

  DWORD lpal[256];
  BYTE r, g, b;
  for (int i=0; i<256; i++) {
    r = (BYTE)(pal[i] & 0xFF);
    g = (BYTE)((pal[i] >> 8) & 0xFF);
    b = (BYTE)((pal[i] >> 16) & 0xFF);
    lpal[i] = r << 16 | g << 8 | b;
  }
  fwrite(lpal, sizeof(DWORD), 256, fp);

  BYTE *palimage;
  palimage = (BYTE *)malloc(w * imgs[no].h);
  if (palimage == NULL) {
    fclose(fp);
    return;
  }
  BYTE *p;
  BYTE c;
  for (int y = 0; y < imgs[no].h; y++) {
    for (int x = 0; x < imgs[no].w; x++) {
      c = *(imgs[no].paldat + y*imgs[no].w + x);
      p = palimage + (imgs[no].h - y - 1)*w + x;
      *p = c;
    }
  }

  fwrite(palimage, 1, w * imgs[no].h, fp);
  fclose(fp);
  free(palimage);
}

void CSPR::WriteTga32(int no, char *fn)
{
  BYTE header[0x12];
  
  for (int i=0; i<sizeof(header); i++) {
    header[i] = 0;
  }
  header[2] = 2;
  int w, h;
  w = imgs[no].w;
  h = imgs[no].h;
  *(WORD *)&header[0xC] = w;
  *(WORD *)&header[0xE] = h;
  header[0x10] = 32;
  header[0x11] = 0x08;

  BYTE r, g, b, a;
  DWORD *wdat;
  wdat = (DWORD *)malloc(sizeof(DWORD)*w*h);
  if (wdat == NULL) return;
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      r = *((BYTE *)(imgs[no].dat + y*w + x) + 0);
      g = *((BYTE *)(imgs[no].dat + y*w + x) + 1);
      b = *((BYTE *)(imgs[no].dat + y*w + x) + 2);
      a = *((BYTE *)(imgs[no].dat + y*w + x) + 3);
      *(wdat+(h-y-1)*w+x) = a << 24 | r << 16 | g << 8 | b;
    }
  }

  FILE *fp;
  fp = fopen(fn, "wb");
  if (fp == NULL) {
    free(wdat);
    return;
  }
  fwrite(header, 1, sizeof(header), fp);
  fwrite(wdat, sizeof(DWORD), w*h, fp);
  fclose(fp);
  free(wdat);
}

void CSPR::WriteSpr(char *fname)
{
  FILE *fp;
  if ((fp = fopen(fname, "wb")) == NULL)  return;

  DWORD magic_v = 0x02015053;
  fwrite(&magic_v, sizeof(DWORD), 1, fp);
  fwrite((WORD *)&numPalImg, sizeof(WORD), 1, fp);
  fwrite((WORD *)&numFlatImg, sizeof(WORD), 1, fp);

  BYTE *cdat;
  int clen;
  for (int i=0; i<numPalImg; i++) {
    cdat = (BYTE *)malloc(imgs[i].w * imgs[i].h);
    clen = 0;
    Compress0RunLen(cdat, &clen, i);
    fwrite((WORD *)&imgs[i].w, sizeof(WORD), 1, fp);
    fwrite((WORD *)&imgs[i].h, sizeof(WORD), 1, fp);
    fwrite((WORD *)&clen, sizeof(WORD), 1, fp);
    fwrite(cdat, 1, clen, fp);
    free(cdat);
  }
  for (int i=numPalImg; i<numFlatImg+numPalImg; i++) {
    cdat = (BYTE *)malloc(sizeof(DWORD) * imgs[i].w * imgs[i].h);
    WriteSprReorderRGBA(cdat, i);
    fwrite((WORD *)&imgs[i].w, sizeof(WORD), 1, fp);
    fwrite((WORD *)&imgs[i].h, sizeof(WORD), 1, fp);
    fwrite(cdat, 1, sizeof(DWORD) * imgs[i].w * imgs[i].h, fp);
    free(cdat);
  }

  DWORD lpal[256];
  memcpy(lpal, pal, sizeof(DWORD)*256);
  for (int i=0; i<256; i++) {
    lpal[i] &= 0x00FFFFFF;
  }
  fwrite(lpal, sizeof(DWORD), 256, fp);

  fclose(fp);
}

void CSPR::Compress0RunLen(BYTE *cdat, int *clen, int no)
{
  int w, h;
  w = imgs[no].w;
  h = imgs[no].h;

  int zero = 0;
  bool zeroflag = false;
  BYTE *pix = imgs[no].paldat;
  BYTE pl;
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      pl = *(pix + y*w + x);
      if (pl == 0) {
        if (zeroflag) {
          zero++;
          if (zero == 0x100) {
            *(cdat++) = 0xFF;
            (*clen)++;
            zero = 1;
            *(cdat++) = 0;
            (*clen)++;
          }
        } else {
          zeroflag = true;
          zero = 1;
          *(cdat++) = 0;
          (*clen)++;
        }
      } else {
        if (zeroflag) {
          zeroflag = false;
          *(cdat++) = zero;
          (*clen)++;
          goto Compress0RunLen_pal;  // after 2 line
        } else {
Compress0RunLen_pal:
          *(cdat++) = pl;
          (*clen)++;
        }
      }
    }
  }
  if (zeroflag) {
    *(cdat++) = zero;
    (*clen)++;
  }
}

void CSPR::WriteSprReorderRGBA(BYTE *cdat, int no)
{
  BYTE r, g, b, a;
  for (int y = 0; y<imgs[no].h; y++) {
    for (int x = 0; x<imgs[no].w; x++) {
      r = *((BYTE *)(imgs[no].dat + y*imgs[no].w + x) + 0);
      g = *((BYTE *)(imgs[no].dat + y*imgs[no].w + x) + 1);
      b = *((BYTE *)(imgs[no].dat + y*imgs[no].w + x) + 2);
      a = *((BYTE *)(imgs[no].dat + y*imgs[no].w + x) + 3);
      *(DWORD *)(cdat + (imgs[no].h-y-1)*imgs[no].w*4 + x*4) = r << 24 | g << 16 | b << 8 | a;
    }
  }
}

void CSPR::ChangeImg(int no, int w, int h, DWORD *ndat, BYTE *pdat, bool isPal)
{
  free(imgs[no].dat);
  free(imgs[no].paldat);


  imgs[no].dat = (DWORD *)malloc(sizeof(DWORD)*w*h);
  memcpy(imgs[no].dat, ndat, sizeof(DWORD)*w*h);
  imgs[no].w = w;
  imgs[no].h = h;

  if (isPal) {
    imgs[no].paldat = (BYTE *)malloc(sizeof(BYTE)*w*h);
    memcpy(imgs[no].paldat, pdat, sizeof(BYTE)*w*h);
  } else {
    imgs[no].paldat = NULL;
  }
}

void CSPR::SetSameColorUnusedPal(DWORD BBGGRR)
{
  bool u[256];
  for (int i=0; i<256; i++) {
    u[i] = false;
  }

  FindUnusedPal(u);
  for (int i=1; i<256; i++) {
    if (u[i] == false) {
      pal[i] = 0xFF000000 | BBGGRR;
    }
  }
}

void CSPR::FindUnusedPal(bool *u)
{
  for (int i=0; i<numPalImg; i++) {
    for (int y=0; y<imgs[i].h; y++) {
      for (int x=0; x<imgs[i].w; x++) {
        u[ imgs[i].paldat[y*imgs[i].w+x] ] = true;
      }
    }
  }
}

void CSPR::SwapImage(int idA, int idB)
{
  /*
  if (idA < numPalImg && numPalImg <= idB)  return;
  if (idB < numPalImg && numPalImg <= idB) return;
  */

  AnySprite tmpS;
  tmpS.dat = NULL;
  tmpS.paldat = NULL;

  CopyImage(&tmpS, &imgs[idA]);
  CopyImage(&imgs[idA], &imgs[idB]);
  CopyImage(&imgs[idB], &tmpS);

  free(tmpS.dat);
  free(tmpS.paldat);
}

void CSPR::CopyImage(AnySprite *to, AnySprite *from)
{
  to->w = from->w;
  to->h = from->h;
  free(to->dat);
  to->dat = (DWORD *)malloc(from->w * from->h * sizeof(DWORD));
  memcpy(to->dat, from->dat, from->w * from->h * sizeof(DWORD));
  if (from->paldat != NULL) {
    free(to->paldat);
	to->paldat = (BYTE *)malloc(from->w * from->h);
    memcpy(to->paldat, from->paldat, from->w * from->h);
  }
}

void CSPR::InsertImage(int id, int w, int h, DWORD *odat, BYTE *pdat, bool isPal)
{
  AddImage(w, h, odat, pdat, isPal, false);
  for (int i=numOfImg-1; i>=id; i--) {
    SwapImage(i, i+1);
  }
  if (isPal) {
    numPalImg++;
  } else {
    numFlatImg++;
  }
  numOfImg++;
}

void CSPR::DelImage(int id)
{
  for (int i=id; i<numOfImg-1; i++) {
    SwapImage(i, i+1);
  }
  free( imgs[numOfImg-1].dat );
  if (id == 0) {
    if (numPalImg > 1) {
      memcpy((imgs+1)->paldat, imgs->paldat, sizeof(DWORD)*256);
    }
    for (int j=id+1; j<numPalImg; j++) {
      (imgs+j)->paldat = (imgs+1)->paldat;
    }
    free( imgs[numOfImg-1].paldat );
  }
  if (id < numPalImg) {
    numPalImg--;
  } else {
    numFlatImg--;
  }
  numOfImg--;

  return;
}

int CSPR::GetNumPalImage(void)
{
  return numPalImg;
}

int CSPR::GetNumFlatImage(void)
{
  return numFlatImg;
}

int CSPR::GetNumImage(void)
{
  return numOfImg;
}

int CSPR::GetImageWidth(int id)
{
  return imgs[id].w;
}

int CSPR::GetImageHeight(int id)
{
  return imgs[id].h;
}

DWORD *CSPR::GetImage(int id)
{
  return imgs[id].dat;
}
