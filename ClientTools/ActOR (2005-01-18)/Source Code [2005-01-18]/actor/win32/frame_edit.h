#ifndef __MY_FRAME_E__
#define __MY_FRAME_E__

#include <windows.h>
#include "resource.h"

#include "../../common/global.h"
#include "../actor.h"
#include "../winmain.h"

extern HWND hwndFrameE;

extern void CreateDialogFrameEdit(HINSTANCE hInst, HWND hwnd);

void FEDSetInitial(HWND hwnd);
void FEDSetInitial_Spin(void);
void FEDSpinProc(WPARAM wParam, HWND hwndscr);
void FEDCheckInt(WORD ID);
void FEDProcAny(void);
void FEDCheckMaxInEdit(void);

extern INT_PTR CALLBACK DialogProcFrameE(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_FRAME_E__
