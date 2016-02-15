#include "labels.h"

//labels
HWND hwndLabel1;
HWND hwndLabel2;
HWND hwndLabel3;
HWND hwndLabel4;
HWND hwndLabel5;
HWND hwndLabel6;
HWND hwndLabelDir;
HWND hwndLabelFrame;

void BuildLabels(HINSTANCE hInst)
{
  hwndLabel1 = CreateWindow("STATIC", _("action"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabel2 = CreateWindow("STATIC", _("direction"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabel3 = CreateWindow("STATIC", _("reference"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabelDir = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STAT_A, hInst, NULL);
  hwndLabelFrame = CreateWindow("STATIC", "1 / 1", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabel4 = CreateWindow("STATIC", _("Interval"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabel5 = CreateWindow("STATIC", _("milli sec"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
  hwndLabel6 = CreateWindow("STATIC", _("Sound"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_STATIC, hInst, NULL);
}

void RepositionLabels(RECT *rc)
{
  MoveWindow(hwndLabel1, rc->right-320, 7, 34, 12, TRUE);
  MoveWindow(hwndLabel2, rc->right-320, 50, 44, 12, TRUE);
  MoveWindow(hwndLabel3, rc->right-120, 7, 80, 12, TRUE);
  MoveWindow(hwndLabelDir, rc->right-242, 49, 14, 14, TRUE);
  MoveWindow(hwndLabelFrame, rc->right-263, 145, 86, 12, TRUE);
  MoveWindow(hwndLabel4, rc->right-347, 158, 80, 12, TRUE);
  MoveWindow(hwndLabel5, rc->right-220, 158, 100, 12, TRUE);
  MoveWindow(hwndLabel6, rc->right-347, 178, 40, 12, TRUE);
}

void OwnerDrawLabels(DRAWITEMSTRUCT *lpd)
{
  HFONT arrowFont;
  HFONT origFont;
  char buf[256];

  switch (lpd->CtlID) {
  case IDC_STAT_A:
    FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_3DFACE+1));
    if (actor != NULL) {
      if ((actor->curDir % 2) == 0) {
        arrowFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");
      } else {
        arrowFont = CreateFont(12, 0, 450, 450, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");
      }
      switch (actor->curDir) {
      case 0:
        strcpy(buf, _("v"));
        break;
      case 1:
      case 2:
        strcpy(buf, _("<"));
        break;
      case 3:
      case 4:
        strcpy(buf, _("^"));
        break;
      case 5:
      case 6:
        strcpy(buf, _(">"));
        break;
      case 7:
        strcpy(buf, _("v"));
        break;
      }
      origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)arrowFont);
      if ((actor->curDir % 2) == 0) {
        TextOut(lpd->hDC, 0, 0, buf, (int)strlen(buf));
      } else {
        TextOut(lpd->hDC, -5, 5, buf, (int)strlen(buf));
      }
    } else {
      arrowFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");
      strcpy(buf, "+");
      origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)arrowFont);
      TextOut(lpd->hDC, 0, 0, buf, (int)strlen(buf));
    }
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    DeleteObject(arrowFont);
    break;
  case IDC_STATIC:
    GetWindowText(lpd->hwndItem, buf, sizeof(buf));
    origFont = (HFONT)SelectObject(lpd->hDC, (HGDIOBJ)defFont);
    FillRect(lpd->hDC, &lpd->rcItem, (HBRUSH)(COLOR_3DFACE+1));
    TextOut(lpd->hDC, 0, 0, buf, (int)strlen(buf));
    SelectObject(lpd->hDC, (HGDIOBJ)origFont);
    break;
  }
}
