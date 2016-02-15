#include "checkbox.h"

//reference
HWND hwndHead;
HWND hwndBody;
HWND hwndEtc;
HWND hwndNeigh;

void BuildCheckboxes(HINSTANCE hInst)
{
  hwndHead = CreateWindow("BUTTON", _("head"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNHEAD, hInst, NULL);
  hwndBody = CreateWindow("BUTTON", _("body"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNBODY, hInst, NULL);
  hwndEtc = CreateWindow("BUTTON", _("etc"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNETC, hInst, NULL);
  hwndNeigh = CreateWindow("BUTTON", _("neighbor(v pos)"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_BTNNEI, hInst, NULL);
}

void RepositionCheckBoxes(RECT *rc)
{
  MoveWindow(hwndHead, rc->right-136, 28, 97, 18, TRUE);
  MoveWindow(hwndBody, rc->right-136, 52, 97, 18, TRUE);
  MoveWindow(hwndEtc, rc->right-136, 76, 97, 18, TRUE);
  MoveWindow(hwndNeigh, rc->right-136, 100, 136, 18, TRUE);
}

void OwnerDrawCheckBox(DRAWITEMSTRUCT *lpd)
{
  HFONT origFont;
  HBRUSH origBrush;
  HPEN curPen;
  HPEN origPen;
  char buf[128];

  switch (lpd->CtlID) {
  case IDC_BTNHEAD:
  case IDC_BTNBODY:
  case IDC_BTNETC:
  case IDC_BTNNEI:
    GetWindowText(lpd->hwndItem, buf, sizeof(buf));
    origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)defFont);
    origBrush = (HBRUSH)SelectObject(lpd->hDC, (HBRUSH)(COLOR_3DFACE+1));
    curPen = CreatePen(PS_SOLID, 0, RGB(0xFF, 0xFF, 0xFF));
    origPen = (HPEN)SelectObject(lpd->hDC, curPen);
    FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_3DFACE+1));
    RECT rc;
    rc.top = lpd->rcItem.top+3;
    rc.bottom = lpd->rcItem.top+16;
    rc.left = lpd->rcItem.left;
    rc.right = lpd->rcItem.left+14;
    FillRect(lpd->hDC, &rc, (HBRUSH)(COLOR_3DHILIGHT+1));
    DeleteObject(curPen);
    curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
    SelectObject(lpd->hDC, (HGDIOBJ)curPen);
    MoveToEx(lpd->hDC, rc.left, rc.bottom-2, NULL);
    LineTo(lpd->hDC, rc.left, rc.top);
    LineTo(lpd->hDC, rc.right-1, rc.top);
    DeleteObject(curPen);
    curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));
    SelectObject(lpd->hDC, (HGDIOBJ)curPen);
    MoveToEx(lpd->hDC, rc.right-2, rc.top+1, NULL);
    LineTo(lpd->hDC, rc.right-2, rc.bottom-2);
    LineTo(lpd->hDC, rc.left, rc.bottom-2);
    DeleteObject(curPen);
    curPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW));
    SelectObject(lpd->hDC, (HGDIOBJ)curPen);
    MoveToEx(lpd->hDC, rc.left+1, rc.bottom-3, NULL);
    LineTo(lpd->hDC, rc.left+1, rc.top+1);
    LineTo(lpd->hDC, rc.right-2, rc.top+1);
    GetWindowText(lpd->hwndItem, buf, sizeof(buf));
    origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)defFont);
    rc.top += 2;
    rc.bottom -= 2;
    rc.left += 2;
    rc.right -= 2;
    //
    if (lpd->itemState & ODS_DISABLED) {
      FillRect(lpd->hDC, &rc, (HBRUSH)(COLOR_3DLIGHT+1));
      SetTextColor(lpd->hDC, RGB(0xFF, 0xFF, 0xFF));
      TextOut(lpd->hDC, 21, 3, buf, (int)strlen(buf));
      SetTextColor(lpd->hDC, GetSysColor(COLOR_GRAYTEXT));
      SetBkMode(lpd->hDC, TRANSPARENT);
      TextOut(lpd->hDC, 20, 2, buf, (int)strlen(buf));
      SelectObject(lpd->hDC, (HGDIOBJ)origFont);
      return;
    }
    // enabled text
    TextOut(lpd->hDC, 20, 2, buf, (int)strlen(buf));
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    if (lpd->itemState & ODS_SELECTED) {
      FillRect(lpd->hDC, &rc, (HBRUSH)(COLOR_3DLIGHT+1));
    }
    // check
    bool drawX;
    switch (lpd->CtlID) {
    case IDC_BTNHEAD:
      drawX = actor->enableRefHead ? true : false;
      break;
    case IDC_BTNBODY:
      drawX = actor->enableRefBody ? true : false;
      break;
    case IDC_BTNETC:
      drawX = actor->enableRefEtc ? true : false;
      break;
    case IDC_BTNNEI:
      drawX = actor->enableRefNeigh ? true : false;
      break;
    }

    if (drawX) {
      DeleteObject(curPen);
      curPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
      SelectObject(lpd->hDC, (HGDIOBJ)curPen);
      MoveToEx(lpd->hDC, rc.left+1, rc.top+1, NULL);
      LineTo(lpd->hDC, rc.right-2, rc.bottom-2);
      MoveToEx(lpd->hDC, rc.right-2, rc.top+1, NULL);
      LineTo(lpd->hDC, rc.left+1, rc.bottom-2);
    }

    DeleteObject(curPen);
    SelectObject(lpd->hDC, (HGDIOBJ)origPen);
    SelectObject(lpd->hDC, (HGDIOBJ)origBrush);
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    return;
  }
}

void OnBtnHead(void)
{
  if (actor->enableRefHead) {
    actor->enableRefHead = false;
  } else {
    actor->enableRefHead = true;
  }
  InvalidateRect(hwndHead, NULL, TRUE);
  ProcPatternChange(true);
  SetFocus(hwndMain);
}

void OnBtnBody(void)
{
  if (actor->enableRefBody) {
    actor->enableRefBody = false;
  } else {
    actor->enableRefBody = true;
  }
  InvalidateRect(hwndBody, NULL, TRUE);
  DrawFrameAll();
  SetFocus(hwndMain);
}

void OnBtnEtc(void)
{
  if (actor->enableRefEtc) {
    actor->enableRefEtc = false;
  } else {
    actor->enableRefEtc = true;
  }
  InvalidateRect(hwndEtc, NULL, TRUE);
  DrawFrameAll();
  SetFocus(hwndMain);
}

void OnBtnNeigh(void)
{
  if (actor->enableRefNeigh) {
    actor->enableRefNeigh = false;
  } else {
    actor->enableRefNeigh = true;
  }
  InvalidateRect(hwndNeigh, NULL, TRUE);
  DrawFrameAll();
  SetFocus(hwndMain);
}

void CheckRefHead(void)
{
  if (actor->sprRefHead->IsReady() == false) {
    EnableWindow(hwndHead, FALSE);
  } else {
    EnableWindow(hwndHead, TRUE);
  }
  InvalidateRect(hwndHead, NULL, TRUE);
}

void CheckRefBody(void)
{
  if (actor->sprRefBody->IsReady() == false) {
    EnableWindow(hwndBody, FALSE);
  } else {
    EnableWindow(hwndBody, TRUE);
  }
  InvalidateRect(hwndBody, NULL, TRUE);
}

void CheckRefEtc(void)
{
  if (actor->sprRefEtc->IsReady() == false) {
    EnableWindow(hwndEtc, FALSE);
  } else {
    EnableWindow(hwndEtc, TRUE);
  }
  InvalidateRect(hwndEtc, NULL, TRUE);
}

void CheckRefNeigh(void)
{
  if (actor->sprRefNeigh->IsReady() == false) {
    EnableWindow(hwndNeigh, FALSE);
    EnableNeighDirWindow(FALSE);
  } else {
    EnableWindow(hwndNeigh, TRUE);
    EnableNeighDirWindow(TRUE);
  }
  InvalidateRect(hwndNeigh, NULL, TRUE);
}
