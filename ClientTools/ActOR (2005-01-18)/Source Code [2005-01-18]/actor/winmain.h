#ifndef __MY_WINMAIN__
#define __MY_WINMAIN__

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <libintl.h>
#include <iconv.h>

#include "../common/global.h"
#include "actor.h"
#include "../common/vec.h"
#include "win32/table.h"
#include "win32/buttons.h"
#include "win32/checkbox.h"
#include "win32/combobox.h"
#include "win32/frames.h"
#include "win32/scroll.h"
#include "win32/mbutton_assign.h"
#include "win32/frame_edit.h"
#include "win32/about.h"
#include "win32/act_copy.h"
#include "win32/priorrefspr.h"
#include "win32/edit.h"
#include "win32/mod_sound.h"

extern CActor *actor;
extern HWND hwndMain;
extern HFONT defFont;
extern DWORD winVersion;
extern MOUSE_ASSIGN mouseAssign;
extern DWORD colorBG, colorSelect, colorLine;

void BuildScreen(HWND hwnd);
void RepositionControls(void);
void DestructScreen(void);

void AuditAll(void);

void OpenDialog_ReadFile(char *definedFilename);
void WriteAct(void);
void WriteActText(void);
void WriteBmp(void);
void WriteSpr(void);
void ReplaceImage(void);
void Write1Image(void);
void InsertImage(bool before);
void DeleteImage(void);
void SetSameColorUnusedPal(void);
void SpritePan(BYTE key);
void SpriteZoom(bool equalRatio);
void SpriteRot(void);
void CameraPan(void);
void CameraZoom(void);
void SelectColor(WORD ID);
void ChangeStateHLine(void);
void ChangeStateVLine(void);
void Undo(void);
void Redo(void);
bool OnClose_Save(void);
void ShowPopupFrameImage(LPARAM lParam);

bool IsSprReady(void);
bool IsActReady(void);
void ProcAfterReadSPR_ACT(void);
void CheckReference(void);
void ProcPatternChange(bool drawAll);

void UndoRedoInit(void);
void DispUndoInitWarningFE(void);
bool DispUndoInitWarningSPO(void);
void UndoEnable(bool enable);
void RedoEnable(bool enable);
void BeginSpritePan(void);
void EndSpritePan(void);
void StoreSpritePan(void);
void BeginSpriteMag(void);
void EndSpriteMag(void);
void StoreSpriteMag(void);
void BeginSpriteRot(void);
void EndSpriteRot(void);
void StoreSpriteRot(void);
void BeginSpriteMirror(void);
void EndSpriteMirror(void);
void StoreSpriteMirror(void);
void BeginSpriteSaturation(void);
void EndSpriteSaturation(void);
void StoreSpriteSaturation(void);

bool GetFileName(char *fn, int fnlen, char *title, char *filter, bool open);
bool CheckInRect(HWND hwnd, POINT *p);
void SetMousePoint(LPARAM lParam);

void OnLButtonDown(WPARAM wParam, LPARAM lParam);
void OnLButtonUp(WPARAM wParam, LPARAM lParam);
void OnRButtonDown(WPARAM wParam, LPARAM lParam);
void OnRButtonUp(WPARAM wParam, LPARAM lParam);
void OnMButtonDown(WPARAM wParam, LPARAM lParam);
void OnMButtonUp(WPARAM wParam, LPARAM lParam);
void OnMouseMove(WPARAM wParam, LPARAM lParam);
void OnKeyUp(WPARAM wParam, LPARAM lParam);
void OnSysKeyUp(WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcFrameImage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK TimerFrameAll(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void ChangeMenuString(void);
void SetTextDomain(char *progname);

bool GetAgreement(void);
void GetPrefFileName(char *fname, int fname_size);
void ReadPreference(void);
void WritePreference(void);

#define TIMER_TABLEEDIT 101
#define TIMER_FRAMEALL 102

#define IDC_STAT_A 1000
#define IDC_STATIC 1001
#define IDC_FRAMEIMG 1002
#define IDC_VSPLIT 1003
#define IDC_TABLE 1004
#define IDC_CBACT 1005
#define IDC_BTNDI0 1010
#define IDC_BTNDI1 1011
#define IDC_BTNDI2 1012
#define IDC_BTNDI3 1013
#define IDC_BTNDI4 1014
#define IDC_BTNDI5 1015
#define IDC_BTNDI6 1016
#define IDC_BTNDI7 1017
#define IDC_SCROLL 1020
#define IDC_SCROLLI 1021
#define IDC_BTNPLAY 1031
#define IDC_BTNSTOP 1032
#define IDC_BTNVIEWRESET 1033
#define IDC_BTNHEAD 1041
#define IDC_BTNBODY 1042
#define IDC_BTNETC 1043
#define IDC_BTNNEI 1044
#define IDC_FRAMEALL 1051
#define IDC_BTNDN0 1060
#define IDC_BTNDN1 1061
#define IDC_BTNDN2 1062
#define IDC_BTNDN3 1063
#define IDC_BTNDN4 1064
#define IDC_BTNDN5 1065
#define IDC_BTNDN6 1066
#define IDC_BTNDN7 1067

#endif // __MY_WINMAIN__
