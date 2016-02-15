#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

#include "actor.h"

CActor::CActor()
{
  curAct = 0;
  curPat = 0;
  curDir = 0;
  curImg = 0;
  sprRefHead = NULL;
  sprRefBody = NULL;
  sprRefEtc = NULL;
  sprRefNeigh = NULL;
  prioRefSpr.head = true;
  prioRefSpr.body = false;
  prioRefSpr.etc = true;
  prioRefSpr.neigh = false;
  drawHLine = true;
  drawVLine = true;
  dragState = 0;
  validPrevCommand = false;
  startRec = 0;
  curRec = 0;
  endRec = 1;
  drawStop = true;
}

CActor::~CActor()
{
}

void CActor::DrawSprite(int x, int y, int no, bool withBorder, DWORD colorBGR)
{
  if (no < 0 || no > GetNumImage())  return;
  int w, h;
  w = GetImageWidth(no);
  h = GetImageHeight(no);

  DWORD *Img1 = GetImage(no);
  int texW = Get2NSize(w);
  int texH = Get2NSize(h);
  DWORD *dummy = (DWORD *)calloc(texW*texH, sizeof(DWORD));
  glTexImage2D(GL_TEXTURE_2D, 0, 4, texW, texH, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, dummy);
  free(dummy);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA,
               GL_UNSIGNED_BYTE, Img1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#ifndef USE_LINE
  glEnable(GL_TEXTURE_2D);
#endif
  GLfloat Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
  Ax = (GLfloat)(x - w/2.);  Ay = (GLfloat)(y + h/2.);
  Bx = (GLfloat)(x + w/2.);  By = Ay;
  Cx = Bx                 ;  Cy = (GLfloat)(y - h/2.);
  Dx = Ax                 ;  Dy = Cy;
  glBegin(GL_QUADS);
    glColor4f(1, 1, 1, 1);
    glTexCoord2f(0, h / (float)texH);
    glVertex3f(Dx, Dy, 0);
    glTexCoord2f(w / (float)texW, h / (float)texH);
    glVertex3f(Cx, Cy, 0);
    glTexCoord2f(w / (float)texW, 0);
    glVertex3f(Bx, By, 0);
    glTexCoord2f(0, 0);
    glVertex3f(Ax, Ay, 0);
  glEnd();
#ifndef USE_LINE
  glDisable(GL_TEXTURE_2D);
#endif
  if (withBorder) {
    glBegin(GL_LINE_LOOP);
      glColor4ub((BYTE)colorBGR, (BYTE)(((WORD)colorBGR) >> 8), (BYTE)((colorBGR >> 16) & 0xFF), 0xFF);
      glVertex3f(Ax, Ay, 0);
      glVertex3f(Bx, By, 0);
      glVertex3f(Cx, Cy, 0);
      glVertex3f(Dx, Dy, 0);
    glEnd();
  }
}

bool CActor::ReadFileSPRorBMPAndAct(char *filename)
{
  bool ret = false;
  int suf_len;

  if (STRNCASECMP(&filename[strlen(filename)-3], "spr", 3) == 0) {
    ret = ReadSpr(filename);
    suf_len = 4;
  } else if (STRNCASECMP(&filename[strlen(filename)-3], "bmp", 3) == 0) {
    ret = ReadBmp(filename);
    suf_len = 7; // 7="xxx.bmp"
  }
  if (ret) {
    char actfname[FILENAME_MAX];
    strncpy(actfname, filename, strlen(filename) - suf_len);
    actfname[strlen(filename) - suf_len] = '\0';
    strcat(actfname, ".act");
    ret = ReadAct(actfname);
  }
  return ret;
}

void CActor::CheckIfAddSuffix(char *filename, int maxLen, char *suffix)
{
  if (strlen(filename)+1+strlen(suffix) > (size_t)maxLen)  return;
  if (STRNCASECMP(&filename[strlen(filename)-strlen(suffix)], suffix, strlen(suffix)) == 0)  return;
  strcat(filename, suffix);
}

bool CActor::ReadAct(char *filename)
{
  FILE *fp;
  BYTE *dat;
  int len;

  if ((fp = fopen(filename, "rb")) == NULL) {
    return false;
  }
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  dat = (BYTE *)malloc(len);
  fseek(fp, 0, SEEK_SET);
  fread(dat, 1, len, fp);
  fclose(fp);

  delete act;
  act = new CACT(dat);

  free(dat);
  return true;
}

bool CActor::ReadSpr(char *filename)
{
  FILE *fp;
  BYTE *dat;
  int len;

  if ((fp = fopen(filename, "rb")) == NULL) {
    return false;
  }
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  dat = (BYTE *)malloc(len);
  fseek(fp, 0, SEEK_SET);
  fread(dat, 1, len, fp);
  fclose(fp);

  delete spr;
  spr = new CSPR(dat, len);

  free(dat);
  return true;
}

bool CActor::ReadBmp(char *filename)
{
  int minno;
  char basefn[FILENAME_MAX];
  char fn[FILENAME_MAX];
  FILE *fp;
  int i;

  FindMinimumNoBmp(&minno, basefn, filename);
  delete spr;
  spr = new CSPR(NULL, 0);

  //use pallet in first image
  sprintf(fn, "%s%03d.bmp", basefn, minno);
  if ((fp = fopen(fn, "rb")) == NULL)  return false;
  ReplacePallet(fp);
  if (!AddPalletImg(fp))  return false;
  fclose(fp);

  //read second...
  for (i=1; i<1000; i++) {
    sprintf(fn, "%s%03d.bmp", basefn, minno+i);
    if ((fp = fopen(fn, "rb")) == NULL) break;
    if (!AddPalletImg(fp)) {
      fclose(fp);
      return false;
    }
    fclose(fp);
  }
  //check tga
  for (int j=i; j<1000; j++) {
    sprintf(fn, "%s%03d.tga", basefn, minno+j);
    if ((fp = fopen(fn, "rb")) == NULL) break;
    if (!AddFlatImg(fp)) {
      fclose(fp);
      return false;
    }
    fclose(fp);
  }

  return true;
}

#ifdef WIN32
void CActor::FindMinimumNoBmp(int *minno, char *basefn, char *filename)
{
  struct _finddata_t c_file;
  long hFile;
  char tmpfn[FILENAME_MAX];
  char *bufc;
  char bpath[FILENAME_MAX];
  int fn_offs;
  int retno;

  *minno = INT_MAX;
  for (bufc = filename+strlen(filename)-5; bufc >= filename; bufc--) {
    if (*bufc < '0' || '9' < *bufc)  break;
  }
  strncpy(basefn, filename, bufc-filename+1);
  basefn[bufc-filename+1] = '\0';
  sprintf(tmpfn, "%s*.bmp", basefn);
  bufc = basefn + strlen(basefn) - 1;
  while (bufc != basefn) {
#ifdef WIN32
    if (*bufc == '\\')  break;
#else
    if (*bufc == '/')  break;
#endif
    bufc--;
  }
  if (bufc != basefn) {
    strncpy(bpath, basefn, bufc-basefn+1);
    bpath[bufc-basefn+1] = '\0';
  } else {
    bpath[0] = '\0';
  }

  fn_offs = (int)(strlen(basefn) - strlen(bpath));
  if ((hFile = (long)_findfirst(tmpfn, &c_file)) == -1L) return;
  retno = FindMinimumNoBmp_GetNo(c_file.name, fn_offs);
  if (*minno > retno)  *minno = retno;
  while (_findnext(hFile, &c_file) == 0) {
    retno = FindMinimumNoBmp_GetNo(c_file.name, fn_offs);
    if (*minno > retno)  *minno = retno;
  }
  _findclose(hFile);
}
#endif

#ifndef WIN32
void CActor::FindMinimumNoBmp(int *minno, char *basefn, char *filename)
{
  DIR *dp;
  struct dirent *entry;
  char *bufc;
  int retno;

  *minno = INT_MAX;
  for (bufc = filename+strlen(filename)-5; bufc >= filename; bufc--) {
    if (*bufc < '0' || '9' < *bufc)  break;
  }
  strncpy(basefn, filename, bufc-filename+1);
  basefn[bufc-filename+1] = '\0';

  if ((dp = opendir(".")) == NULL) return;
  while ((entry = readdir(dp)) != NULL) {
    if (entry->d_type == DT_DIR)  continue;
    //if (strstr(basefn, entry->d_name) != basefn)  continue;
    if (strstr(basefn, entry->d_name) != 0)  continue;
    if (STRNCASECMP(&entry->d_name[strlen(entry->d_name)-4], ".bmp", 4) != 0)  continue;
    retno = FindMinimumNoBmp_GetNo(entry->d_name, 0);
    if (*minno > retno)  *minno = retno;
  }
  closedir(dp);
}
#endif

int CActor::FindMinimumNoBmp_GetNo(char *filename, int offs)
{
  char *bufc, *endc;
  char bufnum[10];

  endc = bufc = filename + offs;
  while (*endc != '\0') {
    if (*endc < '0' || '9' < *endc)  break;
    endc++;
  }
  if (endc - bufc < sizeof(bufnum)) {
    strncpy(bufnum, bufc, endc-bufc);
    bufnum[endc-bufc] = '\0';
  } else {
    return INT_MAX;
  }
  return atoi(bufnum);
}

bool CActor::ReplacePallet(FILE *fp)
{
  BYTE *fdat;
  int flen;

  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fdat = (BYTE *)malloc(flen);
  fseek(fp, 0, SEEK_SET);
  fread(fdat, 1, flen, fp);

  if (!Check256Bitmap(fdat))  return false;

  int offs;
  BITMAPINFO *bi;

  offs = sizeof(BITMAPFILEHEADER);
  bi = (BITMAPINFO *)(fdat + offs);
  offs += bi->bmiHeader.biSize;
  BYTE r, g, b;
  for (int i=0; i<256; i++) {
    b = *(fdat + offs + i*4 + 0);
    g = *(fdat + offs + i*4 + 1);
    r = *(fdat + offs + i*4 + 2);
    *(spr->pal + i) = 0xFF000000 | b << 16 | g << 8 | r;
  }
  free(fdat);
  return true;
}

bool CActor::AddPalletImg(FILE *fp)
{
  BYTE *fdat;
  int flen;

  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fdat = (BYTE *)malloc(flen);
  fseek(fp, 0, SEEK_SET);
  fread(fdat, 1, flen, fp);

  if (!Check256Bitmap(fdat))  return false;

  DWORD *dat;
  BYTE *pdat;
  int w, h;
  MakeFlatImgFromBmp(&dat, &pdat, fdat, &w, &h);

  bool flag;
  flag = true;
  flag = spr->AddImage(w, h, dat, pdat, true, true);
  free(dat);
  free(pdat);
  free(fdat);
  return flag;
}

void CActor::MakeFlatImgFromBmp(DWORD **dat, BYTE **pdat, BYTE *fdat, int *w, int *h)
{
  int w2;
  BITMAPINFO *bi;

  bi = (BITMAPINFO *)(fdat + sizeof(BITMAPFILEHEADER));

  *w = bi->bmiHeader.biWidth;
  w2 = 4 - (*w)%4;
  if (w2 == 4)  w2 = 0;
  w2 += *w;
  *h = bi->bmiHeader.biHeight;

  int offs;
  offs = sizeof(BITMAPFILEHEADER)+bi->bmiHeader.biSize;
  offs += 4*256;
  *dat = (DWORD *)malloc(sizeof(DWORD)*(*w)*(*h));
  *pdat = (BYTE *)malloc(sizeof(BYTE)*(*w)*(*h));
  for (int y=0; y<*h; y++) {
    for (int x=0; x<*w; x++) {
      if (*(fdat+offs+((*h)-y-1)*w2+x) == 0) {
        *(*dat+y*(*w)+x) = 0x0;
      } else {
        *(*dat+y*(*w)+x) = *(spr->pal + *(fdat+offs+((*h)-y-1)*w2+x)) | 0xFF000000;
      }
      *(*pdat+y*(*w)+x) = *(fdat+offs+((*h)-y-1)*w2+x);
    }
  }
}

void CActor::MakeFlatImgFromTga(DWORD **dat, BYTE *fdat, int *w, int *h)
{
  *w = *(WORD *)(fdat+0xC);
  *h = *(WORD *)(fdat+0xE);

  *dat = (DWORD *)malloc(sizeof(DWORD) * *w * *h);
  int offs = 0x12;
  BYTE r, g, b, a;
  for (int y=0; y<*h; y++) {
    for (int x=0; x<*w; x++) {
      b = *(fdat + y* *w*4 + x*4 + offs + 0);
      g = *(fdat + y* *w*4 + x*4 + offs + 1);
      r = *(fdat + y* *w*4 + x*4 + offs + 2);
      a = *(fdat + y* *w*4 + x*4 + offs + 3);
      *(*dat + (*h-y-1)* *w + x) = a << 24 | b << 16 | g << 8 | r;
    }
  }
}

bool CActor::Check256Bitmap(BYTE *dat)
{
  BITMAPFILEHEADER *bmf;
  BITMAPINFO *bi;

  bmf = (BITMAPFILEHEADER *)dat;
  if (bmf->bfType != ('M' << 8 | 'B')) {
    return false;
  }
  bi = (BITMAPINFO *)(dat+sizeof(BITMAPFILEHEADER));
  if (bi->bmiHeader.biBitCount != 8) {
    return false;
  }
  return true;
}

bool CActor::AddFlatImg(FILE *fp)
{
  BYTE *fdat;
  int flen;

  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fdat = (BYTE *)malloc(flen);
  fseek(fp, 0, SEEK_SET);
  fread(fdat, 1, flen, fp);

  if (!Check32BitTga(fdat))  return false;

  BYTE r, g, b, a;
  int w, h;
  BYTE *p;

  p = fdat;
  w = *(WORD *)(p+0xC);
  h = *(WORD *)(p+0xE);
  p += 0x12;
  DWORD *dat;
  BYTE *pdat;
  dat = (DWORD *)malloc(sizeof(DWORD)*w*h);
  pdat = NULL;
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      b = *(p+y*w*4+x*4+0);
      g = *(p+y*w*4+x*4+1);
      r = *(p+y*w*4+x*4+2);
      a = *(p+y*w*4+x*4+3);
      *(dat+(h-y-1)*w+x) = a << 24 | b << 16 | g << 8 | r;
    }
  }

  bool flag;
  flag = true;
  flag = spr->AddImage(w, h, dat, pdat, false, true);
  free(dat);
  free(fdat);
  return flag;
}

bool CActor::Check32BitTga(BYTE *dat)
{
  if (*(dat+0) != 0) return false;
  if (*(dat+1) != 0) return false;
  if (*(dat+2) != 2) return false;
  if (*(dat+0x10) != 32)  return false;
  if (*(dat+0x11) != 0x08)  return false;

  return true;
}

bool CActor::ReplaceImage(char *filename)
{
  FILE *fp;
  BYTE *fdat;
  int flen;

  if ((fp = fopen(filename, "rb")) == NULL)  return false;
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fdat = (BYTE *)malloc(flen);
  fseek(fp, 0, SEEK_SET);
  fread(fdat, 1, flen, fp);
  fclose(fp);

  DWORD *dat;
  BYTE *pdat;
  int w, h;
  if (curImg < spr->numPalImg) {
    if (!Check256Bitmap(fdat))  return false;
    MakeFlatImgFromBmp(&dat, &pdat, fdat, &w, &h);
    spr->ChangeImg(curImg, w, h, dat, pdat, true);
    free(pdat);
  } else {
    if (!Check32BitTga(fdat))  return false;
    MakeFlatImgFromTga(&dat, fdat, &w, &h);
    spr->ChangeImg(curImg, w, h, dat, NULL, false);
  }

  free(dat);
  free(fdat);

  return true;
}

void CActor::InsertImage(int id, char *filename)
{
  FILE *fp;
  BYTE *fdat;
  int flen;

  if ((fp = fopen(filename, "rb")) == NULL)  return;
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fdat = (BYTE *)malloc(flen);
  fseek(fp, 0, SEEK_SET);
  fread(fdat, 1, flen, fp);
  fclose(fp);

  DWORD *dat;
  BYTE *pdat;
  int w, h;
  if (strncmp(&filename[strlen(filename)-3], "bmp", 3) == 0) {
    if (!Check256Bitmap(fdat))  return;
    MakeFlatImgFromBmp(&dat, &pdat, fdat, &w, &h);
    spr->InsertImage(id, w, h, dat, pdat, true);
    free(pdat);
  } else {
    if (!Check32BitTga(fdat))  return;
    MakeFlatImgFromTga(&dat, fdat, &w, &h);
    spr->InsertImage(id, w, h, dat, NULL, false);
  }

  free(dat);
  free(fdat);

  SlideSprNo(id+1, true);

  return;
}

void CActor::DeleteCurrentImage(void)
{
  spr->DelImage(curImg);
  SlideSprNo(curImg+1, false);
  int numImage = GetNumImage();
  if (curImg >= numImage) {
    curImg = numImage - 1;
  }
}

void CActor::SlideSprNo(int aftThis, bool plus)
{
  if (IsActReady() == false) return;

  act->SlideSprNo(aftThis, plus);
}

void CActor::CheckReferenceAny(char *dirFirst, char *dirSecond)
{
  CheckRefHead(dirFirst);
  if (sprRefHead->IsReady() == false) {
    CheckRefHead(dirSecond);
  }
  CheckRefBody(dirFirst);
  if (sprRefBody->IsReady() == false) {
    CheckRefBody(dirSecond);
  }
  CheckRefEtc(dirFirst);
  if (sprRefEtc->IsReady() == false) {
    CheckRefEtc(dirSecond);
  }
  CheckRefNeigh(dirFirst);
  if (sprRefNeigh->IsReady() == false) {
    CheckRefNeigh(dirSecond);
  }
}

void CActor::CheckRefHead(char *dir)
{
  CheckRefXXXX(&sprRefHead, &enableRefHead, &matchRefHead, dir, REFHEAD_ACT, REFHEAD_SPR);
}

void CActor::CheckRefBody(char *dir)
{
  CheckRefXXXX(&sprRefBody, &enableRefBody, &matchRefBody, dir, REFBODY_ACT, REFBODY_SPR);
}

void CActor::CheckRefEtc(char *dir)
{
  CheckRefXXXX(&sprRefEtc, &enableRefEtc, &matchRefEtc, dir, REFETC_ACT, REFETC_SPR);
}

void CActor::CheckRefNeigh(char *dir)
{
  CheckRefXXXX(&sprRefNeigh, &enableRefNeigh, &matchRefNeigh, dir, REFNEIGH_ACT, REFNEIGH_SPR);
}

void CActor::CheckRefXXXX(CSprite **ref, bool *enable, bool *match, char *dir, char *actn, char *sprn)
{
  char actfn[FILENAME_MAX], sprfn[FILENAME_MAX];
#ifdef WIN32
  sprintf(actfn, "%s\\%s", dir, actn);
  sprintf(sprfn, "%s\\%s", dir, sprn);
#else
  sprintf(actfn, "%s/%s", dir, actn);
  sprintf(sprfn, "%s/%s", dir, sprn);
#endif
  delete *ref;
  *ref = new CSprite(actfn, sprfn);
  *enable = *match = false;
  if ((*ref)->IsReady()) {
    if ((*ref)->GetNumAction() == GetNumAction()) {
      *match = true;
    }
  }
}

void CActor::DrawFrameAll_HVLine(void)
{
  glBegin(GL_LINES);
    glColor4ub(GETCOL_R(colorLine), GETCOL_G(colorLine), GETCOL_B(colorLine), 0xFF);
    if (drawHLine) {
      glVertex3f((GLfloat)(-cameraAll.dist*20), (GLfloat)heightAll/4, 0);
      glVertex3f((GLfloat)(cameraAll.dist*20), (GLfloat)heightAll/4, 0);
    }
    if (drawVLine) {
      glVertex3f((GLfloat)widthAll/2, (GLfloat)(-cameraAll.dist*20), 0);
      glVertex3f((GLfloat)widthAll/2, (GLfloat)(cameraAll.dist*20), 0);
    }
  glEnd();
}

void CActor::DrawFrameAll_Reference(bool front)
{
  int tmpp;
  int x = widthAll/2;
  int y = heightAll/4;

  if ((prioRefSpr.neigh == front) && enableRefNeigh) {
    if (matchRefNeigh) {
      tmpp = sprRefNeigh->GetNumPattern(curAct);
      if (tmpp > curPat) {
        tmpp = curPat;
      }
      sprRefNeigh->DrawPattern(x + pointNeigh.x, y - pointNeigh.y, curAct, tmpp);
    } else {
      sprRefNeigh->DrawPattern(x + pointNeigh.x, y - pointNeigh.y, 0, 0);
    }
  }
  if ((prioRefSpr.etc == front) && enableRefEtc) {
    if (matchRefEtc) {
      tmpp = sprRefEtc->GetNumPattern(curAct);
      if (tmpp > curPat) {
        tmpp = curPat;
      }
      sprRefEtc->DrawPattern(x, y, curAct, tmpp);
    } else {
      sprRefEtc->DrawPattern(x, y, 0, 0);
    }
  }
  if ((prioRefSpr.body == front) && enableRefBody) {
    if (matchRefBody) {
      tmpp = sprRefBody->GetNumPattern(curAct);
      if (tmpp > curPat) {
        tmpp = curPat;
      }
      sprRefBody->DrawPattern(x, y, curAct, tmpp);
    } else {
      sprRefBody->DrawPattern(x, y, 0, 0);
    }
  }
  if ((prioRefSpr.head == front) && enableRefHead) {
    int extX, extY;
    GetExtOffsPoint(&extX, &extY, curAct, curPat);
    if (matchRefHead) {
      tmpp = sprRefHead->GetNumPattern(curAct);
      if (tmpp > curPat) {
        tmpp = curPat;
      }
      sprRefHead->DrawPatternHead(x + extX, y - extY, curAct, tmpp);
    } else {
      sprRefHead->DrawPatternHead(x + extX, y - extY, 0, 0);
    }
  }
}

void CActor::DrawFrameAll_Marker(int *id, int numID)
{
  int x = widthAll/2;
  int y = heightAll/4;
  int xOffs, yOffs;
  switch (dragState) {
  case DRAG_ALL_S_PAN:
    break;
  case DRAG_ALL_S_ZOOM:
    if (numID == 0)  break;
    GetOffsPoint(&xOffs, &yOffs, curAct, curPat, *id);
    xOffs += x;
    yOffs = y - yOffs;
    glBegin(GL_LINE_LOOP);
      glColor4ub(GETCOL_R(colorLine), GETCOL_G(colorLine), GETCOL_B(colorLine), 0xFF);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 5, yOffs + cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 5, yOffs + cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 5, yOffs - cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 5, yOffs - cameraAll.dist / (float)y / ratioAll * 2.5f);
    glEnd();
    break;
  case DRAG_ALL_S_ROT:
    if (numID == 0)  break;
    GetOffsPoint(&xOffs, &yOffs, curAct, curPat, *id);
    xOffs += x;
    yOffs = y - yOffs;
    glBegin(GL_LINE_STRIP);
      glColor4ub(GETCOL_R(colorLine), GETCOL_G(colorLine), GETCOL_B(colorLine), 0xFF);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 20          , (float)yOffs);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 20 * 0.7071f, yOffs - cameraAll.dist / (float)y * 10 / ratioAll * 0.7071f);
      glVertex2f((float)xOffs                                    , yOffs - cameraAll.dist / (float)y * 10 / ratioAll);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 20 * 0.7071f, yOffs - cameraAll.dist / (float)y * 10 / ratioAll * 0.7071f);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 20          , (float)yOffs);
    glEnd();
    glBegin(GL_LINE_STRIP);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 10, yOffs - cameraAll.dist / (float)y * 2.5f / ratioAll);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 20, (float)yOffs);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 30, yOffs - cameraAll.dist / (float)y * 5    / ratioAll);
    glEnd();
    glBegin(GL_LINE_STRIP);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 27.5f, yOffs - cameraAll.dist / (float)y * 3.75f / ratioAll);
      glVertex2f(xOffs + cameraAll.dist / (float)x *  20  , (float)yOffs);
      glVertex2f(xOffs + cameraAll.dist / (float)x *  5   , yOffs - cameraAll.dist / (float)y * 3.75f / ratioAll);
    glEnd();
    break;
  case DRAG_ALL_C_PAN:
    break;
  case DRAG_ALL_C_ZOOM:
    xOffs = (int)(cameraAll.c_p.x + (firstMP.x - x  ) * cameraAll.dist*4 / (float)x/2);
    yOffs = (int)(cameraAll.c_p.y + (y*2 - firstMP.y) * cameraAll.dist*4 / (float)y/4 / ratioAll);
    glBegin(GL_LINE_LOOP);
      glColor4ub(GETCOL_R(colorLine), GETCOL_G(colorLine), GETCOL_B(colorLine), 0xFF);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 5, yOffs + cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 5, yOffs + cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs + cameraAll.dist / (float)x * 5, yOffs - cameraAll.dist / (float)y / ratioAll * 2.5f);
      glVertex2f(xOffs - cameraAll.dist / (float)x * 5, yOffs - cameraAll.dist / (float)y / ratioAll * 2.5f);
    glEnd();
  }
}

void CActor::DrawFrameAll_WithBorder(int *id, int numID)
{
  int x = widthAll/2;
  int y = heightAll/4;
  for (int i=0; i<numID; i++, id++) {
    if (*id == GetNumSprites(curAct, curPat)) {
      if (enableRefHead) {
        int xOffs, yOffs;
        GetExtOffsPoint(&xOffs, &yOffs, curAct, curPat);
        sprRefHead->Draw1SpriteHeadWithBorder(x+xOffs, y-yOffs, curAct, curPat, colorSelect);
      }
    } else {
      Draw1SpriteWithBorder(x, y, curAct, curPat, *id, colorSelect);
    }
  }
}

void CActor::DrawFrameAll(int *id, int numID)
{
  CameraSet(&cameraAll, ratioAll);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  DrawFrameAll_HVLine();
  DrawFrameAll_Reference(false);
  if (dragState == 0) {
    DrawFrameAll_WithBorder(id, numID);
  }
  DrawPattern(widthAll/2, heightAll/4, curAct, curPat);
  if (DRAG_ALL_S_PAN <= dragState && dragState <= DRAG_ALL_S_ROT) {
    DrawFrameAll_WithBorder(id, numID);
  }
  DrawFrameAll_Reference(true);
  DrawFrameAll_Marker(id, numID);

  glFlush();
}

void CActor::CameraSet(CAM_STATE *camera, GLfloat ratio)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-camera->dist*2 , camera->dist*2,
          -camera->dist*2 / ratio, camera->dist*2 / ratio,
          -5000, 5000);
  {
    GLfloat ex, ey, ez, cx, cy, cz, ux, uy, uz;
    cx = camera->c_p.x;
    cy = camera->c_p.y;
    cz = camera->c_p.z;
    ex = cx - camera->eye_v.x * camera->dist;
    ey = cy - camera->eye_v.y * camera->dist;
    ez = cz - camera->eye_v.z * camera->dist;
    ux = camera->v_v.x;
    uy = camera->v_v.y;
    uz = camera->v_v.z;
    gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);
  }
}

void CActor::GetViewableStartEndImgNo(int *startNo, int *endNo, int no)
{
  if (IsSprReady() == false)  return;

  int maxImgNo = GetNumImage() - 1;
  int totalWidth;
  int i;

  // backward
  totalWidth = 3 + GetImageWidth(0) / 2;
  for (i=1; i<maxImgNo; i++) {
    totalWidth += GetImageWidth(i-1) / 2 + 10;
    totalWidth += GetImageWidth(i) / 2;
    if (totalWidth > widthImage/2) {
      break;
    }
  }
  if (no <= i) {
    *startNo = 0;
  } else {
    *startNo = -1;
  }

  //forward
  totalWidth = GetImageWidth(maxImgNo) / 2;
  for (i=maxImgNo; i>0; i--) {
    totalWidth += GetImageWidth(i) / 2 + 10;
    totalWidth += GetImageWidth(i-1) / 2;
    if (totalWidth > widthImage/2) {
      break;
    }
  }
  if (no >= i) {
    *endNo = maxImgNo;
  } else {
    *endNo = -1;
  }

  //whole
  totalWidth = 0;
  for (i=0; i<maxImgNo; i++) {
    totalWidth += GetImageWidth(i) + 10;
  }
  totalWidth -= 10;
  if (totalWidth < widthImage) {
    *startNo = 0;
    *endNo = maxImgNo;
  }

  if (*startNo == -1) {
    if (*endNo == -1) {
      for (i=no, totalWidth=GetImageWidth(no) / 2; i>0; i--) {
        totalWidth += GetImageWidth(i) + 10;
        if (totalWidth > widthImage/2) {
          break;
        }
      }
      *startNo = i;
      for (i=*startNo, totalWidth=0; i<maxImgNo; i++) {
        totalWidth += GetImageWidth(i);
        if (totalWidth > widthImage) {
          break;
        }
        totalWidth += 10;
      }
      *endNo = i-1;
      return;
    } else {  // *endno = maximgno
      for (i=maxImgNo, totalWidth=0; i>0; i--) {
        totalWidth += GetImageWidth(i) + 10;
        if (totalWidth > widthImage) {
          *startNo = i+1;
          return;
        }
      }
    }
  } else {  // *startno = 0
    if (*endNo == -1) {
      for (i=0, totalWidth=3; i<maxImgNo; i++) {
        totalWidth += GetImageWidth(i);
        if (totalWidth > widthImage) {
          break;
        }
        totalWidth += 10;
      }
      if (i < *startNo) {
        *endNo = *startNo;
      } else {
        *endNo = i-1;
      }
    }
  }
}

void CActor::SetImages(int no)
{
  int start, end;
  if (dragState == DRAG_IMAGE) {
    GetViewableStartEndImgNo(&start, &end, curImg);
  } else {
    GetViewableStartEndImgNo(&start, &end, no);
  }

  CameraSet(&cameraFrmImg, ratioFrmImg);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

#ifndef USE_LINE
  glEnable(GL_TEXTURE_2D);
#endif
  int w, xOffs;
  xOffs = 3;
  for (int i=start; i<=end; i++) {
    w = GetImageWidth(i);
    if (dragState == DRAG_IMAGE) {
      if (i == no) {
        DrawSprite(xOffs + w/2, heightImage / 2, i, true, colorSelect);
      } else {
        DrawSprite(xOffs + w/2, heightImage / 2, i, false, colorSelect);
      }
    } else {
      if (i == curImg) {
        DrawSprite(xOffs + w/2, heightImage / 2, i, true, colorSelect);
      } else {
        DrawSprite(xOffs + w/2, heightImage / 2, i, false, colorSelect);
      }
    }
    xOffs += w + 10;
  }
#ifndef USE_LINE
  glDisable(GL_TEXTURE_2D);
#endif
  glFlush();
}

int CActor::GetClickImgNo(int pointX)
{
  int start, end;
  GetViewableStartEndImgNo(&start, &end, curImg);
  int ws, we;
  int i;
  ws = we = 3;
  for (i=start; i<=end; i++) {
    we = ws + GetImageWidth(i);
    if (ws <= pointX && pointX <= we) {
      break;
    }
    ws = we+10;
  }
  return i;
}

void CActor::ChangeNeighborPoint(WORD ID)
{
  int pointNeiData[8][2] = {{0, 15}, {-30, 15}, {-30, 0},    // S , SW, W
                            {-30, -25}, {0, -25}, {30, -25}, // NW, N , NE
                            {30, 0}, {30, 15}};              // E , SE

  pointNeigh.x = pointNeiData[ID][0];
  pointNeigh.y = pointNeiData[ID][1];
}

void CActor::SpritePan(BYTE key, int *id, int numID)
{
  Vec origPoint, newPoint;
  origPoint = newPoint = cameraAll.c_p;
  newPoint = newPoint  + cameraAll.u_v * ((oldMP.x - newMP.x) * cameraAll.dist*4 / (float)widthAll);
  newPoint = newPoint  - cameraAll.v_v * ((oldMP.y - newMP.y) * cameraAll.dist*4 / (float)heightAll  / ratioAll );

  int xOffs, yOffs;
  if (key == 0) {
    xOffs = (int)(origPoint.x - newPoint.x);
    yOffs = (int)(newPoint.y - origPoint.y);
  } else {
    switch (key) {
    case VK_LEFT:
      xOffs = -1;  yOffs = 0;
      break;
    case VK_UP:
      xOffs = 0;  yOffs = -1;
      break;
    case VK_RIGHT:
      xOffs = 1;  yOffs = 0;
      break;
    case VK_DOWN:
      xOffs = 0;  yOffs = 1;
      break;
    }
  }

  for (int i=0; i<numID; id++, i++) {
    if (*id == GetNumSprites(curAct, curPat)) {
      // if refhead select
      MoveExtSprite(xOffs, yOffs, curAct, curPat);
      break;
    }
    MoveSprite(xOffs, yOffs, curAct, curPat, *id);
  }
}

void CActor::SpriteZoom(int id, bool equalRatio)
{
  float xMagRatio = 1 - (oldMP.x - newMP.x)/30.f;
  float yMagRatio = 1 - (oldMP.y - newMP.y)/30.f;
  if (equalRatio) {
    if (xMagRatio < yMagRatio) {
      ChangeMagSprite(xMagRatio, xMagRatio, curAct, curPat, id);
    } else {
      ChangeMagSprite(yMagRatio, yMagRatio, curAct, curPat, id);
    }
  } else {
    ChangeMagSprite(xMagRatio, yMagRatio, curAct, curPat, id);
  }
}

void CActor::SpriteRot(int id)
{
  int diffRot = oldMP.x - newMP.x;
  ChangeRotSprite(diffRot, curAct, curPat, id);
}

void CActor::CameraPan(void)
{
  cameraAll.c_p = cameraAll.c_p + cameraAll.u_v * ((oldMP.x - newMP.x) * cameraAll.dist*4 / (float)widthAll);
  cameraAll.c_p = cameraAll.c_p - cameraAll.v_v * ((oldMP.y - newMP.y) * cameraAll.dist*4 / (float)heightAll / ratioAll);
}

void CActor::CameraZoom(void)
{
  GLfloat oldDist;
  oldDist = cameraAll.dist;
  cameraAll.dist += (GLfloat)((oldMP.y - newMP.y) * 0.005 * cameraAll.dist);
  if (cameraAll.dist < 0.001) cameraAll.dist = (GLfloat)0.001;

  cameraAll.c_p = cameraAll.c_p - cameraAll.u_v*(firstMP.x - (float)widthAll/2.f)
                / (float)widthAll * (cameraAll.dist - oldDist)*4;
  cameraAll.c_p = cameraAll.c_p + cameraAll.v_v*(firstMP.y - (float)heightAll/2.f)
                / (float)heightAll * (cameraAll.dist - oldDist)*4 / ratioAll;
}

void CActor::SetupOpenGL(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -10, 10);
  glMatrixMode(GL_MODELVIEW);
#ifdef USE_LINE
  glClearColor(0.75, 0.75, 0.75, 1);
#else
  glClearColor(GETCOL_R(colorBG) / 255.f, GETCOL_G(colorBG) / 255.f, GETCOL_B(colorBG) / 255.f, 1);
#endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
#ifdef USE_LINE
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  glFlush();
}

void CActor::StoreUserCommand(USERCOMMAND *cmd)
{
  validPrevCommand = true;

  curRec++;
  if (curRec == NUMCOMMANDRECORD)  curRec = 0;
  if (curRec == startRec) {
    startRec++;
    if (startRec == NUMCOMMANDRECORD)  startRec = 0;
  }
  endRec = curRec;
  USERCOMMAND *ncmd = &usrcmd[curRec];
  memcpy(ncmd, cmd, sizeof(USERCOMMAND));
}

void CActor::DoPrevUserCommand(void)
{
  if (validPrevCommand == false)  return;
  if (curRec == startRec)  return;
  curRec--;
  if (curRec < 0)  curRec = NUMCOMMANDRECORD-1;
  DoCurUserCommand(&usrcmd[curRec]);

  if (curRec == startRec)  return;
  int nowType = usrcmd[curRec].type;
  int tmpRec = curRec;
  tmpRec--;
  if (tmpRec < 0)  tmpRec = NUMCOMMANDRECORD-1;
  if (nowType != usrcmd[tmpRec].type) {
    curRec = tmpRec;
    DoCurUserCommand(&usrcmd[curRec]);
  }
}

void CActor::DoNextUserCommand(void)
{
  if (validPrevCommand == false)  return;
  if (curRec == endRec)  return;
  curRec++;
  if (curRec == NUMCOMMANDRECORD)  curRec = 0;
  DoCurUserCommand(&usrcmd[curRec]);

  if (curRec == endRec)  return;
  int nowType = usrcmd[curRec].type;
  int tmpRec = curRec;
  tmpRec++;
  if (tmpRec == NUMCOMMANDRECORD)  tmpRec = 0;
  if (nowType != usrcmd[tmpRec].type) {
    curRec = tmpRec;
    DoCurUserCommand(&usrcmd[curRec]);
  }
}

void CActor::DoCurUserCommand(USERCOMMAND *ncmd)
{
  switch (ncmd->type) {
  case COMTYPE_MOVE_XY:
    SetXOffsValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyi[0]);
    SetYOffsValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyi[1]);
    break;
  case COMTYPE_MOVE_EXY:
    SetExtXValue(ncmd->actNo, ncmd->patNo, ncmd->xyi[0]);
    SetExtYValue(ncmd->actNo, ncmd->patNo, ncmd->xyi[1]);
    break;
  case COMTYPE_MAG_XY:
    SetXMagValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyf[0]);
    SetYMagValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyf[1]);
    break;
  case COMTYPE_ROT:
    SetRotValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyd[0]);
    break;
  case COMTYPE_MIRROR:
    SetMirrorValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyd[0]);
    break;
  case COMTYPE_SATURATION:
    SetABGRValue(ncmd->actNo, ncmd->patNo, ncmd->sprId, ncmd->xyd[0]);
    break;
  }
}

void CActor::SetInvalidPrevCommand(void)
{
  validPrevCommand = false;
  startRec = 0;
  curRec = 0;
  endRec = 1;
}

int CActor::AuditAll(int *actNo, int *patNo, int *sprId)
{
  if (AuditCheckSprMax(actNo, patNo, sprId) == false)   return ACTOR_AUDIT_SPRMAXERROR;
  if (AuditCheck0x0205SprWidthHeight(actNo, patNo, sprId) == false)  return ACTOR_AUDIT_0205SPRWIDTHHEIGHTERROR;
  if (AuditCheckSoundMax(actNo, patNo) == false)  return ACTOR_AUDIT_SOUNDMAXERROR;
  return ACTOR_AUDIT_NOERROR;
}

void CActor::ModifyCorrectSprWidthHeight(void)
{
  if (act == NULL)  return;
  if (spr == NULL)  return;
  if (act->GetVersion() != 0x0205)  return;

  int maxpal = spr->numPalImg;
  DWORD spid1, spid2;
  for (int i=0; i<GetNumAction(); i++) {
    for (int j=0; j<GetNumPattern(i); j++) {
      for (int k=0; k<GetNumSprites(i, j); k++) {
        spid1 = GetSprNoValue(i, j, k); 
        if (GetSpTypeValue(i, j, k) == 1) {
          spid2 = spid1 + maxpal;
        } else {
          spid2 = spid1;
        }
        SetSprNoValue(i, j, k, spid2);
      }
    }
  }
}

bool CActor::AuditCheck0x0205SprWidthHeight(int *actNo, int *patNo, int *sprId)
{
  if (act == NULL)  return true;
  if (spr == NULL)  return true;
  if (act->GetVersion() != 0x0205)  return true;

  int maxpal = spr->numPalImg;
  DWORD spid1, spid2;
  DWORD aw, ah, sw, sh;
  for (int i=0; i<GetNumAction(); i++) {
    for (int j=0; j<GetNumPattern(i); j++) {
      for (int k=0; k<GetNumSprites(i, j); k++) {
        spid1 = GetSprNoValue(i, j, k); 
        if (GetSpTypeValue(i, j, k) == 1) {
          spid2 = spid1 + maxpal;
        } else {
          spid2 = spid1;
        }
        GetSprWHValue(i, j, k, &aw, &ah);
        sw = GetImageWidth(spid2);
        sh = GetImageHeight(spid2);
        if (aw != sw)  return false;
        if (ah != sh)  return false;
      }
    }
  }
  return true;
}

bool CActor::AuditCheckSprMax(int *actNo, int *patNo, int *sprId)
{
  if (act == NULL)  return true;
  if (spr == NULL)  return true;

  int maxpal = spr->numPalImg;
  int maxflat = spr->numFlatImg;

  for (int i=0; i<GetNumAction(); i++) {
    for (int j=0; j<GetNumPattern(i); j++) {
      for (int k=0; k<GetNumSprites(i, j); k++) {
        if (GetSpTypeValue(i, j, k) == 0) {
          if (maxpal < (int)GetSprNoValue(i, j, k)) {
            *actNo = i;  *patNo = j;  *sprId = k;
            return false;
          }
        } else {
          if (maxflat < (int)GetSprNoValue(i, j, k)) {
            *actNo = i;  *patNo = j;  *sprId = k;
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool CActor::AuditCheckSoundMax(int *actNo, int *patNo)
{
  if (act == NULL)  return true;
  if (spr == NULL)  return true;

  int maxsound = GetNumSounds();

  for (int i=0; i<GetNumAction(); i++) {
    for (int j=0; j<GetNumPattern(i); j++) {
      if (maxsound <= (int)GetSoundNo(i, j)) {
        *actNo = i;  *patNo = j;
        return false;
      }
    }
  }
  return true;
}

int CActor::GetTimeInterval(void)
{
  int timeInterval = -1;
  timeInterval = (int)GetInterval(curAct);
  if (timeInterval <= 0) timeInterval = INT_MAX;
  return timeInterval;
}

void CActor::SetColors(DWORD BG, DWORD Select, DWORD Line)
{
  colorBG = BG;
  colorSelect = Select;
  colorLine = Line;
}

