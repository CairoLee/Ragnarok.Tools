#ifndef __MY_PRIORREFSPR__
#define __MY_PRIORREFSPR__

#include <windows.h>
#include "../resource.h"

#include "../winmain.h"
#include "../../common/global.h"

extern HWND hwndMain;

extern void OpenDialog_PriorRefSpr(void);

void PRFSetInitial(HWND hwnd);
void PRFProcRadioHead(HWND hwnd, WORD ID);
void PRFProcRadioBody(HWND hwnd, WORD ID);
void PRFProcRadioEtc(HWND hwnd, WORD ID);
void PRFProcRadioNeigh(HWND hwnd, WORD ID);

INT_PTR CALLBACK DialogProcPriorRefSpr(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_PRIORREFSPR__
