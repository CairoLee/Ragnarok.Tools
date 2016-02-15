#ifndef __MY_VSPLIT__
#define __MY_VSPLIT__

#include <windows.h>
#include "../winmain.h"
#include "../../common/global.h"

extern int vSplitPercent;
extern HWND hwndVSplit;

void BuildVSplit(HINSTANCE hInst);
void RepositionVSplit(RECT *rc);
void DrawSplitter(int y);

#endif  // __MY_VSPLIT__