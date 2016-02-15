#include "buttons.h"

//direction
HWND hwndDirec0;
HWND hwndDirec1;
HWND hwndDirec2;
HWND hwndDirec3;
HWND hwndDirec4;
HWND hwndDirec5;
HWND hwndDirec6;
HWND hwndDirec7;

//frame play
HWND hwndPlay;
HWND hwndStop;
bool autoPlay;

//view reset
HWND hwndViewReset;

//neighbor direction
HWND hwndDirN0;
HWND hwndDirN1;
HWND hwndDirN2;
HWND hwndDirN3;
HWND hwndDirN4;
HWND hwndDirN5;
HWND hwndDirN6;
HWND hwndDirN7;

void BuildButtons(HINSTANCE hInst)
{
  //direction
  hwndDirec0 = CreateWindow("BUTTON", _("|"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI0, hInst, NULL);
  hwndDirec1 = CreateWindow("BUTTON", _("/"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI1, hInst, NULL);
  hwndDirec2 = CreateWindow("BUTTON", _("-"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI2, hInst, NULL);
  hwndDirec3 = CreateWindow("BUTTON", _("\\"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI3, hInst, NULL);
  hwndDirec4 = CreateWindow("BUTTON", _("|"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI4, hInst, NULL);
  hwndDirec5 = CreateWindow("BUTTON", _("/"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI5, hInst, NULL);
  hwndDirec6 = CreateWindow("BUTTON", _("-"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI6, hInst, NULL);
  hwndDirec7 = CreateWindow("BUTTON", _("\\"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDI7, hInst, NULL);
  //frame play
  autoPlay = false;
  hwndPlay = CreateWindow("BUTTON", _("play"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNPLAY, hInst, NULL);
  hwndStop = CreateWindow("BUTTON", _("stop"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNSTOP, hInst, NULL);
  SendMessage(hwndStop, BM_SETSTATE, TRUE, 0);
  //view reset
  hwndViewReset = CreateWindow("BUTTON", "view reset", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNVIEWRESET, hInst, NULL);
  //neighbor direction
  hwndDirN0 = CreateWindow("BUTTON", _("|"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN0, hInst, NULL);
  hwndDirN1 = CreateWindow("BUTTON", _("/"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN1, hInst, NULL);
  hwndDirN2 = CreateWindow("BUTTON", _("-"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN2, hInst, NULL);
  hwndDirN3 = CreateWindow("BUTTON", _("\\"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN3, hInst, NULL);
  hwndDirN4 = CreateWindow("BUTTON", _("|"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN4, hInst, NULL);
  hwndDirN5 = CreateWindow("BUTTON", _("/"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN5, hInst, NULL);
  hwndDirN6 = CreateWindow("BUTTON", _("-"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN6, hInst, NULL);
  hwndDirN7 = CreateWindow("BUTTON", _("\\"), WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNDN7, hInst, NULL);
}

void RepositionButtons(RECT *rc)
{
  //direction
  MoveWindow(hwndDirec0, rc->right-245, 67, 14, 14, TRUE);
  MoveWindow(hwndDirec1, rc->right-264, 67, 14, 14, TRUE);
  MoveWindow(hwndDirec2, rc->right-264, 49, 14, 14, TRUE);
  MoveWindow(hwndDirec3, rc->right-264, 31, 14, 14, TRUE); 
  MoveWindow(hwndDirec4, rc->right-245, 31, 14, 14, TRUE); 
  MoveWindow(hwndDirec5, rc->right-226, 31, 14, 14, TRUE); 
  MoveWindow(hwndDirec6, rc->right-226, 49, 14, 14, TRUE);
  MoveWindow(hwndDirec7, rc->right-226, 67, 14, 14, TRUE);
  //view reset
  MoveWindow(hwndViewReset, rc->right-366-70, 0, 70, 18, TRUE);
  //frame play
  MoveWindow(hwndPlay, rc->right-296, 95, 42, 18, TRUE);
  MoveWindow(hwndStop, rc->right-248, 95, 42, 18, TRUE);
  //neighbor direction
  MoveWindow(hwndDirN0, rc->right- 81, 157, 14, 14, TRUE);
  MoveWindow(hwndDirN1, rc->right-100, 157, 14, 14, TRUE);
  MoveWindow(hwndDirN2, rc->right-100, 139, 14, 14, TRUE);
  MoveWindow(hwndDirN3, rc->right-100, 121, 14, 14, TRUE); 
  MoveWindow(hwndDirN4, rc->right- 81, 121, 14, 14, TRUE); 
  MoveWindow(hwndDirN5, rc->right- 62, 121, 14, 14, TRUE); 
  MoveWindow(hwndDirN6, rc->right- 62, 139, 14, 14, TRUE);
  MoveWindow(hwndDirN7, rc->right- 62, 157, 14, 14, TRUE);
}

void OwnerDrawButtons(DRAWITEMSTRUCT *lpd)
{
  HFONT origFont;
  HBRUSH origBrush;
  HPEN origPen;
  HPEN curPen;
  char buf[256];

  switch (lpd->CtlID) {
  case IDC_BTNPLAY:
  case IDC_BTNSTOP:
  case IDC_BTNVIEWRESET:
    GetWindowText(lpd->hwndItem, buf, sizeof(buf));
    origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)defFont);
    origBrush = (HBRUSH)SelectObject(lpd->hDC, (HBRUSH)(COLOR_3DFACE+1));
    curPen = CreatePen(PS_SOLID, 0, RGB(0xFF, 0xFF, 0xFF));
    origPen = (HPEN)SelectObject(lpd->hDC, curPen);
    if (lpd->itemState & ODS_SELECTED) {
      FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_3DFACE+1));
      MoveToEx(lpd->hDC, lpd->rcItem.right-1, 0, NULL);
      LineTo(lpd->hDC, lpd->rcItem.right-1, lpd->rcItem.bottom-1);
      LineTo(lpd->hDC, 0, lpd->rcItem.bottom-1);
      DeleteObject(curPen);
      curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW));
      SelectObject(lpd->hDC, (HGDIOBJ)curPen);
      LineTo(lpd->hDC, 0, 0);
      LineTo(lpd->hDC, lpd->rcItem.right-1, 0);
      DeleteObject(curPen);
      curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
      SelectObject(lpd->hDC, (HGDIOBJ)curPen);
      MoveToEx(lpd->hDC, lpd->rcItem.right-3, 1, NULL);
      LineTo(lpd->hDC, 1, 1);
      LineTo(lpd->hDC, 1, lpd->rcItem.bottom-1);
      TextOut(lpd->hDC, 9, 4, buf, (int)strlen(buf));
    } else {
      FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_3DFACE+1));
      MoveToEx(lpd->hDC, 0, lpd->rcItem.bottom-2, NULL);
      LineTo(lpd->hDC, 0, 0);
      LineTo(lpd->hDC, lpd->rcItem.right-1, 0);
      DeleteObject(curPen);
      curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW));
      SelectObject(lpd->hDC, (HGDIOBJ)curPen);
      LineTo(lpd->hDC, lpd->rcItem.right-1, lpd->rcItem.bottom-1);
      LineTo(lpd->hDC, -1, lpd->rcItem.bottom-1);
      DeleteObject(curPen);
      curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
      SelectObject(lpd->hDC, (HGDIOBJ)curPen);
      MoveToEx(lpd->hDC, 1, lpd->rcItem.bottom-2, NULL);
      LineTo(lpd->hDC, lpd->rcItem.right-2, lpd->rcItem.bottom-2);
      LineTo(lpd->hDC, lpd->rcItem.right-2, 0);
      TextOut(lpd->hDC, 9, 4, buf, (int)strlen(buf));
    }
    DeleteObject(curPen);
    SelectObject(lpd->hDC, (HGDIOBJ)origPen);
    SelectObject(lpd->hDC, (HGDIOBJ)origBrush);
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    break;
  }
}

void SetStateDirecButton(BYTE stat)
{
  (stat & 0x01) ? EnableWindow(hwndDirec0, TRUE) : EnableWindow(hwndDirec0, FALSE);
  (stat & 0x02) ? EnableWindow(hwndDirec1, TRUE) : EnableWindow(hwndDirec1, FALSE);
  (stat & 0x04) ? EnableWindow(hwndDirec2, TRUE) : EnableWindow(hwndDirec2, FALSE);
  (stat & 0x08) ? EnableWindow(hwndDirec3, TRUE) : EnableWindow(hwndDirec3, FALSE);
  (stat & 0x10) ? EnableWindow(hwndDirec4, TRUE) : EnableWindow(hwndDirec4, FALSE);
  (stat & 0x20) ? EnableWindow(hwndDirec5, TRUE) : EnableWindow(hwndDirec5, FALSE);
  (stat & 0x40) ? EnableWindow(hwndDirec6, TRUE) : EnableWindow(hwndDirec6, FALSE);
  (stat & 0x80) ? EnableWindow(hwndDirec7, TRUE) : EnableWindow(hwndDirec7, FALSE);
}

void OnBtnDirectClick(WORD ID)
{
  if (IsActReady() == false)  return;

  bool tmpPlay = autoPlay;
  if (tmpPlay) {
    OnBtnStop();
  }

  UnSelectSelection();
  if (actor->GetNumAction() >= 8) {
    int i = (int)SendMessage(hwndAct, CB_GETCURSEL, 0, 0);
    actor->curAct = i*8 + (ID - IDC_BTNDI0);
  } else {
    switch (actor->GetNumAction()) {
    case 2:
      if (ID == IDC_BTNDI1) {
        actor->curAct = 0;
      } else {
        actor->curAct = 1;
      }
      break;
    case 4:
      switch (ID) {
      case IDC_BTNDI1:
        actor->curAct = 0;
        break;
      case IDC_BTNDI3:
        actor->curAct = 1;
        break;
      case IDC_BTNDI5:
        actor->curAct = 2;
        break;
      case IDC_BTNDI7:
        actor->curAct = 3;
        break;
      }
      break;
    }
  }
  actor->curDir = ID - IDC_BTNDI0;
  InvalidateRect(hwndLabelDir, NULL, TRUE);
  actor->curPat = 0;
  if (GetWindowLong(hwndFrameE, GWL_STYLE) & WS_VISIBLE) {
    ShowWindow(hwndFrameE, SW_HIDE);
    ShowWindow(hwndFrameE, SW_SHOW);
  }

  if (tmpPlay) {
    OnBtnPlay();
  }

  ProcPatternChange(true);
  SetFocus(hwndMain);
}

void SetStateAutoPlay(void)
{
  if (autoPlay) {
    EnableWindow(hwndTable, FALSE);
    EnableWindow(hwndEditInter, FALSE);
    EnableWindow(hwndSnd, FALSE);
  } else {
    EnableWindow(hwndTable, TRUE);
    EnableWindow(hwndEditInter, TRUE);
    EnableWindow(hwndSnd, TRUE);
  }
}

void OnBtnPlay(void)
{
  LRESULT ret;

  ret = SendMessage(hwndStop, BM_GETSTATE, 0, 0);
  if (ret == BST_PUSHED) {
    SendMessage(hwndStop, BM_SETSTATE, FALSE, 0);
  }
  SendMessage(hwndPlay, BM_SETSTATE, TRUE, 0);
  if (actor->IsActReady()) {
    autoPlay = true;
    SetStateAutoPlay();
    SetTimer(hwndMain, TIMER_FRAMEALL, actor->GetTimeInterval(), TimerFrameAll);
  }
  SetFocus(hwndMain);
}

void OnBtnStop(void)
{
  LRESULT ret;

  ret = SendMessage(hwndPlay, BM_GETSTATE, 0, 0);
  if (ret == BST_PUSHED) {
    SendMessage(hwndPlay, BM_SETSTATE, FALSE, 0);
  }
  SendMessage(hwndStop, BM_SETSTATE, TRUE, 0);
  autoPlay = false;
  SetStateAutoPlay();
  KillTimer(hwndMain, TIMER_FRAMEALL);
  SetFocus(hwndMain);
}

void OnBtnViewReset(void)
{
  CameraDefault(hwndAll, hdcAll, hglrcAll, &actor->cameraAll, &actor->widthAll, &actor->heightAll, &actor->ratioAll);
  DrawFrameAll();
  SetFocus(hwndMain);
}

void EnableNeighDirWindow(BOOL sw)
{
  EnableWindow(hwndDirN0, sw);
  EnableWindow(hwndDirN1, sw);
  EnableWindow(hwndDirN2, sw);
  EnableWindow(hwndDirN3, sw);
  EnableWindow(hwndDirN4, sw);
  EnableWindow(hwndDirN5, sw);
  EnableWindow(hwndDirN6, sw);
  EnableWindow(hwndDirN7, sw);
}

void OnBtnNeiDirec(WORD ID)
{
  actor->ChangeNeighborPoint(ID - IDC_BTNDN0);
  DrawFrameAll();
  SetFocus(hwndMain);
}
