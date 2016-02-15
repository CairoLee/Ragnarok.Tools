#ifndef __MY_SCROLL__
#define __MY_SCROLL_

#include <windows.h>
#include "../winmain.h"
#include "../../common/global.h"

extern HWND hwndScr;
extern HWND hwndFrmImgScr;

void BuildScroll(HINSTANCE hInst);
void RepositionScroll(RECT *rc);
void OnHScroll(WPARAM wParam, LPARAM lParam);
int GetScrollImgPos(void);
void SetScrollPos_(HWND hwnd, int no, int max);
void OnHScrollFrameImage(WPARAM wParam);
void OnHScrollFrame(WPARAM wParam);

#endif  // __MY_SCROLL__