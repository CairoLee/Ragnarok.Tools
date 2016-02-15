#include "table.h"

//list view(pseudo table)
HWND hwndTable;
HWND hwndTableEdit;
WNDPROC wndprocOrigTable;
WNDPROC wndprocOrigTableEdit;

void BuildTable(HINSTANCE hInst)
{
  LoadLibrary("RICHED32.DLL");
  char colHeader[NUM_1SPR_PROP][9] = {"SprNo", "SpType", "Xoffs", "Yoffs", "Mirror", "AABBGGRR", "Xmag", "Ymag", "Rot"};
  hwndTable = CreateWindowEx(0, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS,
                             0, 0, 1, 1, hwndMain, (HMENU)IDC_TABLE, hInst, NULL);
  DWORD style;
  style = (DWORD)SendMessage(hwndTable, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
  SendMessage(hwndTable, LVM_SETEXTENDEDLISTVIEWSTYLE, style, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
  wndprocOrigTable = (WNDPROC)GetWindowLong(hwndTable, GWL_WNDPROC);
  SetWindowLong(hwndTable, GWL_WNDPROC, (LONG)WndProcTable);

  LVCOLUMN lvcol;
  lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
  lvcol.fmt = LVCFMT_LEFT;
  for (int i=0; i<NUM_1SPR_PROP; i++) {
    if (i == 1 || i == 4) {
      lvcol.cx = 20;
    } else if (i==5) {
      lvcol.cx = 80;
    } else {
      lvcol.cx = 40;
    }
    lvcol.pszText = colHeader[i];
    lvcol.iSubItem = i;
    SendMessage(hwndTable, LVM_INSERTCOLUMN, i, (LPARAM)&lvcol);
  }
}

void RepositionTable(RECT *rc)
{
  MoveWindow(hwndTable, rc->right-360, 200, 360, rc->bottom * vSplitPercent / 100 - 205, TRUE);
}

DWORD HexToDWORD(char *hex)
{
  DWORD ret = 0;
  while (*hex != '\0') {
    if (*hex >= 'a') *hex -= 'a' - 'A';
    if (*hex >= 'A') {
      *hex = *hex - 'A' + 10;
      if (*hex >= 16)  return 0;
    } else {
      *hex -= '0';
      if (*hex >= 10)  return 0;
    }
    ret *= 16;
    ret |= *hex;
    hex++;
  }
  return ret;
}

BYTE SaturateByte5(BYTE orig, bool plus)
{
  BYTE ret;

  if (plus) {
    if (orig <= 250) {
      ret = orig + 5;
    } else {
      ret = 255;
    }
  } else {
    if (orig >= 5) {
      ret = orig - 5;
    } else {
      ret = 0;
    }
  }
  return ret;
}

void GetSelectedTableItem(int *sel, int max)
{
  int i = 0;
  int f = -1;
  do {
    f = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, f, MAKELPARAM(LVNI_SELECTED, 0));
    if (f != -1) {
      *(sel + i) = f;
    } else {
      break;
    }
    i++;
  } while (i != max);
}

void SwapTableItemText(HWND hwnd, int arow, int acol, int brow, int bcol)
{
  LVITEM li;
  char abuf[128], bbuf[128];

  li.mask = LVIF_TEXT;
  li.pszText = abuf;
  li.cchTextMax = sizeof(abuf);
  li.iSubItem = acol;
  SendMessage(hwnd, LVM_GETITEMTEXT, arow, (LPARAM)&li);
  li.pszText = bbuf;
  li.cchTextMax = sizeof(bbuf);
  li.iSubItem = bcol;
  SendMessage(hwnd, LVM_GETITEMTEXT, brow, (LPARAM)&li);
  li.iSubItem = acol;
  SendMessage(hwnd, LVM_SETITEMTEXT, arow, (LPARAM)&li);
  li.pszText = abuf;
  li.cchTextMax = sizeof(abuf);
  li.iSubItem = bcol;
  SendMessage(hwnd, LVM_SETITEMTEXT, brow, (LPARAM)&li);
}

void SetTableFromEdit(char *str)
{
  actor->SetModified();

  int row = actor->oldMP.y;
  int column = actor->oldMP.x;

  char txt[128];
  if (str == NULL) {
    GetWindowText(hwndTableEdit, txt, sizeof(txt));
  } else {
    strcpy(txt, str);
  }
  // colheader = {"SprNo", "SpType", "Xoffs", "Yoffs", "Mirror", "AABBGGRR", "Xmag", "Ymag", "Rot"};
  int i;
  float f;

  // follow oldMP means selected position in table.  See TimerTableEdit function
  if (row == actor->GetNumSprites(actor->curAct, actor->curPat)) {
    //if head
    i = atoi(txt);
    switch (column) {
    case 2:  //xoffs
      actor->SetExtXValue(actor->curAct, actor->curPat, i);
      break;
    case 3:  //yoffs;
      actor->SetExtYValue(actor->curAct, actor->curPat, i);
      break;
    }
    sprintf(txt, "%d", i);
    SetTableItem(txt, row, column);
    SetWindowText(hwndTableEdit, txt);
    ProcPatternChange(true);
    return;
  }

  switch (column) {
  case 0:  //SprNo
    i = atoi(txt);
    if (actor->GetSpTypeValue(actor->curAct, actor->curPat, row) == 0) {
      int max = actor->GetNumPalImage();
      if (i >= max)  i = max - 1;
    } else {
      int max = actor->GetNumFlatImage();
      if (i >= max)  i = max - 1;
    }
    sprintf(txt, "%d", i);
    actor->SetSprNoValue(actor->curAct, actor->curPat, row, i);
    break;
  case 1:  //SpType
    i = atoi(txt);
    if (i > 1) i = 1;
    if (actor->GetNumFlatImage() == 0) i = 0;
    sprintf(txt, "%d", i);
    actor->SetSpTypeValue(actor->curAct, actor->curPat, row, i);
    break;
  case 2:  //Xoffs
    i = atoi(txt);
    sprintf(txt, "%d", i);
    actor->SetXOffsValue(actor->curAct, actor->curPat, row, i);
    break;
  case 3:  //Yoffs
    i = atoi(txt);
    sprintf(txt, "%d", i);
    actor->SetYOffsValue(actor->curAct, actor->curPat, row, i);
    break;
  case 4:  //Mirror
    i = atoi(txt);
    i = (i != 0) ? 1 : 0;
    sprintf(txt, "%d", i);
    actor->SetMirrorValue(actor->curAct, actor->curPat, row, i);
    break;
  case 5:  //AABBGGRR
    i = HexToDWORD(txt);
    sprintf(txt, "%08X", i);
    actor->SetABGRValue(actor->curAct, actor->curPat, row, i);
    break;
  case 6: //Xmag
    f = (float)atof(txt);
    sprintf(txt, "%f", f);
    actor->SetXMagValue(actor->curAct, actor->curPat, row, f);
    break;
  case 7: //Ymag
    f = (float)atof(txt);
    sprintf(txt, "%f", f);
    actor->SetYMagValue(actor->curAct, actor->curPat, row, f);
    break;
  case 8: //rot
    i = atoi(txt);
    i = i%360;
    if (i < 0) i += 360;
    sprintf(txt, "%d", i);
    actor->SetRotValue(actor->curAct, actor->curPat, row, i);
    break;
  }
  SetTableItem(txt, row, column);
  SetWindowText(hwndTableEdit, txt);
  ProcPatternChange(true);
}

void SetTableItemAll(int noAct, int noPat)
{
  if (IsActReady() == false)  return;

  char buf[20];
  int max = (int)SendMessage(hwndTable, LVM_GETITEMCOUNT, 0, 0);
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *selArray;
  if (!autoPlay) {
    selArray = (int *)malloc(sizeof(int)*max);
    GetSelectedTableItem(selArray, max);
  }
  SendMessage(hwndTable, LVM_DELETEALLITEMS, 0, 0);

  for (int i=0; i< actor->GetNumSprites(noAct, noPat); i++) {
    // colheader = {"SprNo", "SpType", "Xoffs", "Yoffs", "Mir", "–O˜a", "Xmag", "Ymag", "Rot"};
    //spr no
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetSprNoValue(noAct, noPat, i));
    SetTableItem(buf, i, 0);
    //sp type
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetSpTypeValue(noAct, noPat, i));
    SetTableItem(buf, i, 1);
    int xo, yo;
    actor->GetOffsPoint(&xo, &yo, noAct, noPat, i);
    // xoffs
    SNPRINTF(buf, sizeof(buf), "%d", xo);
    SetTableItem(buf, i, 2);
    // yoffs
    SNPRINTF(buf, sizeof(buf), "%d", yo);
    SetTableItem(buf, i, 3);
    // mirror
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetMirrorValue(noAct, noPat, i));
    SetTableItem(buf, i, 4);
    // saturation
    SNPRINTF(buf, sizeof(buf), "%08X", actor->GetABGRValue(noAct, noPat, i));
    SetTableItem(buf, i, 5);
    // xmag
    SNPRINTF(buf, sizeof(buf), "%f", actor->GetXMagValue(noAct, noPat, i));
    SetTableItem(buf, i, 6);
    // ymag
    SNPRINTF(buf, sizeof(buf), "%f", actor->GetYMagValue(noAct, noPat, i));
    SetTableItem(buf, i, 7);
    // rot
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetRotValue(noAct, noPat, i));
    SetTableItem(buf, i, 8);
  }
  if (actor->matchRefHead && actor->enableRefHead) {
    SetTableItem("EXT(ref head)", i, 0);
    int extX, extY;
    actor->GetExtOffsPoint(&extX, &extY, actor->curAct, actor->curPat);
    SNPRINTF(buf, sizeof(buf), "%d", extX);
    SetTableItem(buf, i, 2);
    SNPRINTF(buf, sizeof(buf), "%d", extY);
    SetTableItem(buf, i, 3);
  }
  if (!autoPlay) {
    LVITEM li;
    for (int i=0; i<numS; i++) {
      li.mask = LVIF_STATE;
      li.state = (UINT)-1;
      li.stateMask = LVIS_SELECTED;
      SendMessage(hwndTable, LVM_SETITEMSTATE, *(selArray+i), (LPARAM)&li);
    }
    free(selArray);
  }
}

void SetTableItem(char *txt, int iItem, int iSubItem)
{
  char buf[16];
  LVITEM li;
  li.mask = LVIF_TEXT;
  li.iItem = iItem;

  if (iSubItem == 0) {
    li.pszText = buf;
    li.cchTextMax = sizeof(buf);
    if (SendMessage(hwndTable, LVM_GETITEM, 0, (LPARAM)&li) == FALSE) {
      li.pszText = txt;
      li.cchTextMax = (int)strlen(txt);
      li.iSubItem = 0;
      SendMessage(hwndTable, LVM_INSERTITEM, 0, (LPARAM)&li);
      return;
    } else {
      goto SetTableItem_Set;  // after 3 line
    }
  } else {
SetTableItem_Set:
    li.pszText = txt;
    li.cchTextMax = (int)strlen(txt);
    li.iSubItem = iSubItem;
    SendMessage(hwndTable, LVM_SETITEM, 0, (LPARAM)&li);
  }
}

void DownSelectedTableItem(HWND hwnd, int num)
{
  if (DispUndoInitWarningSPO() == false)  return;
  UndoRedoInit();
  int max = (int)SendMessage(hwnd, LVM_GETITEMCOUNT, 0, 0);
  int *selArray;
  selArray = (int *)malloc(sizeof(int)*max);
  GetSelectedTableItem(selArray, max);

  int i=0, j;
  if (actor->enableRefHead && actor->matchRefHead) {
    if (*(selArray+num-1) == max-1) { //if head row select
      num--;
    }
    if (*(selArray+num-1) == max-2) { //if (head row-1) select
      num--;
    }
  } else {
    while (*(selArray+num-1-i) == max-1-i) { //if last one selected, it is ignored
      i++;
    }
    num -= i;
  }

  LVITEM li;
  int spA, spB;
  for (i=num-1; i>=0; i--) {
    spA = *(selArray+i);
    spB = *(selArray+i)+1;
    if (spA < 0 || spA >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    if (spB < 0 || spB >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    for (j=0; j<NUM_1SPR_PROP; j++) {
      SwapTableItemText(hwnd, *(selArray+i)+1, j, *(selArray+i), j);
    }
    actor->SwapSpriteOrder(actor->curAct, actor->curPat, spA, spB);
    li.mask = LVIF_STATE;
    li.stateMask = LVIS_SELECTED;
    li.state = (UINT)-1;
    SendMessage(hwnd, LVM_SETITEMSTATE, *(selArray+i)+1, (LPARAM)&li);
    li.state = 0;
    SendMessage(hwnd, LVM_SETITEMSTATE, *(selArray+i), (LPARAM)&li);
    UpdateWindow(hwnd);
  }

  free(selArray);
}

void UpSelectedTableItem(HWND hwnd, int num)
{
  if (DispUndoInitWarningSPO() == false)  return;
  UndoRedoInit();
  int max = (int)SendMessage(hwnd, LVM_GETITEMCOUNT, 0, 0);
  int *selArray;
  selArray = (int *)malloc(sizeof(int)*max);
  GetSelectedTableItem(selArray, max);

  if (actor->enableRefHead && actor->matchRefHead) {
    if (*(selArray+num-1) == max-1) { //if head row select
      num--;
    }
  }
  int i=0, j;
  while (*(selArray+i) == i) { //if first one selected, it is ignored
    i++;
  }
  num -= i;

  LVITEM li;
  int spA, spB;
  for (i=0; i<num; i++) {
    spA = *(selArray+i)-1;
    spB = *(selArray+i);
    if (spA < 0 || spA >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    if (spB < 0 || spB >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    for (j=0; j<NUM_1SPR_PROP; j++) {
      SwapTableItemText(hwnd, *(selArray+i)-1, j, *(selArray+i), j);
    }
    actor->SwapSpriteOrder(actor->curAct, actor->curPat, spA, spB);
    li.mask = LVIF_STATE;
    li.stateMask = LVIS_SELECTED;
    li.state = (UINT)-1;
    SendMessage(hwnd, LVM_SETITEMSTATE, *(selArray+i)-1, (LPARAM)&li);
    li.state = 0;
    SendMessage(hwnd, LVM_SETITEMSTATE, *(selArray+i), (LPARAM)&li);
    UpdateWindow(hwnd);
  }

  free(selArray);
}

void ChangeSaturationABGRValue(int type, bool plus)
{
  int iItem = -1;
  char buf[10];
  LVITEM li;
  li.cchTextMax = sizeof(buf);
  li.pszText = buf;
  li.mask = LVIF_TEXT;
  li.iSubItem = 5;
  actor->oldMP.x = 5;
  DWORD sat;
  BYTE v;
  DWORD mask, mask1, mask2;
  mask1 = 0xFFFFFF00;
  mask1 <<= type*8;
  mask2 = 0x00FFFFFF;
  mask2 >>= (3-type)*8;
  mask = mask1 | mask2;
  do {
    iItem = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, iItem, MAKELPARAM(LVNI_SELECTED, 0));
    if (iItem == -1)  break;
    SendMessage(hwndTable, LVM_GETITEMTEXT, iItem, (LPARAM)&li);
    actor->oldMP.y = iItem;
    // AABBGGRR
    sat = HexToDWORD(buf);
    v = (BYTE)((sat & (~mask)) >> (type*8));
    if (plus) {
      v = SaturateByte5(v, true);
    } else {
      v = SaturateByte5(v, false);
    }
    sat &= mask;
    sat |= v << (type*8);
    sprintf(buf, "%08X", sat);
    SetTableFromEdit(buf);
  } while (iItem != -1);
}

void ChangeMirror(void)
{
  int iItem = -1;
  char buf[10];
  LVITEM li;
  li.cchTextMax = sizeof(buf);
  li.pszText = buf;
  li.mask = LVIF_TEXT;
  li.iSubItem = 4;
  actor->oldMP.x = 4;
  do {
    iItem = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, iItem, MAKELPARAM(LVNI_SELECTED, 0));
    if (iItem == -1)  break;
    SendMessage(hwndTable, LVM_GETITEMTEXT, iItem, (LPARAM)&li);
    actor->oldMP.y = iItem;
    if (buf[0] == '1') {
      buf[0] = '0';
    } else {
      buf[0] = '1';
    }
    SetTableFromEdit(buf);
  } while (iItem != -1);
}

void OnKeyUpInTable(WPARAM wParam, LPARAM lParam)
{
  if (wParam == VK_ESCAPE) {
    UnSelectSelection();
  }
}

void OnSysKeyUpInTable(WPARAM wParam, LPARAM lParam)
{
  switch (wParam) {
  case VK_UP:
  case VK_DOWN:
  case VK_RIGHT:
  case VK_LEFT:
    if (HIWORD(lParam) & KF_ALTDOWN) {
      BeginSpritePan();
      SpritePan((BYTE)wParam);
      EndSpritePan();
    }
    break;
  case VK_DELETE:
    if (HIWORD(lParam) & KF_ALTDOWN) {
      if (DispUndoInitWarningSPO() == false)  return;
      UndoRedoInit();
      DeleteSelectedSpriteInTable();
    }
    break;
  }
}

void DeleteSelectedSpriteInTable(void)
{
  int max = (int)SendMessage(hwndTable, LVM_GETITEMCOUNT, 0, 0);
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *selArray = NULL;
  if (!autoPlay) {
    selArray = (int *)malloc(sizeof(int)*max);
    GetSelectedTableItem(selArray, max);
  }

  if (*(selArray+numS-1) == actor->GetNumSprites(actor->curAct, actor->curPat))  numS--;
  for (int i=numS-1; i>=0; i--) {
    actor->DelSpr(actor->curAct, actor->curPat, *(selArray+i));
  }

  free(selArray);

  ProcPatternChange(true);
}

void UnSelectSelection(void)
{
  LVITEM li;
  li.mask = LVIF_STATE;
  li.stateMask = LVIS_SELECTED;
  li.state = 0;
  int iItem = -1;
  do {
    iItem = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, iItem, MAKELPARAM(LVNI_SELECTED, 0));
    if (iItem == -1)  break;
    SendMessage(hwndTable, LVM_SETITEMSTATE, iItem, (LPARAM)&li);
  } while (iItem != -1);
}

void OnNotifyTable(WPARAM wParam, LPARAM lParam)
{
  NMLVDISPINFO *lvi;
  NMLISTVIEW *llv;

  switch (wParam) {
  case IDC_TABLE:
    lvi = (NMLVDISPINFO *)lParam;
    switch (lvi->hdr.code) {
    case LVN_BEGINDRAG:
      actor->dragState = DRAG_TABLE_ROW;
      break;
    case LVN_ITEMCHANGED:
      llv = (NMLISTVIEW *)lParam;
      if ((llv->uChanged & LVIF_STATE) && (actor->dragState != DRAG_TABLE_ROW)) {
        DrawFrameAll();
      }
      break;
    case LVN_ENDLABELEDIT:
      if (lvi->item.pszText == NULL) break;
      actor->oldMP.y = lvi->item.iItem;
      if (actor->oldMP.y == actor->GetNumSprites(actor->curAct, actor->curPat))  break;
      actor->oldMP.x = 0;
      SetTableFromEdit(lvi->item.pszText);
      break;
    }
    break;
  }
}

LRESULT CALLBACK WndProcTable(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LVHITTESTINFO lhti;

  switch (uMsg) {
  case WM_KEYDOWN:
    return 0;
  case WM_KEYUP:
    OnKeyUpInTable(wParam, lParam);
    return 0;
  case WM_SYSKEYUP:
    OnSysKeyUpInTable(wParam, lParam);
    return 0;
  case WM_LBUTTONDOWN:
    SetCapture(hwndTable);
    if (actor->dragState == 0) {
      actor->newMP.x = actor->oldMP.x = LOWORD(lParam);
      actor->newMP.y = actor->oldMP.y = HIWORD(lParam);      
      lhti.pt.x = actor->oldMP.x;
      lhti.pt.y = actor->oldMP.y;
      SendMessage(hwnd, LVM_SUBITEMHITTEST, 0, (LPARAM)&lhti);  
      if (lhti.iSubItem == 0) break;
      RECT rc;
      rc.top = lhti.iSubItem;
      rc.left = LVIR_BOUNDS;
      SendMessage(hwndTable, LVM_GETSUBITEMRECT, lhti.iItem, (LPARAM)&rc);
      actor->firstMP.x = rc.left;
      actor->firstMP.y = rc.top;
      SetTimer(hwnd, TIMER_TABLEEDIT, 1000, (TIMERPROC)TimerTableEdit);
    }
    break;
  case WM_LBUTTONUP:
    ReleaseCapture();
    if (actor->dragState == DRAG_TABLE_ROW) {
      actor->dragState = 0;
      break;
    }
    break;
  case WM_MOUSEMOVE:
    actor->newMP.x = LOWORD(lParam);
    actor->newMP.y = HIWORD(lParam);
    if (actor->dragState == DRAG_TABLE_ROW) {
      int numS = (int)SendMessage(hwnd, LVM_GETSELECTEDCOUNT, 0, 0);
      if (numS == 0)  break;
      /*
      RECT rc;
      rc.left = LVIR_BOUNDS;
      SendMessage(hwndTable, LVM_GETITEMRECT, 0, (LPARAM)&rc);
      int h = rc.bottom - rc.top;
      */
      int h = 12;
      //ClientToScreen(hwnd, &actor->newMP);
      if (actor->newMP.y - actor->oldMP.y > h) {
        DownSelectedTableItem(hwnd, numS);
        actor->oldMP.y = actor->newMP.y;
      } else if (actor->newMP.y - actor->oldMP.y < -h) {
        UpSelectedTableItem(hwnd, numS);
        actor->oldMP.y = actor->newMP.y;
      }
    }
    break;
  }
  return CallWindowProc(wndprocOrigTable, hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProcTableEdit(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
      FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));
      TextOut(hdc, 2, 2, buf, (int)strlen(buf));
      SelectObject(hdc, (HGDIOBJ)origFont);
      EndPaint(hwnd, &ps);
      return 0;
    }
  case WM_KEYUP:
    if (wParam == VK_RETURN) {
      SetTableFromEdit(NULL);
      SendMessage(hwnd, WM_CLOSE, 0, 0);
    } else if (wParam == VK_ESCAPE) {
      SendMessage(hwnd, WM_CLOSE, 0, 0);
    }
    return 0;
  case WM_KILLFOCUS:
    SetTableFromEdit(NULL);
    SendMessage(hwnd, WM_CLOSE, 0, 0);
    return 0;
  case WM_CLOSE:
    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)wndprocOrigTableEdit);
    SetForegroundWindow(hwndMain);
    DestroyWindow(hwnd);
    return 0;
  }
  return CallWindowProc(wndprocOrigTableEdit, hwnd, uMsg, wParam, lParam);
}

VOID CALLBACK TimerTableEdit(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  KillTimer(hwnd, TIMER_TABLEEDIT);
  if (idEvent != TIMER_TABLEEDIT) return;
  if (actor->newMP.x != actor->oldMP.x)  return;
  if (actor->newMP.y != actor->oldMP.y)  return;
  HINSTANCE hinst;
  hinst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);
  LVHITTESTINFO lhti;
  lhti.pt.x = actor->oldMP.x;
  lhti.pt.y = actor->oldMP.y;
  if (SendMessage(hwnd, LVM_SUBITEMHITTEST, 0, (LPARAM)&lhti) == -1)  return;  
  RECT rc;
  rc.top = lhti.iSubItem;
  rc.left = LVIR_BOUNDS;
  SendMessage(hwndTable, LVM_GETSUBITEMRECT, lhti.iItem, (LPARAM)&rc);
  actor->oldMP.x = rc.left;
  actor->oldMP.y = rc.top;
  ClientToScreen(hwnd, &actor->oldMP);
  LVITEM li;
  char buf[128];
  li.mask = LVIF_TEXT;
  li.pszText = buf;
  li.cchTextMax = sizeof(buf);
  li.iSubItem = lhti.iSubItem;
  SendMessage(hwnd, LVM_GETITEMTEXT, lhti.iItem, (LPARAM)&li);
  hwndTableEdit = CreateWindow("RichEdit", buf, WS_POPUP | WS_VISIBLE | ES_AUTOHSCROLL,
                                actor->oldMP.x + 4, actor->oldMP.y-1, rc.right-rc.left-9, rc.bottom-rc.top+3, hwnd, (HMENU)0, hinst, NULL);
  wndprocOrigTableEdit = (WNDPROC)GetWindowLong(hwndTableEdit, GWL_WNDPROC); 
  SetWindowLong(hwndTableEdit, GWL_WNDPROC, (LONG)WndProcTableEdit);
  // now selected position is setted oldMP
  // used in SetTableFromEdit function
  actor->oldMP.x = lhti.iSubItem;
  actor->oldMP.y = lhti.iItem;

  CHARFORMAT cf;
  cf.cbSize = sizeof(cf);
  cf.dwMask = CFM_SIZE | CFM_FACE;
  cf.yHeight = 9*20;
  strcpy(cf.szFaceName, "‚l‚r ‚oƒSƒVƒbƒN");
  SendMessage(hwndTableEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
  ImmSetOpenStatus(ImmGetContext(hwnd), false);
  ShowWindow(hwndTableEdit, SW_SHOW);
  SetForegroundWindow(hwndTableEdit);
}
