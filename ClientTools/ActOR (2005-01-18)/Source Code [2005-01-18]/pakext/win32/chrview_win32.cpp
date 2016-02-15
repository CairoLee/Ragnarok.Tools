#include "chrview_win32.h"

#include <mmsystem.h>

CChrView *cv1;

extern CPAK *pak1;

HDC ThisDC;
HGLRC ThisRC;
HCURSOR defCursor;
HCURSOR updownCursor;
HCURSOR crossCursor;
HWND hwndLB;
int numwav;
BYTE **p_wav;
COLORREF custColor[16];
bool autoPlay;

int CharacterViewer(int argc, char **argv)
{
  cv1 = new CChrView(pak1);

  HINSTANCE hinst;
  HWND chwnd;
  char ttl[512];

  GetConsoleTitle(ttl, sizeof(ttl));
  chwnd = FindWindowEx(0, 0, "ConsoleWindowClass", ttl);
  hinst = GetModuleHandle(NULL);
  defCursor = LoadCursor(NULL, IDC_ARROW);
  updownCursor = LoadCursor(NULL, IDC_SIZENS);
  crossCursor = LoadCursor(NULL, IDC_SIZEALL);
  numwav = 0;
  p_wav = NULL;
  cv1->colBG = 0x00C0C0C0;

  if (DialogBox(hinst, MAKEINTRESOURCE(IDD_CHRVIEW), chwnd, (DLGPROC)DialogProcChrV) == IDCLOSE) {
    return 0;
  } else {
    return 1;
  }
}

void ExitCharacterViewer(HWND hwndDlg, int ID)
{
  KillTimer(hwndDlg, CHRV_TIMER1);
  wglDeleteContext(ThisRC);

  ChrVFreeWavMemory();

  delete cv1;

  EndDialog(hwndDlg, ID);
}


void ChrVInitNFD(HWND hwndDlg)
{
  cv1->InitNFD();

  ChrVInitControlsWeaponShieldPre(hwndDlg);
}

void ChrVInitOpenGL(HWND hwndDlg)
{
  PIXELFORMATDESCRIPTOR pfd;
  int npf;

  if (ThisRC != 0) {
    wglDeleteContext(ThisRC);
  }
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW
              | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 8;
  pfd.iLayerType = PFD_MAIN_PLANE;

  SetWindowPos(GetDlgItem(hwndDlg, IDC_GLAREA), 0, 0, 0, 
               GL_WIN_WIDTH, GL_WIN_HEIGHT, SWP_NOMOVE |SWP_NOZORDER);
  ThisDC = GetDC(GetDlgItem(hwndDlg, IDC_GLAREA));
  npf = ChoosePixelFormat(ThisDC, &pfd);
  SetPixelFormat(ThisDC, npf, &pfd);
  ThisRC = wglCreateContext(ThisDC);
  if (ThisRC == 0) {
    return;
  }
  wglMakeCurrent(ThisDC, ThisRC);

  cv1->InitOpenGL();

  SwapBuffers(ThisDC);
  wglMakeCurrent(ThisDC, 0);
}

bool ChrVCheckInGLArea(HWND hwndDlg, POINT *mp_scrn)
{
  RECT rc;

  GetWindowRect(GetDlgItem(hwndDlg, IDC_GLAREA), &rc);
  if (rc.left <= mp_scrn->x && mp_scrn->x <= rc.right) {
    if (rc.top <= mp_scrn->y && mp_scrn->y <= rc.bottom) {
      return true;
    }
  }
  return false;
}

void ChrVGLZoom(void)
{
  cv1->GLZoom();
  ChrVDrawSprites();
}

void ChrVGLPan(void)
{
  cv1->GLPan();
  ChrVDrawSprites();
}

bool ChrVSpriteSetPCBody(HWND hwndDlg, CSprite **sprite, char *dispname)
{
  char *fname_base = NULL;
  bool ret;

  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    ret = cv1->SpriteSetPCBody(sprite, dispname, fname_base, true);
  } else {
    ret = cv1->SpriteSetPCBody(sprite, dispname, fname_base, false);
  }
  if (ret == false)  return false;

  if (fname_base) {
    if (strstr(fname_base, "\277\356\277\265\300\332\62")) {  // GM2
      ChrVEnableComboAlmost(hwndDlg, false);
      cv1->DestroySprites_GM2();
    } else {
      ChrVEnableComboAlmost(hwndDlg, true);
    }
  }

  return true;
}

bool ChrVSpriteSetPCHead(HWND hwndDlg, CSprite **sprite)
{
  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    return cv1->SpriteSetPCHead(sprite, true);
  } else {
    return cv1->SpriteSetPCHead(sprite, false);
  }
}

bool ChrVSpriteSetPCAcc(HWND hwndDlg, CSprite **sprite, char *dispname, int no)
{
  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    return cv1->SpriteSetPCAcc(sprite, dispname, no, true);
  } else {
    return cv1->SpriteSetPCAcc(sprite, dispname, no, false);
  }
}

bool ChrVSpriteSetPCWeapon(HWND hwndDlg, CSprite **sprite)
{
  char sexname[3], bodyname[100];
  ChrVGetPCBodyName(hwndDlg, bodyname, sexname);

  int i;
  char dispname[100];
  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBWEAPON), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBWEAPON), CB_GETLBTEXT, i, (LPARAM)dispname);

  return cv1->SpriteSetPCWeapon(sprite, dispname, bodyname, sexname);
}

bool ChrVSpriteSetPCShield(HWND hwndDlg, CSprite **sprite)
{
  char sexname[3], bodyname[100];
  ChrVGetPCBodyName(hwndDlg, bodyname, sexname);

  int i;
  char dispname[100];
  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBSHIELD), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBSHIELD), CB_GETLBTEXT, i, (LPARAM)dispname);

  return cv1->SpriteSetPCShield(sprite, bodyname, sexname, dispname);
}

void ChrVDrawSprites(void)
{
  char buf[20];
  sprintf(buf, "%3d / %3d", cv1->curPat+1, cv1->numPat);
  SetWindowText(hwndLB, buf);

  wglMakeCurrent(ThisDC, ThisRC);

  cv1->DrawSprites();

  SwapBuffers(ThisDC);
  wglMakeCurrent(ThisDC, 0);

  if (autoPlay && cv1->sprite_Body) {
    int no = cv1->sprite_Body->GetSoundNo(cv1->curAct, cv1->curPat);
    if (no >= 0) {
      if (*(p_wav + no)) {
        PlaySound((LPCSTR)*(p_wav + no), 0, SND_ASYNC | SND_MEMORY | SND_NODEFAULT | SND_NOWAIT);
      }
    }
  }
}

void ChrVSelectCombo(HWND hwndDlg, WORD ID)
{
  switch (ID) {
  case IDC_CBBODY:
    ChrVSelectComboBody(hwndDlg);
    break;
  case IDC_CBACT:
    ChrVSelectComboAct(hwndDlg);
    break;
  case IDC_CBHEAD:
    ChrVSelectComboHead(hwndDlg);
    break;
  case IDC_CBACC1:
    ChrVSelectComboAcc(hwndDlg, 1);
    break;
  case IDC_CBACC2:
    ChrVSelectComboAcc(hwndDlg, 2);
    break;
  case IDC_CBACC3:
    ChrVSelectComboAcc(hwndDlg, 3);
    break;
  case IDC_CBWEAPON:
    ChrVSelectComboWeapon(hwndDlg);
    break;
  case IDC_CBSHIELD:
    ChrVSelectComboShield(hwndDlg);
    break;
  }
}

void ChrVSelectComboBody(HWND hwndDlg)
{
  char dispname[FILENAME_MAX];
  LRESULT i;

  cv1->sprready = false;
  KillTimer(hwndDlg, CHRV_TIMER1);
  Sleep(100);  //dummy wait

  if (IsDlgButtonChecked(hwndDlg, IDC_RBPC) == BST_CHECKED) {
    ChrVSelectComboBody_PC(hwndDlg);
  } else if (IsDlgButtonChecked(hwndDlg, IDC_RBMOB) == BST_CHECKED) {
    i = SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETCURSEL, 0, 0);
    SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETLBTEXT, i, (LPARAM)dispname);
    if (cv1->SpriteSetMob(&cv1->sprite_Body, dispname) == false)  return;
  } else if (IsDlgButtonChecked(hwndDlg, IDC_RBNPC) == BST_CHECKED) {
    i = SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETCURSEL, 0, 0);
    SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETLBTEXT, i, (LPARAM)dispname);
    if (cv1->SpriteSetNPC(&cv1->sprite_Body, dispname) == false)  return;
  }
  cv1->InitNumActPat();
  ChrVSetScrollBar(hwndDlg, 1, cv1->numPat, 1);

  if (IsDlgButtonChecked(hwndDlg, IDC_RBPC) == BST_CHECKED) {
    ChrVInitControlsActPC(hwndDlg);
  } else if (IsDlgButtonChecked(hwndDlg, IDC_RBMOB) == BST_CHECKED) {
    ChrVInitControlsActMob(hwndDlg);
  } else if (IsDlgButtonChecked(hwndDlg, IDC_RBNPC) == BST_CHECKED) {
    ChrVInitControlsActNPC(hwndDlg);
  }
  cv1->sprready = true;
  ChrVReadWav();
  ChrVDrawSprites();
  if (SendMessage(GetDlgItem(hwndDlg, IDC_BTNAUTO), BM_GETSTATE, 0, 0) == BST_PUSHED) {
    autoPlay = true;
    SetTimer(hwndDlg, CHRV_TIMER1, cv1->GetTimeInterval(), ChrVTimerProc);
  }
}

void ChrVSelectComboBody_PC(HWND hwndDlg)
{
  char dispnameA[FILENAME_MAX], dispnameB[FILENAME_MAX];
  int i;

  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETLBTEXT, i, (LPARAM)dispnameB);

  if (ChrVSpriteSetPCHead(hwndDlg, &cv1->sprite_Head) == false)  return;
  if (ChrVSpriteSetPCBody(hwndDlg, &cv1->sprite_Body, dispnameB) == false)  return;

  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC1), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBACC1), CB_GETLBTEXT, i, (LPARAM)dispnameA);
  if (ChrVSpriteSetPCAcc(hwndDlg, &cv1->sprite_Acc1, dispnameA, 1) == false)  return;
  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC2), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBACC2), CB_GETLBTEXT, i, (LPARAM)dispnameA);
  if (ChrVSpriteSetPCAcc(hwndDlg, &cv1->sprite_Acc2, dispnameA, 2) == false)  return;
  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC3), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBACC3), CB_GETLBTEXT, i, (LPARAM)dispnameA);
  if (ChrVSpriteSetPCAcc(hwndDlg, &cv1->sprite_Acc3, dispnameA, 3) == false)  return;

  cv1->DestroySprite(&cv1->sprite_Weapon);
  cv1->DestroySprite(&cv1->sprite_WpAftimg);
  cv1->DestroySprite(&cv1->sprite_Shield);
  ChrVInitControlsWeaponShieldPre(hwndDlg);
  cv1->DestroyNFD_WeapShield();
  ChrVInitControlsWeaponShield(hwndDlg);
}

void ChrVSelectComboAct(HWND hwndDlg)
{
  if (!cv1->sprready) return;

  bool tmpPlay = false;
  LRESULT ret;
  ret = SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_GETSTATE, 0, 0);
  if (ret == BST_PUSHED)  tmpPlay = true;
  if (tmpPlay) {
    PostMessage(hwndDlg, WM_COMMAND, IDC_BTNSTOP, 0L);
  }

  int idAct = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACT), CB_GETCURSEL, 0, 0);
  cv1->SelectAct(idAct);
  ChrVSetScrollBar(hwndDlg, 1, cv1->numPat, 1);
  ChrVDrawSprites();

  if (tmpPlay) {
    SendMessage(hwndDlg, WM_COMMAND, IDC_BTNAUTO, 0L);
  }
}

void ChrVSelectComboHead(HWND hwndDlg)
{
  cv1->curHead = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBHEAD), CB_GETCURSEL, 0, 0);
  ChrVSelectComboBody(hwndDlg);
}

void ChrVSelectComboAcc(HWND hwndDlg, int no)
{
  switch (no) {
  case 1:
    cv1->curAcc1 = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC1), CB_GETCURSEL, 0, 0);
    break;
  case 2:
    cv1->curAcc2 = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC2), CB_GETCURSEL, 0, 0);
    break;
  case 3:
    cv1->curAcc3 = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACC3), CB_GETCURSEL, 0, 0);
    break;
  }
  ChrVSelectComboBody(hwndDlg);
}

void ChrVSelectComboWeapon(HWND hwndDlg)
{
  ChrVSpriteSetPCWeapon(hwndDlg, &cv1->sprite_Weapon);
}

void ChrVSelectComboShield(HWND hwndDlg)
{
  ChrVSpriteSetPCShield(hwndDlg, &cv1->sprite_Shield);
}

void ChrVSetScrollBar(HWND hwndDlg, int min, int max, int pos)
{
  SCROLLINFO scri;
  scri.cbSize = sizeof(scri);
  scri.nMin = 1;
  scri.nMax = max;
  scri.nPos = pos;
  scri.fMask = SIF_POS | SIF_RANGE;
  SetScrollInfo(GetDlgItem(hwndDlg, IDC_SCRFRM), SB_CTL, &scri, TRUE);
}

void ChrVProcBTNPushDirection(HWND hwndDlg, WORD ID)
{
  if (!cv1->sprready) return;
  bool tmpPlay = false;
  int ret;
  ret = (int)SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_GETSTATE, 0, 0);
  if (ret == BST_PUSHED)  tmpPlay = true;
  if (tmpPlay) {
    PostMessage(hwndDlg, WM_COMMAND, IDC_BTNSTOP, 0L);
  }

  int idAct = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBACT), CB_GETCURSEL, 0, 0);
  cv1->SelectDirection(idAct, ID-IDC_BTNDIRECTION0);
  ChrVSetScrollBar(hwndDlg, 1, cv1->numPat, 1);
  ChrVDrawSprites();

  if (tmpPlay) {
    SendMessage(hwndDlg, WM_COMMAND, IDC_BTNAUTO, 0L);
  }
}

void ChrVInitControls(HWND hwndDlg)
{
  cv1->SetNullSprites();
  ChrVInitNFD(hwndDlg);
  ChrVInitControlsHead(hwndDlg);
  SendDlgItemMessage(hwndDlg, IDC_RBPC, BM_SETCHECK, BST_CHECKED, 0);
  SendDlgItemMessage(hwndDlg, IDC_RBMALE, BM_SETCHECK, BST_CHECKED, 0);
  SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_SETSTATE, TRUE, 0);
  SendDlgItemMessage(hwndDlg, IDC_BTNSTOP, BM_SETSTATE, FALSE, 0);
  hwndLB = GetDlgItem(hwndDlg, IDC_LBPATNO);
  SetWindowText(hwndLB, NULL);
  ChrVInitControlsSub(hwndDlg);
}

void ChrVInitControlsSub(HWND hwndDlg)
{
  cv1->sprready = false;
  KillTimer(hwndDlg, CHRV_TIMER1);
  Sleep(100);  //dummy wait
  cv1->SetCameraDefault();
  cv1->DestroySpriteAll();
  cv1->curAcc1 = cv1->curAcc2 = cv1->curAcc3 = 0;
  if (IsDlgButtonChecked(hwndDlg, IDC_RBPC) == BST_CHECKED) {
    EnableWindow(GetDlgItem(hwndDlg, IDC_MFGROUP), TRUE);
    EnableWindow(GetDlgItem(hwndDlg, IDC_RBMALE), TRUE);
    EnableWindow(GetDlgItem(hwndDlg, IDC_RBFEMALE), TRUE);
  } else {
    EnableWindow(GetDlgItem(hwndDlg, IDC_MFGROUP), FALSE);
    EnableWindow(GetDlgItem(hwndDlg, IDC_RBMALE), FALSE);
    EnableWindow(GetDlgItem(hwndDlg, IDC_RBFEMALE), FALSE);
  }
  SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBACC1, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBACC2, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBACC3, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_RESETCONTENT, 0, 0);
  ChrVInitControlsBody(hwndDlg);
  ChrVInitControlsAccPC(hwndDlg);
}

void ChrVInitControlsBody(HWND hwndDlg)
{
  if (IsDlgButtonChecked(hwndDlg, IDC_RBPC) == BST_CHECKED) {
    ChrVInitControlsBodyPC(hwndDlg);
  } else if (IsDlgButtonChecked(hwndDlg, IDC_RBMOB) == BST_CHECKED) {
    ChrVInitControlsBodyMob(hwndDlg);
  } else {
    ChrVInitControlsBodyNPC(hwndDlg);
  }
}

void ChrVInitControlsBodyMob(HWND hwndDlg)
{
  for (int i=0; i<cv1->NFDMob.num; i++) {
    SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDMob.name + i)->dispname));
  }
  SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsBodyPC(HWND hwndDlg)
{
  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    for (int i=0; i<cv1->NFDPCBodyM.num; i++) {
      SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCBodyM.name + i)->dispname));
    }
  } else {
    for (int i=0; i<cv1->NFDPCBodyF.num; i++) {
      SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCBodyF.name + i)->dispname));
    }
  }
  SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsBodyNPC(HWND hwndDlg)
{
  for (int i=0; i<cv1->NFDNPC.num; i++) {
    SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDNPC.name + i)->dispname));
  }
  SendDlgItemMessage(hwndDlg, IDC_CBBODY, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsHead(HWND hwndDlg)
{
  char *fnp, *fnp2;
  int n = 1;
  char buf[10];

  strcpy(buf, "nothing");
  SendDlgItemMessage(hwndDlg, IDC_CBHEAD, CB_ADDSTRING, -1, (LPARAM)buf);
  for (int i=0; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    fnp = pak1->Entries[i].fn;
    fnp2 = &fnp[strlen(fnp)-3];
    if (stricmp(fnp2, "act") != 0)  continue;
    if (strstr(fnp, PCHEAD_M_DIR) != 0) {
      sprintf(buf, "%d", n++);
      SendDlgItemMessage(hwndDlg, IDC_CBHEAD, CB_ADDSTRING, -1, (LPARAM)buf);
    }
  }
  SendDlgItemMessage(hwndDlg, IDC_CBHEAD, CB_SETCURSEL, 0, 0);
  cv1->curHead = 0;
}

void ChrVInitControlsAccPC(HWND hwndDlg)
{
  char buf[10];

  strcpy(buf, "nothing");
  SendDlgItemMessage(hwndDlg, IDC_CBACC1, CB_ADDSTRING, -1, (LPARAM)buf);
  SendDlgItemMessage(hwndDlg, IDC_CBACC2, CB_ADDSTRING, -1, (LPARAM)buf);
  SendDlgItemMessage(hwndDlg, IDC_CBACC3, CB_ADDSTRING, -1, (LPARAM)buf);

  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    for (int i=0; i<cv1->NFDPCAccM.num; i++) {
      SendDlgItemMessage(hwndDlg, IDC_CBACC1, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccM.name + i)->dispname));
      SendDlgItemMessage(hwndDlg, IDC_CBACC2, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccM.name + i)->dispname));
      SendDlgItemMessage(hwndDlg, IDC_CBACC3, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccM.name + i)->dispname));
    }
  } else {
    for (int i=0; i<cv1->NFDPCAccF.num; i++) {
      SendDlgItemMessage(hwndDlg, IDC_CBACC1, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccF.name + i)->dispname));
      SendDlgItemMessage(hwndDlg, IDC_CBACC2, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccF.name + i)->dispname));
      SendDlgItemMessage(hwndDlg, IDC_CBACC3, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCAccF.name + i)->dispname));
    }
  }
  SendDlgItemMessage(hwndDlg, IDC_CBACC1, CB_SETCURSEL, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBACC2, CB_SETCURSEL, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBACC3, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsWeaponShieldPre(HWND hwndDlg)
{
  cv1->InitControlsWeaponShieldPre();

  char buf[] = " nothing";

  SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_ADDSTRING, -1, (LPARAM)buf);
  SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_ADDSTRING, -1, (LPARAM)buf);
  SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_SETCURSEL, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsWeaponShield(HWND hwndDlg)
{
  char *fnp, *fnp2;
  char bodynameW[100], bodynameS[100], bodyname2[50], sexname[3];

  ChrVGetPCBodyName(hwndDlg, bodyname2, sexname);
  sprintf(bodynameW, "%s%s\\", WEAPON_DIR, bodyname2);
  sprintf(bodynameS, "%s%s\\", SHIELD_DIR, bodyname2);

  for (int i=0; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    fnp = pak1->Entries[i].fn;
    if (strstr(fnp, "act") == 0)  continue;
    if (strstr(fnp, bodynameW)) {
      fnp2 = &fnp[strlen(bodynameW)];
      if (strstr(fnp2, bodyname2) == 0) continue;
      if (strstr(fnp2, sexname) == 0) continue;
      cv1->SetWeaponName(fnp, bodynameW, bodyname2);
    } else if (strstr(fnp, bodynameS)) {
      fnp2 = &fnp[strlen(bodynameS)];
      if (strstr(fnp2, bodyname2) == 0) continue;
      if (strstr(fnp2, sexname) == 0) continue;
      cv1->SetShieldName(fnp, bodynameS, bodyname2);
    }
  }
  qsort(cv1->NFDPCWeapon.name, cv1->NFDPCWeapon.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(cv1->NFDPCShield.name, cv1->NFDPCShield.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);

  for (int i=0; i<cv1->NFDPCWeapon.num; i++) {
    SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCWeapon.name + i)->dispname));
  }
  for (int i=0; i<cv1->NFDPCShield.num; i++) {
    SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_ADDSTRING, -1, (LPARAM)((cv1->NFDPCShield.name + i)->dispname));
  }
  SendDlgItemMessage(hwndDlg, IDC_CBWEAPON, CB_SETCURSEL, 0, 0);
  SendDlgItemMessage(hwndDlg, IDC_CBSHIELD, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsActPC(HWND hwndDlg)
{
  char actname[50], actnamed[50];
  int n;

  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_RESETCONTENT, 0, 0);
  n = cv1->numAct/8;
  if (n == 0) n = 1;
  for (int i=0; i<n; i++) {
    sprintf(actname, PC_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
    SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_ADDSTRING, -1, (LPARAM)actnamed);
  }
  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsActMob(HWND hwndDlg)
{
  char actname[50], actnamed[50];
  int n;

  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_RESETCONTENT, 0, 0);
  n = cv1->numAct/8;
  if (n == 0) n = 1;
  for (int i=0; i<n; i++) {
    sprintf(actname, MOB_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
    SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_ADDSTRING, -1, (LPARAM)actnamed);
  }
  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_SETCURSEL, 0, 0);
}

void ChrVInitControlsActNPC(HWND hwndDlg)
{
  char actname[50], actnamed[50];
  int n;

  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_RESETCONTENT, 0, 0);
  n = cv1->numAct/8;
  if (n == 0) n = 1;
  for (int i=0; i<n; i++) {
    sprintf(actname, MOB_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
    SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_ADDSTRING, -1, (LPARAM)actnamed);
  }
  SendDlgItemMessage(hwndDlg, IDC_CBACT, CB_SETCURSEL, 0, 0);
}

void ChrVEnableComboAlmost(HWND hwndDlg, bool flag)
{
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBHEAD), flag);
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBACC1), flag);
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBACC2), flag);
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBACC3), flag);
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBSHIELD), flag);
  EnableWindow(GetDlgItem(hwndDlg, IDC_CBWEAPON), flag);
}

void ChrVGetPCBodyName(HWND hwndDlg, char *bodyname, char *sexname)
{
  int i;
  char dispname[100];

  i = (int)SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETCURSEL, 0, 0);
  SendMessage(GetDlgItem(hwndDlg, IDC_CBBODY), CB_GETLBTEXT, i, (LPARAM)dispname);
  if (IsDlgButtonChecked(hwndDlg, IDC_RBMALE) == BST_CHECKED) {
    cv1->GetPCBodyName(bodyname, sexname, dispname, true);
  } else {
    cv1->GetPCBodyName(bodyname, sexname, dispname, false);
  }
}

void ChrVReadWav(void)
{
  int num = cv1->sprite_Body->GetNumSounds();
  ChrVAllocWavMemory(num);

  char fname[FILENAME_MAX];
  for (int i=0; i<num; i++) {
    strcpy(fname, "data\\wav\\");
    strncat(fname, cv1->sprite_Body->GetSoundFN(i), sizeof(fname));
    pak1->Uncomp2tmp(fname);
    if (pak1->tlen == 0)  continue;
    *(p_wav + i) = (BYTE *)malloc(pak1->tlen);
    if (*(p_wav + i)) {
      memcpy(*(p_wav + i), pak1->tdata, pak1->tlen);
    }
  }
}

void ChrVAllocWavMemory(int num)
{
  if (numwav > 0) {
    ChrVFreeWavMemory();
  }

  numwav = num;
  p_wav = (BYTE **)malloc(sizeof(BYTE *) * numwav);
  for (int i=0; i<numwav; i++) {
    *(p_wav + i) = NULL;
  }
}

void ChrVFreeWavMemory(void)
{
  for (int i=0; i<numwav; i++) {
    free(*(p_wav + i));
  }
  free(p_wav);
  numwav = 0;
}

void ChrVSetBGColor(HWND hwndDlg)
{
  CHOOSECOLOR cc; 

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwndDlg;
  cc.Flags = CC_FULLOPEN | CC_RGBINIT;
  cc.lpCustColors = &custColor[0];
  cc.rgbResult = 0;
  if (ChooseColor(&cc)) {
    cv1->colBG = cc.rgbResult;
  }

  wglMakeCurrent(ThisDC, ThisRC);
  cv1->InitOpenGL();
  SwapBuffers(ThisDC);
  wglMakeCurrent(ThisDC, 0);

  ChrVDrawSprites();
}

INT_PTR CALLBACK DialogProcChrV(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT ret;

  switch (uMsg) {
  case WM_INITDIALOG:
    ChrVInitOpenGL(hwndDlg);
    ChrVInitControls(hwndDlg);
    return TRUE;
  case WM_CLOSE:
    ExitCharacterViewer(hwndDlg, IDCLOSE);
    return TRUE;
  case WM_PAINT:
    DefWindowProc(hwndDlg, uMsg, wParam, lParam);
    if (cv1->sprready) {
      ChrVDrawSprites();
    }
    return TRUE;
  case WM_COMMAND:
    switch (HIWORD(wParam)) {
    case CBN_DROPDOWN:
      RECT r;
      GetWindowRect(GetDlgItem(hwndDlg, LOWORD(wParam)), &r);
      SetWindowPos(GetDlgItem(hwndDlg, LOWORD(wParam)), 0, 0, 0, 
                   r.right-r.left, (r.bottom-r.top)*12, SWP_NOMOVE | SWP_NOZORDER);
      return TRUE;
    case CBN_SELCHANGE:
      ChrVSelectCombo(hwndDlg, LOWORD(wParam));
      return TRUE;
    }
    switch (LOWORD(wParam)) {
    case IDC_BTNDIRECTION0:
    case IDC_BTNDIRECTION1:
    case IDC_BTNDIRECTION2:
    case IDC_BTNDIRECTION3:
    case IDC_BTNDIRECTION4:
    case IDC_BTNDIRECTION5:
    case IDC_BTNDIRECTION6:
    case IDC_BTNDIRECTION7:
      ChrVProcBTNPushDirection(hwndDlg, LOWORD(wParam));
      return TRUE;
    case IDC_RBPC:
      CheckRadioButton(hwndDlg, IDC_RBPC, IDC_RBNPC, IDC_RBPC);
      ChrVEnableComboAlmost(hwndDlg, true);
      ChrVInitControlsSub(hwndDlg);
      return TRUE;
    case IDC_RBMOB:
      CheckRadioButton(hwndDlg, IDC_RBPC, IDC_RBNPC, IDC_RBMOB);
      ChrVEnableComboAlmost(hwndDlg, false);
      ChrVInitControlsSub(hwndDlg);
      return TRUE;
    case IDC_RBNPC:
      CheckRadioButton(hwndDlg, IDC_RBPC, IDC_RBNPC, IDC_RBNPC);
      ChrVEnableComboAlmost(hwndDlg, false);
      ChrVInitControlsSub(hwndDlg);
      return TRUE;
    case IDC_RBMALE:
      CheckRadioButton(hwndDlg, IDC_RBMALE, IDC_RBFEMALE, IDC_RBMALE);
      ChrVInitControlsSub(hwndDlg);
      return TRUE;
    case IDC_RBFEMALE:
      CheckRadioButton(hwndDlg, IDC_RBMALE, IDC_RBFEMALE, IDC_RBFEMALE);
      ChrVInitControlsSub(hwndDlg);
      return TRUE;
    case IDCLOSE:
      ExitCharacterViewer(hwndDlg, IDCLOSE);
      return TRUE;
    case IDC_BTNAUTO:
      ret = SendDlgItemMessage(hwndDlg, IDC_BTNSTOP, BM_GETSTATE, 0, 0);
      if (ret == BST_PUSHED) {
        SendDlgItemMessage(hwndDlg, IDC_BTNSTOP, BM_SETSTATE, FALSE, 0);
      }
      SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_SETSTATE, TRUE, 0);
      autoPlay = true;
      SetTimer(hwndDlg, CHRV_TIMER1, cv1->GetTimeInterval(), ChrVTimerProc);
      return TRUE;
    case IDC_BTNSTOP:
      ret = SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_GETSTATE, 0, 0);
      if (ret == BST_PUSHED) {
        SendDlgItemMessage(hwndDlg, IDC_BTNAUTO, BM_SETSTATE, FALSE, 0);
      }
      autoPlay = false;
      SendDlgItemMessage(hwndDlg, IDC_BTNSTOP, BM_SETSTATE, TRUE, 0);
      KillTimer(hwndDlg, CHRV_TIMER1);
      return TRUE;
    case IDC_BTNRESETVIEW:
      cv1->SetCameraDefault();
      ChrVDrawSprites();
      return TRUE;
    case IDC_BTNTOEASY:
      ExitCharacterViewer(hwndDlg, IDC_BTNTOEASY);
      return TRUE;
    case IDC_BTNBG:
      ChrVSetBGColor(hwndDlg);
      return TRUE;
    default:
      return FALSE;
    }
    break;
  case WM_MBUTTONDOWN:
    cv1->old_mp.x = LOWORD(lParam);
    cv1->old_mp.y = HIWORD(lParam);
    ClientToScreen(hwndDlg, &cv1->old_mp);
    if (ChrVCheckInGLArea(hwndDlg, &cv1->old_mp)) {
      RECT r;
      GetWindowRect(GetDlgItem(hwndDlg, IDC_GLAREA), &r);
      cv1->first_mp.x = cv1->old_mp.x - r.left;
      cv1->first_mp.y = cv1->old_mp.y - r.top;
      cv1->inMDragging = true;
      SetCapture(hwndDlg);
      SetCursor(updownCursor);
    } else {
      cv1->inMDragging = false;
    }
    return TRUE;
  case WM_MBUTTONUP:
    if (cv1->inMDragging) {
      SetCursor(defCursor);
      ReleaseCapture();
    }
    cv1->inMDragging = false;
    return TRUE;
  case WM_RBUTTONDOWN:
    cv1->old_mp.x = LOWORD(lParam);
    cv1->old_mp.y = HIWORD(lParam);
    ClientToScreen(hwndDlg, &cv1->old_mp);
    if (ChrVCheckInGLArea(hwndDlg, &cv1->old_mp)) {
      RECT r;
      GetWindowRect(GetDlgItem(hwndDlg, IDC_GLAREA), &r);
      cv1->first_mp.x = cv1->old_mp.x - r.left;
      cv1->first_mp.y = cv1->old_mp.y - r.top;
      cv1->inRDragging = true;
      SetCapture(hwndDlg);
      SetCursor(crossCursor);
    } else {
      cv1->inRDragging = false;
    }
    return TRUE;
  case WM_RBUTTONUP:
    if (cv1->inRDragging) {
      SetCursor(defCursor);
      ReleaseCapture();
    }
    cv1->inRDragging = false;
    return TRUE;
  case WM_MOUSEMOVE:
    if (!cv1->inMDragging && !cv1->inRDragging)  return FALSE;
    cv1->new_mp.x = LOWORD(lParam);
    cv1->new_mp.y = HIWORD(lParam);
    ClientToScreen(hwndDlg, &cv1->new_mp);
    if (cv1->inMDragging) {
      ChrVGLZoom();
    } else if (cv1->inRDragging) {
      ChrVGLPan();
    }
    cv1->old_mp.x = cv1->new_mp.x;
    cv1->old_mp.y = cv1->new_mp.y;
    return TRUE;
  case WM_HSCROLL:
    if (LOWORD(wParam) == SB_ENDSCROLL)  return FALSE;
    ChrVWMHScroll(hwndDlg, wParam);
    return TRUE;
  default:
    return FALSE;
  }

  return FALSE;
}

void CALLBACK ChrVTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  if (cv1->sprready) {
    cv1->curPat++;
    if (cv1->curPat == cv1->numPat)  cv1->curPat = 0;
    ChrVSetScrollBar(hwnd, 1, cv1->numPat, cv1->curPat+1);
    ChrVDrawSprites();
  }
}

void ChrVWMHScroll(HWND hwnd, WPARAM wParam)
{
  switch (LOWORD(wParam)) {
  case SB_LINEDOWN:
    cv1->curPat++;
    break;
  case SB_PAGEDOWN:
    if (cv1->curPat+5 > cv1->numPat-1) {
      cv1->curPat = cv1->numPat-1;
    } else {
      cv1->curPat += 5;
    }
    break;
  case SB_LINEUP:
    cv1->curPat--;
    break;
  case SB_PAGEUP:
    if (cv1->curPat-5 < 0) {
      cv1->curPat = 0;
    } else {
      cv1->curPat -= 5;
    }
    break;
  case SB_THUMBTRACK:
    cv1->curPat = HIWORD(wParam)-1; 
    ChrVDrawSprites();
    break;
  default:
    return;
  }
  if (cv1->curPat+1 > cv1->numPat)  cv1->curPat = 0;
  if (cv1->curPat < 0) cv1->curPat = cv1->numPat - 1;
  ChrVSetScrollBar(hwnd, 1, cv1->numPat, cv1->curPat+1);
  ChrVDrawSprites();
}
