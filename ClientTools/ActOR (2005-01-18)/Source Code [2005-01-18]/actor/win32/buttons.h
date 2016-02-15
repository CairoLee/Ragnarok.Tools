#ifndef __MY_BUTTONS__
#define __MY_BUTTONS__

#include <windows.h>

#include "../winmain.h"
#include "labels.h"

extern HWND hwndDirec0;
extern HWND hwndDirec1;
extern HWND hwndDirec2;
extern HWND hwndDirec3;
extern HWND hwndDirec4;
extern HWND hwndDirec5;
extern HWND hwndDirec6;
extern HWND hwndDirec7;
extern HWND hwndPlay;
extern HWND hwndStop;
extern bool autoPlay;
extern HWND hwndViewReset;
extern HWND hwndDirN0;
extern HWND hwndDirN1;
extern HWND hwndDirN2;
extern HWND hwndDirN3;
extern HWND hwndDirN4;
extern HWND hwndDirN5;
extern HWND hwndDirN6;
extern HWND hwndDirN7;

void BuildButtons(HINSTANCE hInst);
void RepositionButtons(RECT *rc);
void OwnerDrawButtons(DRAWITEMSTRUCT *lpd);
void SetStateDirecButton(BYTE stat);
void OnBtnDirectClick(WORD ID);
void SetStateAutoPlay(void);
extern void OnBtnPlay(void);
extern void OnBtnStop(void);
void OnBtnViewReset(void);
void EnableNeighDirWindow(BOOL sw);
void OnBtnNeiDirec(WORD ID);

#endif  // __MY_BUTTONS__
