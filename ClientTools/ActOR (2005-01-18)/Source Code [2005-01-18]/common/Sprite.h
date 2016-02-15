#ifndef __MY_SPRITE__
#define __MY_SPRITE__

#include "act.h"
#include "spr.h"

class CSprite
{
private:
  int cur_actid;

  void Draw1Sprite(int sprNo, int x, int y, float xmag, float ymag, DWORD color, int rot, int mirror);
  INLINE void Draw1Sprite_Main(int sprNo, int x, int y, float xmag, float ymag, DWORD color, int rot, int mirror, DWORD RRGGBBAA);
  void Draw1SpriteWithBorder_Main(int sprNo, int x, int y, float xmag, float ymag, DWORD color, int rot, int mirror, DWORD RRGGBBAA);
  void TwoDimentionRot(float angle, float *x, float *y);

public:
  CACT *act;
  CSPR *spr;
  int w, h;
  DWORD *Img1;
  int NumAct;
  int NumPat;

  CSprite(char *actfn, char *sprfn);
  CSprite(BYTE *actdat, BYTE *sprdat, int sprlen);
  CSprite();
  virtual ~CSprite(void);

  bool IsReady(void);
  bool IsSprReady(void);
  bool IsActReady(void);

  int Get2NSize(int size);
  void DrawPattern(int x, int y, int actid, int patid);
  void Draw1SpriteWithBorder(int x, int y, int actid, int patid, int sprid, DWORD RRGGBBAA);
  void Draw1SpriteHeadWithBorder(int x, int y, int actid, int patid, DWORD RRGGBBAA);
  void DrawPatternHead(int x, int y, int actid, int patid);
  int GetWHBase(int actid, int patid);  // only second image
  int GetWidth(int actid, int patid);  // only second image
  int GetHeight(int actid, int patid);  // only second image

  // from CSPR
  void SetSameColorUnusedPal(DWORD BBGGRR);
  void WriteBmp256Tga32(char *basefn, int no, bool onlyone);
  void WriteSpr(char *fname);
  int GetNumPalImage(void);
  int GetNumFlatImage(void);
  int GetNumImage(void);
  int GetImageWidth(int id);
  int GetImageHeight(int id);
  DWORD *GetImage(int id);

  // from CACT
  bool GetModified(void);
  void SetModified(void);
  WORD GetVersion(void);
  int GetNumAction(void);
  int GetNumPattern(int actid);
  int GetNumSprites(int actNo, int patNo);
  void WriteAct(char *fname);
  void WriteActText(char *fname);
  void SetXOffsValue(int actNo, int patNo, int sprId, int val);
  void SetYOffsValue(int actNo, int patNo, int sprId, int val);
  void SetSprNoValue(int actNo, int patNo, int sprId, DWORD val);
  void SetXMagValue(int actNo, int patNo, int sprId, float val);
  void SetYMagValue(int actNo, int patNo, int sprId, float val);
  void SetRotValue(int actNo, int patNo, int sprId, DWORD val);
  void SetMirrorValue(int actNo, int patNo, int sprId, DWORD val);
  void SetABGRValue(int actNo, int patNo, int sprId, DWORD val);
  void SetSpTypeValue(int actNo, int patNo, int sprId, DWORD val);
  void SetExtXValue(int actNo, int patNo, int val);
  void SetExtYValue(int actNo, int patNo, int val);
  void GetOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo, int sprid);
  DWORD GetSprNoValue(int actNo, int patNo, int sprId);
  float GetXMagValue(int actNo, int patNo, int sprId);
  float GetYMagValue(int actNo, int patNo, int sprId);
  DWORD GetRotValue(int actNo, int patNo, int sprId);
  DWORD GetMirrorValue(int actNo, int patNo, int sprId);
  DWORD GetABGRValue(int actNo, int patNo, int sprId);
  void GetSprWHValue(int actNo, int patNo, int sprId, DWORD *sprW, DWORD *sprH);
  DWORD GetSpTypeValue(int actNo, int patNo, int sprId);
  void GetExtOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo);
  void MoveSprite(int xOffs, int yOffs, int actNo, int patNo, int sprid);
  void MoveExtSprite(int xOffs, int yOffs, int actNo, int patNo);
  void ChangeMagSprite(float xMagRatio, float yMagRatio, int actNo, int patNo, int sprid);
  void ChangeRotSprite(int diffRot, int actNo, int patNo, int sprid);
  void AddSpr(int actNo, int patNo, int sprNo);
  void SwapSpriteOrder(int actNo, int patNo, int spA, int spB);
  void DelSpr(int actNo, int patNo, int sprId);
  void AddPat(int actNo, int basePatNo);
  void SwapPat(int actNo, int patA, int patB);
  void DelPat(int actNo, int patNo);
  void SlideSprNo(int aftThis, bool plus);
  void CopyAction(int destActNo, int srcActNo);
  float GetInterval(int actNo);
  void SetInterval(int actNo, float val);
  int GetNumSounds(void);
  int GetSoundNo(int actNo, int patNo);
  void SetSoundNo(int actNo, int patNo, int sndNo);
  char *GetSoundFN(int no);
  bool AddSound(char *fname);
  void SwapSound(int noA, int noB);
  void DelSound(int no);
};

#endif // __MY_SPRITE__
