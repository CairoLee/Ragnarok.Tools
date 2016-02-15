#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Sprite.h"

CSprite::CSprite(char *actfn, char *sprfn)
{
  act = NULL;
  spr = NULL;
  Img1 = NULL;
  FILE *fp;
  int flen;
  BYTE *adat, *sdat;
  if ((fp = fopen(actfn, "rb")) == NULL)  return;
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  adat = (BYTE *)malloc(flen);
  fread(adat, 1, flen, fp);
  fclose(fp);
  act = new CACT(adat);
  free(adat);

  if ((fp = fopen(sprfn, "rb")) == NULL)  return;
  fseek(fp, 0, SEEK_END);
  flen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  sdat = (BYTE *)malloc(flen);
  fread(sdat, 1, flen, fp);
  fclose(fp);

  spr = new CSPR(sdat, flen);
}

CSprite::CSprite(BYTE *actdat, BYTE *sprdat, int sprlen)
{
  act = new CACT(actdat);
  spr = new CSPR(sprdat, sprlen);
  Img1 = NULL;
}

CSprite::CSprite()
{
  act = NULL;
  spr = NULL;
  Img1 = NULL;
}

CSprite::~CSprite(void)
{
  delete spr;
  delete act;
}

bool CSprite::GetModified(void)
{
  return act->GetModified();
}

void CSprite::SetModified(void)
{
  act->SetModified();
}

WORD CSprite::GetVersion(void)
{
  return act->GetVersion();
}

bool CSprite::IsReady(void)
{
  if (IsSprReady() == false || IsActReady() == false) {
    return false;
  }
  return true;
}

bool CSprite::IsSprReady(void)
{
  if (spr == NULL)  return false;

  return true;
}

bool CSprite::IsActReady(void)
{
  if (act == NULL)  return false;
  
  return true;
}

void CSprite::SetSameColorUnusedPal(DWORD BBGGRR)
{
  spr->SetSameColorUnusedPal(BBGGRR);
}

int CSprite::GetNumAction(void)
{
  return act->GetNumAction();
}

int CSprite::GetNumPattern(int actid)
{
  return act->GetNumPattern(actid);
}

int CSprite::GetNumSprites(int actNo, int patNo)
{
  return act->GetNumSprites(actNo, patNo);
}

void CSprite::DrawPattern(int x, int y, int actid, int patid)
{
  AnyActAnyPat *aaap;
  aaap = &act->aall.aa[actid].aaap[patid];
  int version = act->aall.ah.version & 0xFFFF;

  int spNo;
  for (unsigned int i=0; i<aaap->numOfSpr; i++) {
    switch (version) {
      case 0x0101:
        spNo = aaap->aps0[i].sprNo;
        if (spNo < 0) continue;
        Draw1Sprite(spNo, x + aaap->aps0[i].xOffs, y - aaap->aps0[i].yOffs,
                    1, 1, 0xFFFFFFFF, 0, aaap->aps0[i].mirrorOn);
        break;
      case 0x0201:
      case 0x0202:
      case 0x0203:
        if (aaap->aps1[i].spType == 1) {
          spNo = aaap->aps1[i].sprNo + spr->numPalImg;
        } else {
          spNo = aaap->aps1[i].sprNo;
        }
        if (spNo < 0) continue;
        Draw1Sprite(spNo,
                    x + aaap->aps1[i].xOffs,
                    y - aaap->aps1[i].yOffs,
                    aaap->aps1[i].xyMag, aaap->aps1[i].xyMag,
                    aaap->aps1[i].color, 
                    aaap->aps1[i].rot,
                    aaap->aps1[i].mirrorOn);
        break;
      case 0x0204:
        if (aaap->aps4[i].spType == 1) {
          spNo = aaap->aps4[i].sprNo + spr->numPalImg;
        } else {
          spNo = aaap->aps4[i].sprNo;
        }
        if (spNo < 0) continue;
        Draw1Sprite(spNo,
                    x + aaap->aps4[i].xOffs,
                    y - aaap->aps4[i].yOffs,
                    aaap->aps4[i].xMag, aaap->aps4[i].yMag,
                    aaap->aps4[i].color,
                    aaap->aps4[i].rot,
                    aaap->aps4[i].mirrorOn);
        break;
      case 0x0205:
        if (aaap->aps5[i].spType == 1) {
          spNo = aaap->aps5[i].sprNo + spr->numPalImg;
        } else {
          spNo = aaap->aps5[i].sprNo;
        }
        if (spNo < 0) continue;
        Draw1Sprite(spNo,
                    x + aaap->aps5[i].xOffs + spr->imgs[spNo].w - aaap->aps5[i].sprW,
                    y - aaap->aps5[i].yOffs - spr->imgs[spNo].h + aaap->aps5[i].sprH,
                    aaap->aps5[i].xMag, aaap->aps5[i].yMag,
                    aaap->aps5[i].color,
                    aaap->aps5[i].rot,
                    aaap->aps5[i].mirrorOn);
        break;
    }
  }
}

void CSprite::Draw1SpriteWithBorder(int x, int y, int actid, int patid, int sprid, DWORD RRGGBBAA)
{
  AnyActAnyPat *aaap;
  aaap = &act->aall.aa[actid].aaap[patid];
  int version = act->aall.ah.version & 0xFFFF;
  int spNo;

  switch (version) {
  case 0x0101:
    spNo = aaap->aps0[sprid].sprNo;
    if (spNo < 0) return;
    Draw1SpriteWithBorder_Main(spNo, x + aaap->aps0[sprid].xOffs, y - aaap->aps0[sprid].yOffs,
                1, 1, 0xFFFFFFFF, 0, aaap->aps0[sprid].mirrorOn, RRGGBBAA);
    break;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    spNo = aaap->aps1[sprid].sprNo;
    if (spNo < 0)  return;
    if (aaap->aps1[sprid].spType == 1) {
      spNo += spr->numPalImg;
    }
    Draw1SpriteWithBorder_Main(spNo,
                x + aaap->aps1[sprid].xOffs,
                y - aaap->aps1[sprid].yOffs,
                aaap->aps1[sprid].xyMag, aaap->aps1[sprid].xyMag,
                aaap->aps1[sprid].color, 
                aaap->aps1[sprid].rot,
                aaap->aps1[sprid].mirrorOn, RRGGBBAA);
    break;
  case 0x0204:
    spNo = aaap->aps4[sprid].sprNo;
    if (spNo < 0)  return;
    if (aaap->aps4[sprid].spType == 1) {
      spNo += spr->numPalImg;
    }
    Draw1SpriteWithBorder_Main(spNo,
                x + aaap->aps4[sprid].xOffs,
                y - aaap->aps4[sprid].yOffs,
                aaap->aps4[sprid].xMag, aaap->aps4[sprid].yMag,
                aaap->aps4[sprid].color,
                aaap->aps4[sprid].rot,
                aaap->aps4[sprid].mirrorOn, RRGGBBAA);
    break;
  case 0x0205:
    spNo = aaap->aps5[sprid].sprNo;
    if (spNo < 0)  return;
    if (aaap->aps5[sprid].spType == 1) {
      spNo += spr->numPalImg;
    }
    Draw1SpriteWithBorder_Main(spNo,
                x + aaap->aps5[sprid].xOffs + spr->imgs[spNo].w - aaap->aps5[sprid].sprW,
                y - aaap->aps5[sprid].yOffs - spr->imgs[spNo].h + aaap->aps5[sprid].sprH,
                aaap->aps5[sprid].xMag, aaap->aps5[sprid].yMag,
                aaap->aps5[sprid].color,
                aaap->aps5[sprid].rot,
                aaap->aps5[sprid].mirrorOn, RRGGBBAA);
    break;
  }
}

void CSprite::DrawPatternHead(int x, int y, int actid, int patid)
{
  AnyActAnyPat *aaap;
  aaap = &act->aall.aa[actid].aaap[patid];

  x -= aaap->ExtXoffs;
  y += aaap->ExtYoffs;
  DrawPattern(x, y, actid, patid);
}

void CSprite::Draw1SpriteHeadWithBorder(int x, int y, int actid, int patid, DWORD RRGGBBAA)
{
  AnyActAnyPat *aaap;
  aaap = &act->aall.aa[actid].aaap[patid];

  x -= aaap->ExtXoffs;
  y += aaap->ExtYoffs;
  Draw1SpriteWithBorder(x, y, actid, patid, 1, RRGGBBAA);
}

void CSprite::Draw1Sprite(int sprNo, int x, int y, float xmag, float ymag,
                          DWORD color, int rot, int mirror)
{
  Draw1Sprite_Main(sprNo, x, y, xmag, ymag, color, rot, mirror, 0);
}

void CSprite::Draw1SpriteWithBorder_Main(int sprNo, int x, int y, float xmag, float ymag, DWORD color, int rot, int mirror, DWORD RRGGBBAA)
{
  Draw1Sprite_Main(sprNo, x, y, xmag, ymag, color, rot, mirror, RRGGBBAA);
}

INLINE void CSprite::Draw1Sprite_Main(int sprNo, int x, int y, float xmag, float ymag, DWORD color, int rot, int mirror, DWORD RRGGBBAA)
{
  w = spr->imgs[sprNo].w;
  h = spr->imgs[sprNo].h;

  // Set texture
  Img1 = spr->imgs[sprNo].dat;
  int texw = Get2NSize(w);
  int texh = Get2NSize(h);
  DWORD *dum;
  dum = (DWORD *)calloc(texw*texh, sizeof(DWORD));
  glTexImage2D(GL_TEXTURE_2D, 0, 4, texw, texh, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, dum);
  free(dum);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA,
               GL_UNSIGNED_BYTE, Img1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // D-C
  // | |
  // A-B : define corner
  float Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
  Ax = Ay = Bx = By = Cx = Cy = Dx = Dy = 0;

  if (mirror == 0) {
    //Ax = -(w-w/2)*xmag ;  Ay = (h-h/2)*ymag; 
    Ax = -(w-w/2)*xmag ;  Ay = (h-h/2)*ymag; 
    Bx = Ax + texw*xmag;  By = Ay;
    Cx = Bx            ;  Cy = Ay - texh*ymag;
    Dx = Ax            ;  Dy = Cy;
  } else {
    Ax = (float)(w-(int)((w-0.5)/2))*xmag  ;  Ay = (h-h/2)*ymag;
    //Ax = (w/2)*xmag  ;  Ay = (h-h/2)*ymag;
    Bx = Ax - texw*xmag;  By = Ay;
    Cx = Bx            ;  Cy = Ay - texh*ymag;
    Dx = Ax            ;  Dy = Cy;
  }

  // rotation
  if (rot != 0) {
    TwoDimentionRot((float)rot, &Ax, &Ay);
    TwoDimentionRot((float)rot, &Bx, &By);
    TwoDimentionRot((float)rot, &Cx, &Cy);
    TwoDimentionRot((float)rot, &Dx, &Dy);
  }
  //
  Ax += x; Ay += y;
  Bx += x; By += y;
  Cx += x; Cy += y;
  Dx += x; Dy += y;


#ifndef USE_LINE
  glEnable(GL_TEXTURE_2D);
#endif
  glBegin(GL_QUADS);
    glColor4ub((GLubyte)( color        & 0xFF),
               (GLubyte)((color >>  8) & 0xFF),
               (GLubyte)((color >> 16) & 0xFF),
               (GLubyte)( color >> 24        ));
#ifdef USE_LINE
    glColor4f(1, 1, 1, 1);
#endif
    glTexCoord2f(0, 1);
    glVertex3f(Dx, Dy, 0);
    glTexCoord2f(1, 1);
    glVertex3f(Cx, Cy, 0);
    glTexCoord2f(1, 0);
    glVertex3f(Bx, By, 0);
    glTexCoord2f(0, 0);
    glVertex3f(Ax, Ay, 0);
  glEnd();
#ifndef USE_LINE
  glDisable(GL_TEXTURE_2D);
#endif

  if (RRGGBBAA) {
    if (mirror == 0) {
      Ax = -(w-w/2)*xmag;  Ay = (h-h/2)*ymag; 
      Bx = Ax + w*xmag  ;  By = Ay;
      Cx = Bx           ;  Cy = Ay - h*ymag;
      Dx = Ax           ;  Dy = Cy;
    } else {
      Ax = (float)(w-(int)((w-0.5)/2))*xmag;  Ay = (h-h/2)*ymag;
      Bx = Ax - w*xmag;  By = Ay;
      Cx = Bx         ;  Cy = Ay - h*ymag;
      Dx = Ax         ;  Dy = Cy;
    }

    // rotation
    if (rot != 0) {
      TwoDimentionRot((float)rot, &Ax, &Ay);
      TwoDimentionRot((float)rot, &Bx, &By);
      TwoDimentionRot((float)rot, &Cx, &Cy);
      TwoDimentionRot((float)rot, &Dx, &Dy);
    }
    //
    Ax += x; Ay += y;
    Bx += x; By += y;
    Cx += x; Cy += y;
    Dx += x; Dy += y;

    glBegin(GL_LINE_LOOP);
      glColor4ub((BYTE)RRGGBBAA, ((WORD)RRGGBBAA) >> 8, (BYTE)((RRGGBBAA) >> 16), 0xFF);
      glVertex3f(Dx, Dy, 0);
      glVertex3f(Cx, Cy, 0);
      glVertex3f(Bx, By, 0);
      glVertex3f(Ax, Ay, 0);
    glEnd();
  }
}

int CSprite::Get2NSize(int size)
{
  if (size <= 64) {
    return 64;
  } else if (size <= 128) {
    return 128;
  } else if (size <= 256) {
    return 256;
  } else if (size <= 512) {
    return 512;
  }
  return 1024;
}

void CSprite::WriteAct(char *fname)
{
  act->WriteAct(fname);
}

void CSprite::WriteActText(char *fname)
{
  act->WriteActText(fname);
}

void CSprite::WriteBmp256Tga32(char *basefn, int no, bool onlyone)
{
  spr->WriteBmp256Tga32(basefn, no, onlyone);
}

void CSprite::WriteSpr(char *fname)
{
  spr->WriteSpr(fname);
}

int CSprite::GetNumPalImage(void)
{
  return spr->GetNumPalImage();
}

int CSprite::GetNumFlatImage(void)
{
  return spr->GetNumFlatImage();
}

int CSprite::GetNumImage(void)
{
  return spr->GetNumImage();
}

int CSprite::GetImageWidth(int id)
{
  return spr->GetImageWidth(id);
}

int CSprite::GetImageHeight(int id)
{
  return spr->GetImageHeight(id);
}

void CSprite::TwoDimentionRot(float deg_angle, float *x, float *y)
{
  float tmpx, tmpy;

  tmpx = *x;  tmpy = -*y; // because screen coordinate system
  *x = tmpx * cosf(deg_angle/180.f*M_PI) - tmpy * sinf(deg_angle/180.f*M_PI);
  *y = tmpx * sinf(deg_angle/180.f*M_PI) + tmpy * cosf(deg_angle/180.f*M_PI);
  *y = -*y;
}

int CSprite::GetWHBase(int actid, int patid)  // only second image
{
  AnyActAnyPat *aaap;
  aaap = &act->aall.aa[actid].aaap[patid];
  BYTE version = act->aall.ah.version & 0xFFFF;

  switch (version) {
  case 0x0101:
    return aaap->aps0[1].sprNo;
  case 0x0201:
  case 0x0202:
  case 0x0203:
    return aaap->aps1[1].sprNo;
  case 0x0204:
    return aaap->aps4[1].sprNo;
  case 0x0205:
    return aaap->aps5[1].sprNo;
  default:
    return aaap->aps1[1].sprNo;
  }
}

int CSprite::GetWidth(int actid, int patid)  // only second image
{
  int sprid;
  sprid = GetWHBase(actid, patid);
  return spr->imgs[sprid].w;
}

int CSprite::GetHeight(int actid, int patid)  // only second image
{
  int sprid;
  sprid = GetWHBase(actid, patid);
  return spr->imgs[sprid].h;
}

DWORD *CSprite::GetImage(int id)
{
  return spr->GetImage(id);
}

void CSprite::MoveSprite(int xOffs, int yOffs, int actNo, int patNo, int sprid)
{
  act->MoveSprite(xOffs, yOffs, actNo, patNo, sprid);
}

void CSprite::MoveExtSprite(int xOffs, int yOffs, int actNo, int patNo)
{
  act->MoveExtSprite(xOffs, yOffs, actNo, patNo);
}

void CSprite::ChangeMagSprite(float xMagRatio, float yMagRatio, int actNo, int patNo, int sprid)
{
  act->ChangeMagSprite(xMagRatio, yMagRatio, actNo, patNo, sprid);
}

void CSprite::ChangeRotSprite(int diffRot, int actNo, int patNo, int sprid)
{
  act->ChangeRotSprite(diffRot, actNo, patNo, sprid);
}

void CSprite::SwapSpriteOrder(int actNo, int patNo, int spA, int spB)
{
  act->SwapSpriteOrder(actNo, patNo, spA, spB);
}

void CSprite::DelSpr(int actNo, int patNo, int sprId)
{
  act->DelSpr(actNo, patNo, sprId);
}

void CSprite::AddSpr(int actNo, int patNo, int sprNo)
{
  int w, h;
  int sprNo2;
  int spType;
  if (sprNo >= GetNumPalImage()) {
    sprNo2 = sprNo - GetNumPalImage();
    spType = 1;
  } else {
    sprNo2 = sprNo;
    spType = 0;
  }
  w = GetImageWidth(sprNo);
  h = GetImageHeight(sprNo);
  act->AddSpr(actNo, patNo, sprNo2, spType, w, h);
}

void CSprite::AddPat(int actNo, int basePatNo)
{
  act->AddPat(actNo, basePatNo);
}

void CSprite::SwapPat(int actNo, int patA, int patB)
{
  act->SwapPat(actNo, patA, patB);
}

void CSprite::DelPat(int actNo, int patNo)
{
  act->DelPat(actNo, patNo);
}

void CSprite::SlideSprNo(int aftThis, bool plus)
{
  act->SlideSprNo(aftThis, plus);
}

void CSprite::SetXOffsValue(int actNo, int patNo, int sprId, int val)
{
  act->SetXOffsValue(actNo, patNo, sprId, val);
}

void CSprite::SetYOffsValue(int actNo, int patNo, int sprId, int val)
{
  act->SetYOffsValue(actNo, patNo, sprId, val);
}

void CSprite::SetSprNoValue(int actNo, int patNo, int sprId, DWORD val)
{
  int w, h;
  w = GetImageWidth(val);
  h = GetImageHeight(val);
  act->SetSprNoValue(actNo, patNo, sprId, val, w, h);
}

void CSprite::SetXMagValue(int actNo, int patNo, int sprId, float val)
{
  act->SetXMagValue(actNo, patNo, sprId, val);
}

void CSprite::SetYMagValue(int actNo, int patNo, int sprId, float val)
{
  act->SetYMagValue(actNo, patNo, sprId, val);
}

void CSprite::SetRotValue(int actNo, int patNo, int sprId, DWORD val)
{
  act->SetRotValue(actNo, patNo, sprId, val);
}

void CSprite::SetMirrorValue(int actNo, int patNo, int sprId, DWORD val)
{
  act->SetMirrorValue(actNo, patNo, sprId, val);
}

void CSprite::SetABGRValue(int actNo, int patNo, int sprId, DWORD val)
{
  act->SetABGRValue(actNo, patNo, sprId, val);
}

void CSprite::SetSpTypeValue(int actNo, int patNo, int sprId, DWORD val)
{
  act->SetSpTypeValue(actNo, patNo, sprId, val);
}

void CSprite::SetExtXValue(int actNo, int patNo, int val)
{
  act->SetExtXValue(actNo, patNo, val);
}

void CSprite::SetExtYValue(int actNo, int patNo, int val)
{
  act->SetExtYValue(actNo, patNo, val);
}

void CSprite::GetOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo, int sprid)
{
  act->GetOffsPoint(xOffs, yOffs, actNo, patNo, sprid);
}

DWORD CSprite::GetSprNoValue(int actNo, int patNo, int sprId)
{
  int sprNo;
  act->GetSprNoValue(actNo, patNo, sprId, &sprNo);
  return sprNo;
}

float CSprite::GetXMagValue(int actNo, int patNo, int sprId)
{
  return act->GetXMagValue(actNo, patNo, sprId);
}

float CSprite::GetYMagValue(int actNo, int patNo, int sprId)
{
  return act->GetYMagValue(actNo, patNo, sprId);
}

DWORD CSprite::GetRotValue(int actNo, int patNo, int sprId)
{
  return act->GetRotValue(actNo, patNo, sprId);
}

DWORD CSprite::GetMirrorValue(int actNo, int patNo, int sprId)
{
  return act->GetMirrorValue(actNo, patNo, sprId);
}

DWORD CSprite::GetABGRValue(int actNo, int patNo, int sprId)
{
  return act->GetABGRValue(actNo, patNo, sprId);
}

void CSprite::GetSprWHValue(int actNo, int patNo, int sprId, DWORD *sprW, DWORD *sprH)
{
  return act->GetSprWHValue(actNo, patNo, sprId, sprW, sprH);
}

DWORD CSprite::GetSpTypeValue(int actNo, int patNo, int sprId)
{
  int spType;
  act->GetSpTypeValue(actNo, patNo, sprId, &spType);
  return spType;
}

void CSprite::GetExtOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo)
{
  act->GetExtOffsPoint(xOffs, yOffs, actNo, patNo);
}

void CSprite::CopyAction(int destActNo, int srcActNo)
{
  act->CopyAction(destActNo, srcActNo);
}

float CSprite::GetInterval(int actNo)
{
  return act->GetInterval(actNo);
}

void CSprite::SetInterval(int actNo, float val)
{
  act->SetInterval(actNo, val);
}

int CSprite::GetNumSounds(void)
{
  return act->GetNumSounds();
}

int CSprite::GetSoundNo(int actNo, int patNo)
{
  return act->GetSoundNo(actNo, patNo);
}

void CSprite::SetSoundNo(int actNo, int patNo, int sndNo)
{
  act->SetSoundNo(actNo, patNo, sndNo);
}

char *CSprite::GetSoundFN(int no)
{
  return act->GetSoundFN(no);
}

bool CSprite::AddSound(char *fname)
{
  return act->AddSound(fname);
}

void CSprite::SwapSound(int noA, int noB)
{
  act->SwapSound(noA, noB);
}

void CSprite::DelSound(int no)
{
  act->DelSound(no);
}
