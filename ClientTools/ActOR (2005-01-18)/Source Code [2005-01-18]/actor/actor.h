#ifndef __MY_ACTOR__
#define __MY_ACTOR__

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "../common/global.h"
#include "../common/Sprite.h"
#include "../common/vec.h"

#define DRAG_VSPLIT 1
#define DRAG_TABLE_ROW 2
#define DRAG_ALL_S_PAN 3
#define DRAG_ALL_S_ZOOM 4
#define DRAG_ALL_S_ROT 5
#define DRAG_ALL_C_PAN 6
#define DRAG_ALL_C_ZOOM 7
#define DRAG_ALL_C_ROT 8
#define DRAG_IMAGE 9

#define REFHEAD_ACT "ref_head.act"
#define REFHEAD_SPR "ref_head.spr"
#define REFBODY_ACT "ref_body.act"
#define REFBODY_SPR "ref_body.spr"
#define REFETC_ACT "ref_etc.act"
#define REFETC_SPR "ref_etc.spr"
#define REFNEIGH_ACT "ref_neigh.act"
#define REFNEIGH_SPR "ref_neigh.spr"

#ifndef WIN32
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

typedef struct {
  LONG x, y;
} POINT;

typedef struct tagRGBQUAD {
  BYTE    rgbBlue; 
  BYTE    rgbGreen; 
  BYTE    rgbRed; 
  BYTE    rgbReserved; 
} RGBQUAD; 

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD bmiColors[1];
} BITMAPINFO;

#endif

#define COMTYPE_MOVE_XY 1
#define COMTYPE_MOVE_EXY 2
#define COMTYPE_MAG_XY 3
#define COMTYPE_ROT 4
#define COMTYPE_MIRROR 5
#define COMTYPE_SATURATION 6

#define NUMCOMMANDRECORD 50

typedef struct {
  int type;
  int actNo;
  int patNo;
  int sprId;
  union {
    int xyi[2];
    DWORD xyd[2];
    float xyf[2];
  };
} USERCOMMAND;

typedef struct {
  bool head;  // true: front,  false: back
  bool body;
  bool etc;
  bool neigh;
} RefSpr_Prior;

#define ACTOR_AUDIT_NOERROR 0
#define ACTOR_AUDIT_SPRMAXERROR -1
#define ACTOR_AUDIT_0205SPRWIDTHHEIGHTERROR -2
#define ACTOR_AUDIT_SOUNDMAXERROR -3

class CActor : public CSprite {
private:
  bool ReadSpr(char *filename);
  bool ReadBmp(char *filename);
  bool ReadAct(char *filename);
  void FindMinimumNoBmp(int *minno, char *basefn, char *filename);
  int FindMinimumNoBmp_GetNo(char *filename, int offs);
  bool ReplacePallet(FILE *fp);
  bool AddPalletImg(FILE *fp);
  bool AddFlatImg(FILE *fp);
  void MakeFlatImgFromBmp(DWORD **dat, BYTE **pdat, BYTE *fdat, int *w, int *h);
  void MakeFlatImgFromTga(DWORD **dat, BYTE *fdat, int *w, int *h);
  bool Check256Bitmap(BYTE *dat);
  bool Check32BitTga(BYTE *dat);
  void SlideSprNo(int aftThis, bool plus);

  void CheckRefHead(char *dir);
  void CheckRefBody(char *dir);
  void CheckRefEtc(char *dir);
  void CheckRefNeigh(char *dir);
  void CheckRefXXXX(CSprite **ref, bool *enable, bool *match, char *dir, char *actn, char *sprn);

  void DrawFrameAll_HVLine(void);
  void DrawFrameAll_Reference(bool front);
  void DrawFrameAll_Marker(int *id, int numID);
  void DrawFrameAll_WithBorder(int *id, int numID);

  void CameraSet(CAM_STATE *camera, GLfloat ratio);
  void GetViewableStartEndImgNo(int *startNo, int *endNo, int no);

  void DoCurUserCommand(USERCOMMAND *ncmd);

  bool AuditCheckSprMax(int *actNo, int *patNo, int *sprId);
  bool AuditCheck0x0205SprWidthHeight(int *actNo, int *patNo, int *sprId);
  bool AuditCheckSoundMax(int *actNo, int *patNo);

public:
  CActor();
  virtual ~CActor();
  
  bool drawStop;
  int curAct, curPat, curDir;
  int curImg;
  CSprite *sprRefHead;
  CSprite *sprRefBody;
  CSprite *sprRefEtc;
  CSprite *sprRefNeigh;
  RefSpr_Prior  prioRefSpr;
  bool enableRefHead;
  bool enableRefBody;
  bool enableRefEtc;
  bool enableRefNeigh;
  bool matchRefHead;
  bool matchRefBody;
  bool matchRefEtc;
  bool matchRefNeigh;
  POINT pointNeigh;
  int widthAll;
  int heightAll;
  GLfloat ratioAll;
  CAM_STATE cameraAll;
  int widthImage;
  int heightImage;
  GLfloat ratioFrmImg;
  CAM_STATE cameraFrmImg;
  DWORD colorBG, colorSelect, colorLine;
  bool drawHLine;
  bool drawVLine;

  POINT firstMP, oldMP, newMP;
  int dragState;

  USERCOMMAND usrcmd[ NUMCOMMANDRECORD ];
  bool validPrevCommand;
  int startRec;
  int curRec;
  int endRec;

  void SetColors(DWORD BG, DWORD Select, DWORD Line);
  bool ReadFileSPRorBMPAndAct(char *filename);
  void CheckIfAddSuffix(char *filename, int maxLen, char *suffix);

  void ChangeNeighborPoint(WORD ID);
  void CheckReferenceAny(char *dirFirst, char *dirSecond);

  bool ReplaceImage(char *filename);
  void InsertImage(int id, char *filename);
  void DeleteCurrentImage(void);

  void SetupOpenGL(int width, int height);

  void DrawSprite(int x, int y, int no, bool withBorder, DWORD colorBorder);
  void DrawFrameAll(int *id, int numID);
  void SetImages(int no);
  int GetClickImgNo(int pointX);

  void SpritePan(BYTE key, int *id, int numID);
  void SpriteZoom(int id, bool equalRatio);
  void SpriteRot(int id);
  void CameraPan(void);
  void CameraZoom(void);

  void StoreUserCommand(USERCOMMAND *newcmd);
  void DoPrevUserCommand(void);
  void DoNextUserCommand(void);
  void SetInvalidPrevCommand(void);

  int AuditAll(int *actNo, int *patNo, int *sprId);
  void ModifyCorrectSprWidthHeight(void);

  int GetTimeInterval(void);
};

#endif // __MY_ACTOR__
