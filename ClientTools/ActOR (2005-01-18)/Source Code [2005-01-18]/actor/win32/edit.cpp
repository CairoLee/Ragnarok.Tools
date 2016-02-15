#include "edit.h"


HWND hwndEditInter;
WNDPROC wndprocOrigEditInter;

void BuildEdits(HINSTANCE hInst)
{
  hwndEditInter = CreateWindow("RichEdit", "", WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_EDITINTER, hInst, NULL);
  wndprocOrigEditInter = (WNDPROC)GetWindowLong(hwndEditInter, GWL_WNDPROC); 
  SetWindowLong(hwndEditInter, GWL_WNDPROC, (LONG)WndProcEditInter);
}

void RepositionEdits(RECT *rc)
{
  MoveWindow(hwndEditInter, rc->right-305, 158, 80, 15, TRUE);
}

void SetEditInterval(int inter)
{
  char tmp[20];
  SNPRINTF(tmp, sizeof(tmp), "%d", inter);
  SetWindowText(hwndEditInter, tmp);
}

void SetIntervalValue(void)
{
  char tmp[20];
  GetWindowText(hwndEditInter, tmp, sizeof(tmp));
  actor->SetInterval(actor->curAct, (float)atof(tmp));
}

LRESULT CALLBACK WndProcEditInter(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      HFONT origFont;
      hdc = BeginPaint(hwnd, &ps);
      char buf[128];
      GetWindowText(hwnd, buf, sizeof(buf));
      origFont = (HFONT)SelectObject(hdc, (HGDIOBJ)defFont);
      RECT rc;
      rc.top = ps.rcPaint.top + 1;
      rc.left = ps.rcPaint.left + 1;
      rc.bottom = ps.rcPaint.bottom - 1;
      rc.right = ps.rcPaint.right - 1;
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOWTEXT+1));
      if (IsWindowEnabled(hwnd)) {
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
      } else {
        FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE+1));
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
      }
      TextOut(hdc, 2, 2, buf, (int)strlen(buf));
      SelectObject(hdc, (HGDIOBJ)origFont);
      EndPaint(hwnd, &ps);
      return 0;
    }
  case WM_KEYUP:
    if (wParam == VK_RETURN) {
      SetIntervalValue();
    }
    break;
  case WM_KILLFOCUS:
    SetIntervalValue();
    break;
  case WM_CLOSE:
    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)wndprocOrigEditInter);
    DestroyWindow(hwnd);
    break;
  }
  return CallWindowProc(wndprocOrigEditInter, hwnd, uMsg, wParam, lParam);
}
