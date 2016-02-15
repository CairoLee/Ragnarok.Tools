#ifndef __MY_ACT_COPY__
#define __MY_ACT_COPY__

#include <windows.h>
#include "resource.h"

#include "../../common/global.h"
#include "../actor.h"
#include "../winmain.h"

extern HWND hwndFrameAC;

extern void CreateDialogActCopy(HINSTANCE hInst, HWND hwnd);

void ACDSetInitial(HWND hwnd);
void ACDSetInitial_Combo(HWND hwnd);
void ACDSetInitial_Radio(HWND hwnd, int actno, bool isfrom);
void ACDDetailComboChange(HWND hwnd, WORD ID);
void ACDRadioExclusive(HWND hwnd, WORD ID, bool isfrom);
void ACDProcAny(HWND hwnd);
void ACDProcCopy4to8(HWND hwnd, bool cw);
void ACDProcCopyDetail(HWND hwnd);

extern INT_PTR CALLBACK DialogProcActCopy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif  // __MY_ACT_COPY__
