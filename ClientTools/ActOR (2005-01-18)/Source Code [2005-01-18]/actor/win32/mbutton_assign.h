#ifndef __MY_MBUTTON__
#define __MY_MBUTTON__

#include <windows.h>
#include "../resource.h"

#include "../winmain.h"
#include "../../common/global.h"

extern MOUSE_ASSIGN mouseAssign;
extern HWND hwndMain;

extern void OpenDialog_MouseButtonAssign(void);

void MBASetInitial(HWND hwnd);
void MBAProcRadio(HWND hwnd, WORD ID);
void MBASetState(HWND hwnd);

INT_PTR CALLBACK DialogProcMBAssign(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_MBUTTON__
