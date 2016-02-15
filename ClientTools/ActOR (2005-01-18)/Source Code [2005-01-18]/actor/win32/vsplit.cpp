#include "vsplit.h"

//vsplitter
int vSplitPercent;
HWND hwndVSplit;

void BuildVSplit(HINSTANCE hInst)
{
  vSplitPercent = 70;
  hwndVSplit = CreateWindow("STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 0, 0, 1, 1, hwndMain, (HMENU)IDC_VSPLIT, hInst, NULL);
}

void RepositionVSplit(RECT *rc)
{
  MoveWindow(hwndVSplit, 0, rc->bottom * vSplitPercent / 100, rc->right, 2, TRUE);
}

void DrawSplitter(int y)
{
  POINT sp;
  sp.x = 0;
  sp.y = y;
  ClientToScreen(hwndMain, &sp);
  RECT rc;
  GetWindowRect(hwndMain, &rc);
  HDC hdc;

  if (winVersion > 4) {
    hdc = GetDCEx(GetDesktopWindow(),NULL, DCX_LOCKWINDOWUPDATE);
  } else {
    // ignore Win NT4. If you are NT4 user, use upper GetDCEx.
    hdc = GetDCEx(GetDesktopWindow(),NULL,DCX_PARENTCLIP | DCX_LOCKWINDOWUPDATE);
  }
  PatBlt(hdc, rc.left, sp.y, rc.right - rc.left, 3, DSTINVERT);
  ReleaseDC(GetDesktopWindow(), hdc);
}
