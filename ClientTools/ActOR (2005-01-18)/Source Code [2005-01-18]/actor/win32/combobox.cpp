#include "combobox.h"

//act combo
HWND hwndAct;
//act snd
HWND hwndSnd;

void BuildComboBox(HINSTANCE hInst)
{
  hwndAct = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_COMBOBOX | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, 0, 0, 1, 1, hwndMain, (HMENU)IDC_CBACT, hInst, NULL);
  hwndSnd = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_COMBOBOX | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, 0, 0, 1, 1, hwndMain, (HMENU)IDC_CBSND, hInst, NULL);
}

void RepositionComboBox(RECT *rc)
{
  MoveWindow(hwndAct, rc->right-275, 4, 70, 14, TRUE);
  MoveWindow(hwndSnd, rc->right-305, 175, 130, 14, TRUE);
}

void SetStringComboSnd(void)
{
  char buf[128];
  SendMessage(hwndSnd, CB_RESETCONTENT, 0, 0);
  int num = actor->GetNumSounds();
  SNPRINTF(buf, sizeof(buf), "nothing");
  SendMessage(hwndSnd, CB_INSERTSTRING, -1, (LPARAM)buf);
  for (int i=0; i<num; i++) {
    //SNPRINTF(buf, sizeof(buf), "%d", i);
    SNPRINTF(buf, sizeof(buf), "%s", actor->GetSoundFN(i));
    SendMessage(hwndSnd, CB_INSERTSTRING, -1, (LPARAM)buf);
  }
  SendMessage(hwndSnd, CB_SETCURSEL, 0, 0);
}

void OwnerDrawComboBox(DRAWITEMSTRUCT *lpd)
{
  HFONT origFont;
  char buf[256];

  switch (lpd->CtlID) {
  case IDC_CBACT:
  case IDC_CBSND:
    origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)defFont);
    if (lpd->itemID == -1) return;
    SetBkColor(lpd->hDC, GetSysColor(COLOR_WINDOW));
    SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
    if (lpd->itemAction & ODA_DRAWENTIRE && lpd->itemState & ODS_COMBOBOXEDIT) {
      SendMessage(lpd->hwndItem, CB_GETLBTEXT, lpd->itemID, (LPARAM)buf);
      if (IsWindowEnabled(hwndSnd)) {
        SetBkColor(lpd->hDC, GetSysColor(COLOR_WINDOW));
        SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
      } else {
        SetBkColor(lpd->hDC, GetSysColor(COLOR_BTNFACE));
        SetTextColor(lpd->hDC, GetSysColor(COLOR_GRAYTEXT));
      }
      TextOut(lpd->hDC, 5, 3, buf, (int)strlen(buf));
    } else if (lpd->itemAction & ODA_DRAWENTIRE && !(lpd->itemState & ODS_COMBOBOXEDIT)) {
      int m = (int)SendMessage(lpd->hwndItem, CB_GETCOUNT, 0, 0);
      for (int i=0; i<m; i++) {
        SendMessage(lpd->hwndItem, CB_GETLBTEXT, i, (LPARAM)buf);
        TextOut(lpd->hDC, 0, i*12, buf, (int)strlen(buf));
      }
    } else if ((lpd->itemState & ODS_SELECTED) && (lpd->itemAction & (ODA_SELECT|ODA_DRAWENTIRE))) {
      FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_HIGHLIGHT+1));
      SendMessage(lpd->hwndItem, CB_GETLBTEXT, lpd->itemID, (LPARAM)buf);
      SetBkColor(lpd->hDC, GetSysColor(COLOR_HIGHLIGHT));
      SetTextColor(lpd->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
      TextOut(lpd->hDC, 0, lpd->itemID*12, buf, (int)strlen(buf));
    } else if (!(lpd->itemState & ODS_SELECTED) && (lpd->itemAction & ODA_SELECT)) {
      FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_WINDOW+1));
      SendMessage(lpd->hwndItem, CB_GETLBTEXT, lpd->itemID, (LPARAM)buf);
      SetBkColor(lpd->hDC, GetSysColor(COLOR_WINDOW));
      SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
      TextOut(lpd->hDC, 0, lpd->itemID*12, buf, (int)strlen(buf));
    }
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    break;
  }
}

void OnComboActChange(void)
{
  bool tmpPlay = autoPlay;
  if (tmpPlay) {
    OnBtnStop();
  }

  UnSelectSelection();
  int i = (int)SendMessage(hwndAct, CB_GETCURSEL, 0, 0);

  switch (actor->GetNumAction()) {
  case 1:
    SetStateDirecButton(0x00);
    break;
  case 2:
    SetStateDirecButton(0x82);
    break;
  case 4:
    SetStateDirecButton(0xAA);
    break;
  default:
    if ((i+1)*8 <= actor->GetNumAction()) {
      SetStateDirecButton(0xFF);
    } else {
      BYTE f = 0;
      for (int i=0; i<actor->GetNumAction() % 8; i++) {
        f <<= 1;
        f |= 1;
      }
      SetStateDirecButton(f);
    }
    break;
  }

  actor->curAct = i*8;
  actor->curDir = 0;
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

void OnComboSndChange(void)
{
  if (actor->act == NULL) return;
  if (autoPlay)  return;
  int no = (int)SendMessage(hwndSnd, CB_GETCURSEL, 0, 0);
  actor->SetSoundNo(actor->curAct, actor->curPat, no-1);
}

void OnCommandCombo(WPARAM wParam)
{
  switch (LOWORD(wParam)) {
  case IDC_CBACT:
    switch (HIWORD(wParam)) {
    case CBN_DROPDOWN:
      RECT r;
      int m;
      GetWindowRect(hwndAct, &r);
      m = (int)SendMessage(hwndAct, CB_GETCOUNT, 0, 0);
      SetWindowPos(hwndAct, 0, 0, 0, r.right-r.left, (r.bottom-r.top)*(m+1), SWP_NOMOVE | SWP_NOZORDER);
      break;
    case CBN_SELCHANGE:
      OnComboActChange();
      break;
    }
    break;
  case IDC_CBSND:
    switch (HIWORD(wParam)) {
    case CBN_DROPDOWN:
      RECT r;
      int m;
      GetWindowRect(hwndSnd, &r);
      m = (int)SendMessage(hwndSnd, CB_GETCOUNT, 0, 0);
      SetWindowPos(hwndSnd, 0, 0, 0, r.right-r.left, (r.bottom-r.top)*(m+1), SWP_NOMOVE | SWP_NOZORDER);
      break;
    case CBN_SELCHANGE:
      OnComboSndChange();
      break;
    }
    break;
  }
}

void OnMeasureCombo(MEASUREITEMSTRUCT *mi)
{
  switch (mi->CtlID) {
  case IDC_CBACT:
  case IDC_CBSND:
    mi->itemHeight = 12;
    break;
  }
}
