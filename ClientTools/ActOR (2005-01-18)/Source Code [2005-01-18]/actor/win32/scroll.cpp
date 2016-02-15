#include "scroll.h"

//scroll
HWND hwndScr;
HWND hwndFrmImgScr;

void BuildScroll(HINSTANCE hInst)
{
  //frame scroll
  hwndScr = CreateWindow("SCROLLBAR", "", WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 1, 1, hwndMain, (HMENU)IDC_SCROLL, hInst, NULL);
  //frame image scr
  hwndFrmImgScr = CreateWindow("SCROLLBAR", "", WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 1, 1, hwndMain, (HMENU)IDC_SCROLLI, hInst, NULL);
}

void RepositionScroll(RECT *rc)
{
  //frame scroll
  MoveWindow(hwndScr, rc->right-320, 124, 130, 16, TRUE);
  //image list scr
  MoveWindow(hwndFrmImgScr, 3, rc->bottom - 15, rc->right - 6, 14, TRUE);
}

void OnHScroll(WPARAM wParam, LPARAM lParam)
{
  if (LOWORD(wParam) == SB_ENDSCROLL)  return;
  if ((HWND)lParam == hwndFrmImgScr) {
    OnHScrollFrameImage(wParam);
  } else {
    UnSelectSelection();
    OnHScrollFrame(wParam);
  }
}

int GetScrollImgPos(void)
{
  SCROLLINFO scri;
  scri.cbSize = sizeof(SCROLLINFO);
  scri.fMask = SIF_POS;
  GetScrollInfo(hwndMain, SB_HORZ, &scri);
  return scri.nPos;
}

void SetScrollPos_(HWND hwnd, int no, int max)
{
  SCROLLINFO scri;
  scri.cbSize = sizeof(SCROLLINFO);
  scri.fMask = SIF_POS | SIF_RANGE;
  scri.nMin = 0;
  scri.nMax = max;
  scri.nPos = no;
  SetScrollInfo(hwnd, SB_CTL, &scri, TRUE);
}

void OnHScrollFrameImage(WPARAM wParam)
{
  int max = actor->GetNumImage()-1;
  switch (LOWORD(wParam)) {
  case SB_LINEDOWN:
    actor->curImg++;
    break;
  case SB_PAGEDOWN:
    actor->curImg += 5;
    break;
  case SB_LINEUP:
    actor->curImg--;
    break;
  case SB_PAGEUP:
    actor->curImg -= 5;
    break;
  case SB_THUMBTRACK:
    actor->curImg = HIWORD(wParam); 
    SetImages(actor->curImg);
    break;
  default:
    return;
  }
  if (actor->curImg > max)  actor->curImg = max;
  if (actor->curImg < 0)  actor->curImg = 0;
  SetScrollPos_(hwndFrmImgScr, actor->curImg, max);
  SetImages(actor->curImg);
}

void OnHScrollFrame(WPARAM wParam)
{
  if (IsActReady() == false)  return;

  int max = actor->GetNumPattern(actor->curAct) - 1;

  switch (LOWORD(wParam)) {
  case SB_LINEDOWN:
    actor->curPat++;
    break;
  case SB_PAGEDOWN:
    if (actor->curPat+5 > max) {
      actor->curPat = max;
    } else {
      actor->curPat += 5;
    }
    break;
  case SB_LINEUP:
    actor->curPat--;
    break;
  case SB_PAGEUP:
    if (actor->curPat-5 < 0) {
      actor->curPat = 0;
    } else {
      actor->curPat -= 5;
    }
    break;
  case SB_THUMBTRACK:
    actor->curPat = HIWORD(wParam); 
    break;
  default:
    return;
  }
  if (actor->curPat > max)  actor->curPat = 0;
  if (actor->curPat < 0)  actor->curPat = max;
  ProcPatternChange(true);
}
