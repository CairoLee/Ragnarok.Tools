#ifndef __MY_ABOUT__
#define __MY_ABOUT__

#include <windows.h>
#include "../resource.h"
#include "../winmain.h"
#include "../../common/global.h"

extern void OpenDialog_About(void);

INT_PTR CALLBACK DialogProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_ABOUT__