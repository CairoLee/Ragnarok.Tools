#ifndef __MY_MOD_SOUND__
#define __MY_MOD_SOUND__

#include <windows.h>
#include "../resource.h"

#include "../winmain.h"
#include "../../common/global.h"

extern HWND hwndMain;

extern void OpenDialog_ModifySound(void);


void MSSetInitial(HWND hwnd);
void MSRedrawList(HWND hwnd);
void MSProcAddLast(HWND hwnd);
void MSProcDelSel(HWND hwnd);
void MSProcUpSel(HWND hwnd);
void MSProcDownSel(HWND hwnd);
int MSGetCurSel(HWND hwnd);

INT_PTR CALLBACK DialogProcModSound(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_MOD_SOUND__
