#ifndef __MY_ACT__
#define __MY_ACT__

#include "global.h"

typedef struct {
  WORD magic;
  WORD version;
  WORD numOfAct;
  WORD xxx4;
  DWORD xxx5;
  DWORD xxx6;
} ActHeader;

typedef struct {
  DWORD pal1;
  DWORD pal2;
  DWORD xxx3;
  DWORD xxx4;
  DWORD xxx5;
  DWORD xxx6;
  DWORD xxx7;
  DWORD xxx8;
} PatHeader;

typedef struct {
  int xOffs;
  int yOffs;
  DWORD sprNo;
  DWORD mirrorOn;
} AnyPatSprV0101;

typedef struct {
  int xOffs;
  int yOffs;
  DWORD sprNo;
  DWORD mirrorOn;
  DWORD color;
  float xyMag;
  DWORD rot;
  DWORD spType;
} AnyPatSprV0201;

typedef struct {
  int xOffs;
  int yOffs;
  DWORD sprNo;
  DWORD mirrorOn;
  DWORD color;
  float xMag;
  float yMag;
  DWORD rot;
  DWORD spType;
} AnyPatSprV0204;

typedef struct {
  int xOffs;
  int yOffs;
  DWORD sprNo;
  DWORD mirrorOn;
  DWORD color;
  float xMag;
  float yMag;
  DWORD rot;
  DWORD spType;
  DWORD sprW;
  DWORD sprH;
} AnyPatSprV0205;

typedef struct {
  PatHeader ph;
  DWORD numOfSpr;
  union {
    AnyPatSprV0101 *aps0;
    AnyPatSprV0201 *aps1;
    AnyPatSprV0204 *aps4;
    AnyPatSprV0205 *aps5;
  };
  int sndNo;
  DWORD numxxx;
  DWORD Ext1;
  int ExtXoffs;
  int ExtYoffs;
  DWORD terminate;
} AnyActAnyPat;

typedef struct {
  int numOfPat;
  AnyActAnyPat *aaap;
} AnyAction;

typedef struct {
  ActHeader ah;
  AnyAction *aa;
  int numOfSound;
  char **fnameSnd;
  float *interval;
} ActAll;

class CACT
{
private:
  bool ParseActData(BYTE *dat);
  bool ParseActData_Header(BYTE *dat);
  void ParseActData_AnyAct(BYTE **dat, int no);
  void ParseActData_AnyPatAnySpr(BYTE **dat, int actNo, int patNo, int sprNo, int version);
  void MemAllocAnyPatAnySpr(int no, int spr, int version);
  void ParseActData_Sound(BYTE **dat);
  void ParseActData_Interval(BYTE *dat);

  INLINE void Swap(int *a, int *b);
  INLINE void Swap(DWORD *a, DWORD *b);
  INLINE void Swap(float *a, float *b);
  INLINE void SwapSpriteOrderV0101(int actNo, int patNo, int spA, int spB);
  INLINE void SwapSpriteOrderV0201(int actNo, int patNo, int spA, int spB);
  INLINE void SwapSpriteOrderV0204(int actNo, int patNo, int spA, int spB);
  INLINE void SwapSpriteOrderV0205(int actNo, int patNo, int spA, int spB);

  void NormalizeRot(DWORD *rot);

  void AddSprV0101(AnyActAnyPat *aaap1, int sprNo, int spType);
  void AddSprV0201(AnyActAnyPat *aaap1, int sprNo, int spType);
  void AddSprV0204(AnyActAnyPat *aaap1, int sprNo, int spType);
  void AddSprV0205(AnyActAnyPat *aaap1, int sprNo, int spType, int spWidth, int spHeight);

  void CopyAnyActAnyPat(AnyActAnyPat *to, AnyActAnyPat *from);

  int CorrectNoInsDelPoint(int no);

public:
  ActAll aall;
  bool modified;

  CACT(BYTE *dat);
  virtual ~CACT(void);

  bool GetModified(void);
  void SetModified(void);
  WORD GetVersion(void);
  int GetNumAction(void);
  int GetNumPattern(int actNo);
  int GetNumSprites(int actNo, int patNo);

  void WriteAct(char *fname);
  void WriteActText(char *fname);

  void GetOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo, int sprid);
  void GetSprNoValue(int actNo, int patNo, int sprId, int *sprNo);
  float GetXMagValue(int actNo, int patNo, int sprId);
  float GetYMagValue(int actNo, int patNo, int sprId);
  DWORD GetRotValue(int actNo, int patNo, int sprId);
  DWORD GetMirrorValue(int actNo, int patNo, int sprId);
  DWORD GetABGRValue(int actNo, int patNo, int sprId);
  void GetSprWHValue(int actNo, int patNo, int sprId, DWORD *sprW, DWORD *sprH);
  void GetSpTypeValue(int actNo, int patNo, int sprId, int *spType);
  void GetExtOffsPoint(int *xOffs, int *yOffs, int actNo, int patNo);

  void SetXOffsValue(int actNo, int patNo, int sprId, int val);
  void SetYOffsValue(int actNo, int patNo, int sprId, int val);
  void SetSprNoValue(int actNo, int patNo, int sprId, DWORD val, DWORD sprW, DWORD sprH);
  void SetXMagValue(int actNo, int patNo, int sprId, float val);
  void SetYMagValue(int actNo, int patNo, int sprId, float val);
  void SetRotValue(int actNo, int patNo, int sprId, DWORD val);
  void SetMirrorValue(int actNo, int patNo, int sprId, DWORD val);
  void SetABGRValue(int actNo, int patNo, int sprId, DWORD val);
  void SetSpTypeValue(int actNo, int patNo, int sprId, DWORD val);
  void SetExtXValue(int actNo, int patNo, int val);
  void SetExtYValue(int actNo, int patNo, int val);

  void MoveSprite(int xOffs, int yOffs, int actNo, int patNo, int sprid);
  void MoveExtSprite(int xOffs, int yOffs, int actNo, int patNo);
  void ChangeMagSprite(float xMagRatio, float yMagRatio, int actNo, int patNo, int sprid);
  void ChangeRotSprite(int diffRot, int actNo, int patNo, int sprid);

  void AddSpr(int actNo, int patNo, int sprNo, int spType, int spWidth, int spHeight);
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

  void InsertNullAct8(int no);
  void DeleteAct8(int no);
};

#endif // __MY_ACT__
