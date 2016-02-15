#ifndef __MY_LABELS__
#define __MY_LABELS__

#include <windows.h>
#include "../winmain.h"
#include "../../common/global.h"

extern HWND hwndLabelDir;
extern HWND hwndLabelFrame;

void BuildLabels(HINSTANCE hInst);
void RepositionLabels(RECT *rc);
void OwnerDrawLabels(DRAWITEMSTRUCT *lpd);

#endif // __MY_LABELS__