#ifndef __MY_CHRVIEW_WIN32__
#define __MY_CHRVIEW_WIN32__

#include <windows.h>
#include "../resource.h"
#include "../chrview.h"
#include "../../common/pak.h"

#define CHRV_TIMER1 100

extern int CharacterViewer(int argc, char **argv);

void ExitCharacterViewer(HWND hwndDlg, int ID);

//related NFD
void ChrVInitNFD(HWND hwndDlg);

//related OpenGL
void ChrVInitOpenGL(HWND hwndDlg);
bool ChrVCheckInGLArea(HWND hwndDlg, POINT *mp_scrn);
void ChrVGLZoom(void);
void ChrVGLPan(void);

//related sprite
bool ChrVSpriteSetPCBody(HWND hwndDlg, CSprite **sprite, char *dispname);
bool ChrVSpriteSetPCHead(HWND hwndDlg, CSprite **sprite);
bool ChrVSpriteSetPCAcc(HWND hwndDlg, CSprite **sprite, char *dispname, int no);
bool ChrVSpriteSetPCWeapon(HWND hwndDlg, CSprite **sprite);
bool ChrVSpriteSetPCShield(HWND hwndDlg, CSprite **sprite);
void ChrVDrawSprites(void);

//related ComboBox
void ChrVSelectCombo(HWND hwndDlg, WORD ID);
void ChrVSelectComboBody(HWND hwndDlg);
void ChrVSelectComboBody_PC(HWND hwndDlg);
void ChrVSelectComboAct(HWND hwndDlg);
void ChrVSelectComboHead(HWND hwndDlg);
void ChrVSelectComboAcc(HWND hwndDlg, int no);
void ChrVSelectComboWeapon(HWND hwndDlg);
void ChrVSelectComboShield(HWND hwndDlg);

void ChrVSetScrollBar(HWND hwndDlg, int min, int max, int pos);
void ChrVProcBTNPushDirection(HWND hwndDlg, WORD ID);

//related initialize controls
void ChrVInitControls(HWND hwndDlg);
void ChrVInitControlsSub(HWND hwndDlg);
void ChrVInitControlsBody(HWND hwndDlg);
void ChrVInitControlsBodyMob(HWND hwndDlg);
void ChrVInitControlsBodyPC(HWND hwndDlg);
void ChrVInitControlsBodyNPC(HWND hwndDlg);
void ChrVInitControlsHead(HWND hwndDlg);
void ChrVInitControlsWeaponShieldPre(HWND hwndDlg);
void ChrVInitControlsWeaponShield(HWND hwndDlg);
void ChrVInitControlsAccPC(HWND hwndDlg);
void ChrVInitControlsActPC(HWND hwndDlg);
void ChrVInitControlsActMob(HWND hwndDlg);
void ChrVInitControlsActNPC(HWND hwndDlg);

void ChrVEnableComboAlmost(HWND hwndDlg, bool flag);
void ChrVGetPCBodyName(HWND hwndDlg, char *bodyname, char *sexname);

void ChrVReadWav(void);
void ChrVAllocWavMemory(int num);
void ChrVFreeWavMemory(void);

void ChrVSetBGColor(HWND hwndDlg);

INT_PTR CALLBACK DialogProcChrV(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CALLBACK ChrVTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void ChrVWMHScroll(HWND hwnd, WPARAM wParam);

#endif //__MY_CHRVIEW_WIN32__

