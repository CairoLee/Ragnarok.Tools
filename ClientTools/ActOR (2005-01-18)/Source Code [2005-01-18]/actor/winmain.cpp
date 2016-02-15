#include <stdio.h>
#include <direct.h>
#include <zlib.h>

#include "winmain.h"
#include "resource.h"

#include "../common/relocate/progname.h"
#include "../common/relocate/relocatable.h"

#define MAINWIN_WIDTH 650
#define MAINWIN_HEIGHT 450

#define THIS_APP_NAME "actOR"

#include "../EULA_comp.z"

CActor *actor;
bool agreement;
MOUSE_ASSIGN mouseAssign;
DWORD colorBG, colorSelect, colorLine;
char appPath[FILENAME_MAX];
char lastDir[MAX_PATH];
COLORREF custColor[16];
DWORD winVersion;
bool warningUndoFE;
bool warningUndoSPO;

HWND hwndMain;
HCURSOR defCursor;
HCURSOR updownCursor;
HCURSOR grubCursor;
HFONT defFont;

extern "C" {
  extern 
#ifdef WIN32
  __declspec(dllimport)
#endif
  const char *locale_charset(void);
}
char NativeLANG[20];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  SetTextDomain(__argv[0]);

  WNDCLASS wc;
  MSG msg;

  actor = new CActor();

  GetFullPathName(__argv[0], sizeof(appPath), appPath, NULL);
  *(strrchr(appPath, '\\')) = '\0';
  GETCWD(lastDir, sizeof(lastDir));

  ReadPreference();
  if (agreement == false) {
    agreement = GetAgreement();
    if (agreement == false) {
      return 0;
    }
  }
  actor->SetColors(colorBG, colorSelect, colorLine);

  {
    OSVERSIONINFO ovi;
    ovi.dwOSVersionInfoSize = sizeof(ovi);
    GetVersionEx(&ovi);
    winVersion = ovi.dwMajorVersion;
  }

  if (!hPrevInstance) {
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszMenuName = "MAINMENU";
    wc.lpszClassName = THIS_APP_NAME;
    if (!RegisterClass(&wc)) return FALSE;
  }

  hwndMain = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,
                            THIS_APP_NAME, THIS_APP_NAME,
                            WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            MAINWIN_WIDTH, MAINWIN_HEIGHT,
                            NULL, NULL, hInstance, NULL);
  if (hwndMain == NULL)  return FALSE;
  ChangeMenuString();

  BuildScreen(hwndMain);
  RepositionControls();
  CreateDialogFrameEdit(hInstance, hwndMain);
  CreateDialogActCopy(hInstance, hwndMain);

  ShowWindow(hwndMain, SW_SHOW);
  UpdateWindow(hwndMain);

  DragAcceptFiles(hwndMain, TRUE);
 
  defCursor = LoadCursor(NULL, IDC_ARROW);
  updownCursor = LoadCursor(NULL, IDC_SIZENS);
  grubCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSORGRUB));

  if (__argc == 2) {
    OpenDialog_ReadFile(__argv[1]);
  }

  HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (hwndFrameE && IsDialogMessage(hwndFrameE , &msg)) continue;
    if (!TranslateAccelerator(hwndMain, hAccel, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  WritePreference();

  return (int)msg.wParam;
}

bool IsSprReady(void)
{
  if (actor == NULL || actor->IsSprReady() == false)  return false;

  return true;
}

bool IsActReady(void)
{
  if (actor == NULL || actor->IsActReady() == false)  return false;

  return true;
}

void SetSameColorUnusedPal(void)
{
  if (IsSprReady() == false)  return;

  CHOOSECOLOR cc; 

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwndMain;
  cc.Flags = CC_FULLOPEN | CC_RGBINIT;
  cc.lpCustColors = &custColor[0];
  cc.rgbResult = 0;
  if (ChooseColor(&cc)) {
    actor->SetSameColorUnusedPal(cc.rgbResult);
  }
}

void ProcPatternChange(bool drawAll)
{
  if (IsActReady() == false)  return;

  int cur = actor->curPat;
  int max = actor->GetNumPattern(actor->curAct) - 1;
  //frame all image
  if (drawAll) {
    DrawFrameAll();
  }
  //frame label
  char buf[10];
  SNPRINTF(buf, sizeof(buf), "%d / %d", cur+1, max+1);
  SetWindowText(hwndLabelFrame, buf);
  //frame scroll
  SetScrollPos_(hwndScr, cur, max);
  //table
  SetTableItemAll(actor->curAct, actor->curPat);
  //edit
  SetEditInterval(actor->GetTimeInterval());
  //combo snd
  bool tmpPlay = autoPlay;
  if (!tmpPlay) {
    autoPlay = true;
  }
  SendMessage(hwndSnd, CB_SETCURSEL, 1 + actor->GetSoundNo(actor->curAct, actor->curPat), 0);
  if (!tmpPlay) {
    autoPlay = tmpPlay;
  }
}

void ProcAfterReadSPR_ACT(void)
{
  if (IsActReady() == false)  return;

  CheckReference();

  char buf[128];
  //act combo
  SendMessage(hwndAct, CB_RESETCONTENT, 0, 0);
  int numact8 = actor->GetNumAction() / 8;
  if (actor->GetNumAction() % 8)  numact8++;
  for (int i=0; i<numact8; i++) {
    SNPRINTF(buf, sizeof(buf), "act%02d", i);
    SendMessage(hwndAct, CB_INSERTSTRING, -1, (LPARAM)buf);
  }
  SendMessage(hwndAct, CB_SETCURSEL, 0, 0);
  //act sound
  SetStringComboSnd();

  OnComboActChange();

  InvalidateRect(hwndLabelDir, NULL, TRUE);
}

void AuditAll(void)
{
  int actNo, patNo, sprId;
  int res;
  res = actor->AuditAll(&actNo, &patNo, &sprId);

  char mes[1024];
  char direc[20];
  char sdir[8][3] = {"S", "SW", "W", "NW", "N", "NE", "E", "SE"};
  SNPRINTF(direc, sizeof(direc), "%s", sdir[actNo % 8]);

  switch (res) {
  case ACTOR_AUDIT_NOERROR:
    MessageBox(0, _("No error and warning."), THIS_APP_NAME, MB_OK);
    break;
  case ACTOR_AUDIT_SPRMAXERROR:
    sprintf(mes, _("sprNo is over.\nAt act%02d direction %s frame %d sprite %d."),
      actNo / 8, _(direc), patNo, sprId);
    MessageBox(0, mes, _("warning"), MB_OK | MB_ICONWARNING);
    break;
  case ACTOR_AUDIT_0205SPRWIDTHHEIGHTERROR:
    res = MessageBox(0, _("Width, heigh of sprite is not matching between .spr with .act.\nDo you want to modify correct?"),
            _("warning"), MB_YESNO | MB_ICONWARNING);
    if (res == IDYES) {
      actor->ModifyCorrectSprWidthHeight();
    }
    break;
  case ACTOR_AUDIT_SOUNDMAXERROR:
    sprintf(mes, _("SoundNo is over.\nAt act%02d direction %s frame %d."),
      actNo / 8, _(direc), patNo);
    MessageBox(0, mes, _("warning"), MB_OK | MB_ICONWARNING);
    break;
  }
}

void OpenDialog_ReadFile(char *definedFilename)
{
  if (OnClose_Save() == false)  return;

  bool def = false;
  char fn[FILENAME_MAX];

  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  mmi.fState = MFS_DISABLED;
  SetMenuItemInfo(GetMenu(hwndMain), IDM_AUDIT, 0, &mmi);

  if (definedFilename != NULL) {
    strcpy(fn, definedFilename);
    if (strncmp(&fn[strlen(fn)-3], "act", 3) == 0) {
      strcpy(&fn[strlen(fn)-3], "spr");
    }
    def = true;
  }

  if (def == true || GetFileName(fn, sizeof(fn), _("Open spr or bmp"),
    "spr file or consective bmp file in 3 digits(*.spr, *.bmp)\0*.spr;*.bmp\0", true)) {
    actor->drawStop = true;
    delete actor;
    actor = new CActor();
    actor->SetColors(colorBG, colorSelect, colorLine);
    char ttl[1024+50];
    sprintf(ttl, "%s - %s", THIS_APP_NAME, fn);
    if (actor->ReadFileSPRorBMPAndAct(fn)) {
      //AuditAll();
      ProcAfterReadSPR_ACT();
      strcat(ttl, " & act");

      MENUITEMINFO mmi;
      mmi.cbSize = sizeof(mmi);
      mmi.fMask = MIIM_STATE;
      mmi.fState = MFS_ENABLED;
      SetMenuItemInfo(GetMenu(hwndMain), IDM_AUDIT, 0, &mmi);
      DrawMenuBar(hwndMain);
    }
    SetWindowText(hwndMain, ttl);
    actor->drawStop = false;
    UndoRedoInit();
    RepositionControls();
    RepositionControls();
    SetImages(0);
  }
}

void WriteAct(void)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), _("Save act"), "act file\0*.act\0", false)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".act");
    actor->WriteAct(fn);
  }
}

void WriteActText(void)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), _("Save act as text"), "txt file\0*.txt\0", false)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".txt");
    actor->WriteActText(fn);
  }
}

void WriteBmp(void)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), _("Save as bmp(&tga)"), 
                  "Basename of consective bmp file in 3 digits(i.e. foo of foo000.bmp)\0*.*\0", false)
      == false)  return;

  actor->WriteBmp256Tga32(fn , -1, false);
}

void WriteSpr(void)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), _("Save as spr"), "spr file\0*.spr\0", false)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".spr");
    actor->WriteSpr(fn);
  }
}

void ReplaceImage(void)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  if (actor->curImg < actor->GetNumPalImage()) {
    if (GetFileName(fn, sizeof(fn), _("Select replace file"), "256 color bitmap file(*.bmp)\0*.bmp\0", true)) {
      actor->ReplaceImage(fn);
    }
  } else {
    if (GetFileName(fn, sizeof(fn), _("Select replace file"), "32bit tga file(*.tga)\0*.tga\0", true)) {
      actor->ReplaceImage(fn);
    }
  }
  SetImages(actor->curImg);
}

void Write1Image(void)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  if (actor->curImg < actor->GetNumPalImage()) {
    if (GetFileName(fn, sizeof(fn), _("Save file"), "256 color bitmap file(*.bmp)\0*.bmp\0", false)) {
      actor->CheckIfAddSuffix(fn, sizeof(fn), ".bmp");
      actor->WriteBmp256Tga32(fn, actor->curImg, true);
    }
  } else {
    if (GetFileName(fn, sizeof(fn), _("Save file"), "32bit tga file(*.tga)\0*.tga\0", false)) {
      actor->CheckIfAddSuffix(fn, sizeof(fn), ".tga");
      actor->WriteBmp256Tga32(fn, actor->curImg, true);
    }
  }
}

void InsertImage(bool before)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  int pointInsert;
  if (before) {
    pointInsert = actor->curImg;
  } else {
    pointInsert = actor->curImg+1;
  }

  if (GetFileName(fn, sizeof(fn), _("Select file inserted"), "256color bitmap or 32bit tga file(*.bmp,*.tga)\0*.bmp;*.tga\0", true)) {
    if (strncmp(&fn[strlen(fn)-4], ".bmp", 4) == 0) {
      if (actor->GetNumFlatImage()) {
        if (pointInsert > actor->GetNumPalImage()) {
          MessageBox(0, _("Can't insert palleted image there."), THIS_APP_NAME, MB_OK | MB_ICONSTOP);
          return;
        }
      }
    } else if (strncmp(&fn[strlen(fn)-4], ".tga", 4) == 0) {
      if (pointInsert < actor->GetNumPalImage() - 1) {
        MessageBox(0, _("Can't insert flat image there."), THIS_APP_NAME, MB_OK | MB_ICONSTOP);
        return;
      }
    } else {
      MessageBox(0, _("Unhandled suffix."), THIS_APP_NAME, MB_OK | MB_ICONSTOP);
      return;
    }
    actor->InsertImage(pointInsert, fn);
    SetImages(actor->curImg);
    ProcPatternChange(true);
  }
}

void DeleteImage(void)
{
  if (IsSprReady() == false)  return;

  actor->DeleteCurrentImage();

  SetImages(actor->curImg);
  ProcPatternChange(true);
}

void CheckReference(void)
{
  char d[FILENAME_MAX];
  GETCWD(d, sizeof(d));

  actor->CheckReferenceAny(d, appPath);

  CheckRefHead();
  CheckRefBody();
  CheckRefEtc();
  CheckRefNeigh();

  actor->ChangeNeighborPoint(IDC_BTNDN2 - IDC_BTNDN0);
}

void SpritePan(BYTE key) 
{
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = NULL;
  if (numS) {
    iItems = (int *)malloc(numS * sizeof(int));
    GetSelectedTableItem(iItems, numS);

    actor->SpritePan(key, iItems, numS);

    free(iItems);
  }
  ProcPatternChange(false);
}

void SpriteZoom(bool equalRatio)
{
  int iItem;
  iItem = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED, 0));

  actor->SpriteZoom(iItem, equalRatio);
  ProcPatternChange(false);
}

void SpriteRot(void)
{
  int iItem;
  iItem = (int)SendMessage(hwndTable, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED, 0));

  actor->SpriteRot(iItem);
  ProcPatternChange(false);
}

void CameraPan(void)
{
  actor->CameraPan();
  DrawFrameAll();
}

void CameraZoom(void)
{
  actor->CameraZoom();
  DrawFrameAll();
}

void SelectColor(WORD ID)
{
  CHOOSECOLOR cc;

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwndMain;
  cc.Flags = CC_FULLOPEN | CC_RGBINIT;
  cc.lpCustColors = &custColor[0];
  if (ID == IDM_COLSELBG) {
    cc.rgbResult = colorBG;
  } else if (ID == IDM_COLSELSEL) {
    cc.rgbResult = colorSelect;
  } else {
    cc.rgbResult = colorLine;
  }
  if (ChooseColor(&cc)) {
    if (ID == IDM_COLSELBG) {
      colorBG = actor->colorBG = cc.rgbResult;
      SetupOpenGL(hwndAll, &hdcAll, &hglrcAll);
      SetupOpenGL(hwndFrmImg, &hdcFrmImg, &hglrcFrmImg);
    } else if (ID == IDM_COLSELSEL) {
      colorSelect = cc.rgbResult;
    } else {
      colorLine = cc.rgbResult;
    }
    SetImages(actor->curImg);
    DrawFrameAll();
  }
  actor->SetColors(colorBG, colorSelect, colorLine);
}

void ChangeStateHLine(void)
{
  actor->drawHLine = !actor->drawHLine;
  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  if (actor->drawHLine) {
    mmi.fState = MFS_CHECKED;
  } else {
    mmi.fState = MFS_UNCHECKED;
  }
  SetMenuItemInfo(GetMenu(hwndMain), IDM_HORIZON, 0, &mmi);
  DrawMenuBar(hwndMain);
  DrawFrameAll();
}

void ChangeStateVLine(void)
{
  actor->drawVLine = !actor->drawVLine;
  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  if (actor->drawVLine) {
    mmi.fState = MFS_CHECKED;
  } else {
    mmi.fState = MFS_UNCHECKED;
  }
  SetMenuItemInfo(GetMenu(hwndMain), IDM_VERTICAL, 0, &mmi);
  DrawMenuBar(hwndMain);
  DrawFrameAll();
}

void BuildScreen(HWND hwnd)
{
  InitCommonControls();

  for (int i=0; i<16; i++) {
    custColor[i] = 0;
  }
  mouseAssign.pan = MK_LBUTTON;
  mouseAssign.zoom = MK_MBUTTON;
  mouseAssign.rot = MK_RBUTTON;

  HINSTANCE hInst;
  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
  defFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "‚l‚r ‚oƒSƒVƒbƒN");
  BuildLabels(hInst);
  BuildComboBox(hInst);
  BuildButtons(hInst);
  BuildFrames(hInst);
  BuildScroll(hInst);
  BuildVSplit(hInst);
  BuildCheckboxes(hInst);
  BuildTable(hInst);
  BuildEdits(hInst);

  RepositionControls();

  warningUndoFE = true;
  warningUndoSPO = true;
  UndoEnable(false);
  RedoEnable(false);

  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  mmi.fState = MFS_CHECKED;
  SetMenuItemInfo(GetMenu(hwndMain), IDM_HORIZON, 0, &mmi);
  SetMenuItemInfo(GetMenu(hwndMain), IDM_VERTICAL, 0, &mmi);
  DrawMenuBar(hwndMain);

}

void DestructScreen(void)
{
  DestroyWindow(hwndFrameE);
  DeleteObject(defFont);
  SetWindowLong(hwndTable, GWL_WNDPROC, (LONG)wndprocOrigTable);
  wglDeleteContext(hglrcFrmImg);
  wglDeleteContext(hglrcAll);
  delete actor;
}

void RepositionControls(void)
{
  RECT rc;
  GetClientRect(hwndMain, &rc);

  RepositionLabels(&rc);
  RepositionComboBox(&rc);
  RepositionButtons(&rc);
  RepositionScroll(&rc);
  RepositionCheckBoxes(&rc);
  RepositionFrames(&rc);
  RepositionTable(&rc);
  RepositionVSplit(&rc);
  RepositionEdits(&rc);
}

void UndoRedoInit(void)
{
  actor->SetInvalidPrevCommand();
  UndoEnable(false);
  RedoEnable(false);
}

void DispUndoInitWarningFE(void)
{
  if (warningUndoFE) {
    int ret;
    ret = MessageBox(hwndMain, 
      _("If you edit frames, you can't undo.\n\nDoes redisplay this warning?\n\n(Redisplay this warning at next session.)"),
      THIS_APP_NAME, MB_ICONWARNING | MB_YESNO);
    if (ret == IDNO)  warningUndoFE = false;
  }
}

bool DispUndoInitWarningSPO(void)
{
  if (warningUndoSPO) {
    int ret;
    ret = MessageBox(hwndMain,
      _("If you reorder sprites, you can't undo.\n\nDoes redisplay this warning?\n\n(Redisplay this warning at next session.)\n\nIf you click cancel, not change priority."),
      THIS_APP_NAME, MB_ICONWARNING | MB_YESNOCANCEL);
    if (ret == IDCANCEL) {
      return false;
    }
    if (ret == IDNO) {
      warningUndoSPO = false;
    }
    return true;
  }
  return true;
}

void UndoEnable(bool enable)
{
  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  if (enable) {
    mmi.fState = MFS_ENABLED;
  } else {
    mmi.fState = MFS_DISABLED;
  }
  SetMenuItemInfo(GetMenu(hwndMain), IDM_UNDO, 0, &mmi);

  DrawMenuBar(hwndMain);
}

void RedoEnable(bool enable)
{
  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  if (enable) {
    mmi.fState = MFS_ENABLED;
  } else {
    mmi.fState = MFS_DISABLED;
  }
  SetMenuItemInfo(GetMenu(hwndMain), IDM_REDO, 0, &mmi);

  DrawMenuBar(hwndMain);
}

void Undo(void)
{
  actor->DoPrevUserCommand();
  if (actor->curRec == actor->startRec){
    UndoEnable(false);
  } else {
    UndoEnable(true);
  }
  RedoEnable(true);

  ProcPatternChange(true);
}

void Redo(void)
{
  actor->DoNextUserCommand();
  if (actor->curRec == actor->endRec) {
    RedoEnable(false);
  } else {
    RedoEnable(true);
  }
  UndoEnable(true);

  ProcPatternChange(true);
}

void BeginSpritePan(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MOVE_XY) 
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MOVE_EXY) ) {
    StoreSpritePan();
  }
}

void EndSpritePan(void)
{
  StoreSpritePan();
  UndoEnable(true);
}

void StoreSpritePan(void)
{
  USERCOMMAND tmpcmd;
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems, numS);

  int xOffs, yOffs;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      tmpcmd.type = COMTYPE_MOVE_EXY;
      actor->GetExtOffsPoint(&xOffs, &yOffs, actor->curAct, actor->curPat);
      tmpcmd.xyi[0] = xOffs;
      tmpcmd.xyi[1] = yOffs;
    } else {
      tmpcmd.type = COMTYPE_MOVE_XY;
      tmpcmd.sprId = *(iItems+i);
      actor->GetOffsPoint(&xOffs, &yOffs, actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyi[0] = xOffs;
      tmpcmd.xyi[1] = yOffs;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteMag(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MAG_XY) ) {
    StoreSpriteMag();
  }
}

void EndSpriteMag(void)
{
  StoreSpriteMag();
  UndoEnable(true);
}

void StoreSpriteMag(void)
{
  USERCOMMAND tmpcmd;
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems, numS);

  float xMag, yMag;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_MAG_XY;
      tmpcmd.sprId = *(iItems+i);
      xMag = actor->GetXMagValue(actor->curAct, actor->curPat, *(iItems+i));
      yMag = actor->GetYMagValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyf[0] = xMag;
      tmpcmd.xyf[1] = yMag;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteRot(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_ROT) ) {
    StoreSpriteRot();
  }
}

void EndSpriteRot(void)
{
  StoreSpriteRot();
  UndoEnable(true);
}

void StoreSpriteRot(void)
{
  USERCOMMAND tmpcmd;
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems, numS);

  DWORD rot;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_ROT;
      tmpcmd.sprId = *(iItems+i);
      rot = actor->GetRotValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = rot;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteMirror(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MIRROR) ) {
    StoreSpriteMirror();
  }
}

void EndSpriteMirror(void)
{
  StoreSpriteMirror();
  UndoEnable(true);
}

void StoreSpriteMirror(void)
{
  USERCOMMAND tmpcmd;
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems, numS);

  DWORD mir;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_MIRROR;
      tmpcmd.sprId = *(iItems+i);
      mir = actor->GetMirrorValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = mir;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteSaturation(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_SATURATION) ) {
    StoreSpriteSaturation();
  }
}

void EndSpriteSaturation(void)
{
  StoreSpriteSaturation();
  UndoEnable(true);
}

void StoreSpriteSaturation(void)
{
  USERCOMMAND tmpcmd;
  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems, numS);

  DWORD abgr;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_SATURATION;
      tmpcmd.sprId = *(iItems+i);
      abgr = actor->GetABGRValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = abgr;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

bool GetFileName(char *fn, int fnlen, char *title, char *filter, bool open)
{
  OPENFILENAME ofn;
  char curDir[MAX_PATH];
  GETCWD(curDir, sizeof(curDir));

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwndMain;
  fn[0] = '\0';
  ofn.lpstrFilter = filter;
  ofn.lpstrFile = fn;
  ofn.nMaxFile = fnlen;
  ofn.lpstrTitle = title;
  ofn.lpstrInitialDir = lastDir;

  BOOL ret;
  if (open) {
    ret = GetOpenFileName(&ofn);
  } else {
    ret = GetSaveFileName(&ofn);
  }
  GETCWD(lastDir, sizeof(lastDir));
  CHDIR(curDir);
  
  if (ret == TRUE) {
    return true;
  } else {
    return false;
  }
}

void ShowPopupFrameImage(LPARAM lParam)
{
  POINT mp;
  mp.x = LOWORD(lParam);
  mp.y = HIWORD(lParam);

  HMENU hmenu, hpmenu;
  hmenu = LoadMenu((HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE), "FRMIMG_MENU");
  hpmenu = GetSubMenu(hmenu, 0);

  char menuStr[256];
  int IDs[6] = {
    IDM_REPLACEIMAGE, IDM_WRITE1IMAGE, IDM_INSERTBEFORE, IDM_INSERTAFTER, IDM_DELETEIMAGE, IDM_PALLETSAME, // count 6
  };

  char menuiS[6][28] = {
    "Replace image...", "Write one image...", "Insert image before...", "Insert image after...", "Delete image", "Set same color empty pallet", // count 6 maxlen 27
  };

  MENUITEMINFO mii;
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STRING;
  mii.fType = MFT_STRING;
  for (int i=0; i < 6; i++) {
    strcpy(menuStr, _(menuiS[i]));
    mii.dwTypeData = menuStr;
    mii.cch = (UINT)strlen(menuStr);
    SetMenuItemInfo(hmenu, IDs[i], FALSE, &mii);
  }

  ClientToScreen(hwndMain, &mp);
  TrackPopupMenu(hpmenu, TPM_LEFTALIGN, mp.x, mp.y, 0, hwndMain, NULL);
  DestroyMenu(hmenu);
}

bool CheckInRect(HWND hwnd, LPARAM lParam)
{
  RECT rc;
  GetWindowRect(hwnd, &rc);

  POINT mp;
  mp.x = LOWORD(lParam);
  mp.y = HIWORD(lParam);
  ClientToScreen(hwndMain, &mp);
  if (rc.left <= mp.x && mp.x <= rc.right) {
    if (rc.top <= mp.y && mp.y <= rc.bottom) {
      return true;
    }
  }
  return false;
}

void SetMousePoint(LPARAM lParam)
{
  actor->oldMP.x = LOWORD(lParam);
  actor->oldMP.y = HIWORD(lParam);
  ClientToScreen(hwndMain, &actor->oldMP);
  RECT rc;
  GetWindowRect(hwndAll, &rc);
  actor->firstMP.x = actor->oldMP.x - rc.left;
  actor->firstMP.y = actor->oldMP.y - rc.top;
}

void OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
  SetFocus(hwndMain);
  SetCapture(hwndMain);
  if (CheckInRect(hwndVSplit, lParam)) {
    actor->dragState = DRAG_VSPLIT;
    LockWindowUpdate(GetDesktopWindow());
    actor->oldMP.y = HIWORD(wParam);
    DrawSplitter(actor->oldMP.y);
  }
  if (CheckInRect(hwndAll, lParam)) {
    SetMousePoint(lParam);
    int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
    if (numS == 0 || (wParam & MK_CONTROL)) {
      if (mouseAssign.pan == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
        SetCursor(grubCursor);
      } else if (mouseAssign.zoom == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
    } else {
      if (mouseAssign.pan == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        SetCursor(grubCursor);
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_LBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
    }
    ProcPatternChange(true);
  }
  if (CheckInRect(hwndFrmImg, lParam)) {
    actor->dragState = DRAG_IMAGE;
    SetMousePoint(lParam);
    int newno = GetClickImgNo(lParam);
    if (newno == -1) {
      actor->firstMP.x = 0;
      return;
    } else {
      actor->firstMP.x = newno;
    }
    SetCursor(grubCursor);
    SetImages(newno);
  }
}

void OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
  ReleaseCapture();
  if (actor->dragState == DRAG_IMAGE && (CheckInRect(hwndTable, lParam) || CheckInRect(hwndAll, lParam))) {
    if (IsActReady()) {
      if (DispUndoInitWarningSPO()) {
        actor->AddSpr(actor->curAct, actor->curPat, actor->firstMP.x);
      }
    }
  }

  switch (actor->dragState) {
  case DRAG_VSPLIT:
    DrawSplitter(actor->oldMP.y);
    LockWindowUpdate(NULL);
    actor->dragState = 0;
    RepositionControls();
    RepositionControls();
    return;
  case DRAG_TABLE_ROW:
    ReleaseCapture();
    actor->dragState = 0;
    return;
  case DRAG_ALL_S_PAN:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpritePan();
    return;
  case DRAG_ALL_S_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteMag();
    return;
  case DRAG_ALL_S_ROT:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteRot();
    return;
  case DRAG_ALL_C_PAN:
  case DRAG_ALL_C_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    return;
  case DRAG_IMAGE:
    actor->dragState = 0;
    if (IsSprReady()) {
      SetScrollPos_(hwndFrmImgScr, actor->curImg, actor->GetNumImage()-1);
    } else {
      SetScrollPos_(hwndFrmImgScr, actor->curImg, 0);
    }
    actor->curImg = actor->firstMP.x;
    SetCursor(defCursor);
    SetImages(actor->curImg);
    ProcPatternChange(true);
    break;
  }
}

void OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
  SetCapture(hwndMain);
  if (CheckInRect(hwndAll, lParam)) {
    SetMousePoint(lParam);
    int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
    if (numS == 0 || (wParam & MK_CONTROL)) {
      if (mouseAssign.pan == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
        SetCursor(grubCursor);
      } else if (mouseAssign.zoom == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
    } else {
      if (mouseAssign.pan == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        SetCursor(grubCursor);
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_RBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
    }
    ProcPatternChange(true);
  }
}

void OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
  ReleaseCapture();
  switch (actor->dragState) {
  case DRAG_ALL_S_PAN:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpritePan();
  case DRAG_ALL_S_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteMag();
    return;
  case DRAG_ALL_S_ROT:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteRot();
    return;
  case DRAG_ALL_C_PAN:
  case DRAG_ALL_C_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    return;
  }
  if (CheckInRect(hwndFrmImg, lParam)) {
    ShowPopupFrameImage(lParam);
  }
}

void OnMButtonDown(WPARAM wParam, LPARAM lParam)
{
  SetCapture(hwndMain);
  if (CheckInRect(hwndAll, lParam)) {
    SetMousePoint(lParam);
    int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
    if (numS == 0 || (wParam & MK_CONTROL)) {
      if (mouseAssign.pan == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
        SetCursor(grubCursor);
      } else if (mouseAssign.zoom == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
    } else {
      if (mouseAssign.pan == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        SetCursor(grubCursor);
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_MBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
    }
    ProcPatternChange(true);
  }
}

void OnMButtonUp(WPARAM wParam, LPARAM lParam)
{
  ReleaseCapture();
  switch (actor->dragState) {
  case DRAG_ALL_S_PAN:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpritePan();
  case DRAG_ALL_S_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteMag();
    return;
  case DRAG_ALL_S_ROT:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    EndSpriteRot();
    return;
  case DRAG_ALL_C_PAN:
  case DRAG_ALL_C_ZOOM:
    actor->dragState = 0;
    SetCursor(defCursor);
    ProcPatternChange(true);
    return;
  }
}

void OnMouseMove(WPARAM wParam, LPARAM lParam)
{
  RECT rc;
  actor->newMP.x = LOWORD(lParam);
  actor->newMP.y = HIWORD(lParam);
  switch (actor->dragState) {
  case DRAG_VSPLIT:
    DrawSplitter(actor->oldMP.y);
    DrawSplitter(actor->newMP.y);
    actor->oldMP.y = actor->newMP.y;
    GetClientRect(hwndMain, &rc);
    if (actor->newMP.y < 280)  return;
    vSplitPercent = actor->newMP.y * 100 / rc.bottom;
    return;
  case DRAG_TABLE_ROW:
    {
      //if mouse is out of range of table, dragging is end
      if (!CheckInRect(hwndTable, lParam)) {
        ReleaseCapture();
        actor->dragState = 0;
        return;
      }
    }
    return;
  case DRAG_ALL_S_PAN:
    ClientToScreen(hwndMain, &actor->newMP);
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return;
    SpritePan(NULL);
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return;
  case DRAG_ALL_S_ZOOM:
    ClientToScreen(hwndMain, &actor->newMP);
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return;
    if (wParam & MK_SHIFT) {
      SpriteZoom(true);
    } else {
      SpriteZoom(false);
    }
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return;
  case DRAG_ALL_S_ROT:
    ClientToScreen(hwndMain, &actor->newMP);
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return;
    SpriteRot();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return;
  case DRAG_ALL_C_PAN:
    ClientToScreen(hwndMain, &actor->newMP);
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return;
    CameraPan();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return;
  case DRAG_ALL_C_ZOOM:
    ClientToScreen(hwndMain, &actor->newMP);
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return;
    CameraZoom();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return;
  case DRAG_IMAGE:
    return;
  }
  if (CheckInRect(hwndVSplit, lParam)) {
    SetCursor(updownCursor);
  } else {
    SetCursor(defCursor);
  }
}

bool OnClose_Save(void)
{
  int ret;

  PostMessage(hwndMain, WM_COMMAND, IDC_BTNSTOP, 0);
  if (IsActReady()) {
    if (actor->GetModified()) {
      ret = MessageBox(hwndMain, "ACT‚ª•ÏX‚³‚ê‚Ü‚µ‚½B•Û‘¶‚µ‚Ü‚·‚©H", THIS_APP_NAME, MB_YESNOCANCEL | MB_ICONWARNING);
      if (ret == IDYES) {
        WriteAct();
      } else if (ret == IDCANCEL) {
        return false;
      }
    }
  }
  return true;
}

void OnKeyUp(WPARAM wParam, LPARAM lParam)
{
  if (wParam == VK_ESCAPE) {
    UnSelectSelection();
  }
}

void OnSysKeyUp(WPARAM wParam, LPARAM lParam)
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
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  MEASUREITEMSTRUCT *mi;

  switch (uMsg) {
  case WM_DROPFILES:
    {
      HDROP hDrop;
      char fn[FILENAME_MAX];
      hDrop = (HDROP)wParam;
      DragQueryFile(hDrop, 0, fn, sizeof(fn));
      OpenDialog_ReadFile(fn);
    }
    return 0;
  case WM_SIZE:
      RepositionControls();
      RepositionControls();
    return 0;
  case WM_GETMINMAXINFO:
    {
      MINMAXINFO *mi;
      mi = (MINMAXINFO *)lParam;
      mi->ptMinTrackSize.x = MAINWIN_WIDTH;
      mi->ptMinTrackSize.y = MAINWIN_HEIGHT;
    }
    return 0;
  case WM_LBUTTONDOWN:
    OnLButtonDown(wParam, lParam);
    return 0;
  case WM_LBUTTONUP:
    OnLButtonUp(wParam, lParam);
    return 0;
  case WM_RBUTTONDOWN:
    OnRButtonDown(wParam, lParam);
    return 0;
  case WM_RBUTTONUP:
    OnRButtonUp(wParam, lParam);
    return 0;
  case WM_MBUTTONDOWN:
    OnMButtonDown(wParam, lParam);
    return 0;
  case WM_MBUTTONUP:
    OnMButtonUp(wParam, lParam);
    return 0;
  case WM_MOUSEMOVE:
    OnMouseMove(wParam, lParam);
    return 0;
  case WM_KEYUP:
    OnKeyUp(wParam, lParam);
    return 0;
  case WM_SYSKEYUP:
    OnSysKeyUp(wParam, lParam);
    return 0;
  case WM_COMMAND:
    OnCommandCombo(wParam);
    switch (LOWORD(wParam)) {
    case IDC_BTNDI0:
    case IDC_BTNDI1:
    case IDC_BTNDI2:
    case IDC_BTNDI3:
    case IDC_BTNDI4:
    case IDC_BTNDI5:
    case IDC_BTNDI6:
    case IDC_BTNDI7:
      OnBtnDirectClick(LOWORD(wParam));
      break;
    case IDC_BTNPLAY:
      OnBtnPlay();
      break;
    case IDC_BTNSTOP:
      OnBtnStop();
      break;
    case IDC_BTNVIEWRESET:
      OnBtnViewReset();
      break;
    case IDC_BTNHEAD:
      OnBtnHead();
      break;
    case IDC_BTNBODY:
      OnBtnBody();
      break;
    case IDC_BTNETC:
      OnBtnEtc();
      break;
    case IDC_BTNNEI:
      OnBtnNeigh();
      break;
    case IDC_BTNDN0:
    case IDC_BTNDN1:
    case IDC_BTNDN2:
    case IDC_BTNDN3:
    case IDC_BTNDN4:
    case IDC_BTNDN5:
    case IDC_BTNDN6:
    case IDC_BTNDN7:
      OnBtnNeiDirec(LOWORD(wParam));
      break;
    case IDM_READ:
      OpenDialog_ReadFile(NULL);
      break;
    case IDM_WRITEBMP:
      WriteBmp();
      break;
    case IDM_WRITESPR:
      WriteSpr();
      break;
    case IDM_REPLACEIMAGE:
      ReplaceImage();
      break;
    case IDM_WRITE1IMAGE:
      Write1Image();
      break;
    case IDM_EXIT:
      SendMessage(hwnd, WM_CLOSE, 0, 0);
      break;
    case IDM_WRITEACT:
      WriteAct();
      break;
    case IDM_WRITEACTTEXT:
      WriteActText();
      break;
    case IDM_COLSELBG:
    case IDM_COLSELSEL:
    case IDM_COLSELLINE:
      SelectColor(LOWORD(wParam));
      break;
    case IDM_MBASSIGN:
      OpenDialog_MouseButtonAssign();
      break;
    case IDM_HORIZON:
      ChangeStateHLine();
      break;
    case IDM_VERTICAL:
      ChangeStateVLine();
      break;
    case IDM_SATRPLUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(0, true);
      EndSpriteSaturation();
      break;
    case IDM_SATRMINUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(0,false);
      EndSpriteSaturation();
      break;
    case IDM_SATGPLUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(1, true);
      EndSpriteSaturation();
      break;
    case IDM_SATGMINUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(1, false);
      EndSpriteSaturation();
      break;
    case IDM_SATBPLUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(2, true);
      EndSpriteSaturation();
      break;
    case IDM_SATBMINUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(2, false);
      EndSpriteSaturation();
      break;
    case IDM_SATAPLUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(3, true);
      EndSpriteSaturation();
      break;
    case IDM_SATAMINUS:
      BeginSpriteSaturation();
      ChangeSaturationABGRValue(3, false);
      EndSpriteSaturation();
      break;
    case IDM_MIRRORCHANGE:
      BeginSpriteMirror();
      ChangeMirror();
      EndSpriteMirror();
      break;
    case IDM_FRAMEEDIT:
      DispUndoInitWarningFE();
      ShowWindow(hwndFrameE, SW_SHOW);
      break;
    case IDM_PALLETSAME:
      SetSameColorUnusedPal();
      break;
    case IDM_INSERTBEFORE:
      InsertImage(true);
      break;
    case IDM_INSERTAFTER:
      InsertImage(false);
      break;
    case IDM_DELETEIMAGE:
      DeleteImage();
      break;
    case IDM_UNDO:
      Undo();
      break;
    case IDM_REDO:
      Redo();
      break;
    case IDM_ABOUT:
      OpenDialog_About();
      break;
    case IDM_ACTCOPY:
      DispUndoInitWarningFE();
      ShowWindow(hwndFrameAC, SW_SHOW);
      break;
    case IDM_PRIORREFSPR:
      OpenDialog_PriorRefSpr();
      ProcPatternChange(true);
      break;
    case IDM_AUDIT:
      AuditAll();
      break;
    case IDM_NEXTFRAME:
      OnHScroll((WPARAM)SB_LINEDOWN, (LPARAM)hwndScr);
      break;
    case IDM_PREVFRAME:
      OnHScroll((WPARAM)SB_LINEUP, (LPARAM)hwndScr);
      break;
    case IDM_SOUND:
      OpenDialog_ModifySound();
      break;
    default:
      break;
    }
    return 0;
  case WM_CLOSE:
    if (OnClose_Save() == false)  return 1;
    DestructScreen();
    DestroyWindow(hwnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_HSCROLL:
    OnHScroll(wParam, lParam);
    return 0;
  case WM_DRAWITEM:
    DRAWITEMSTRUCT *lpd;
    lpd = (LPDRAWITEMSTRUCT)lParam;
    OwnerDrawButtons(lpd);
    OwnerDrawLabels(lpd);
    OwnerDrawCheckBox(lpd);
    OwnerDrawComboBox(lpd);
    OwnerDrawFrames(lpd);
    return TRUE;
  case WM_NOTIFY:
    OnNotifyTable(wParam, lParam);
    break;
  case WM_MEASUREITEM:
    mi = (MEASUREITEMSTRUCT *)lParam;
    OnMeasureCombo(mi);
    return TRUE;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

VOID CALLBACK TimerFrameAll(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  actor->curPat++;
  int max = actor->GetNumPattern(actor->curAct);
  if (actor->curPat == max) {
    actor->curPat = 0;
  }
  ProcPatternChange(true);
}

#define CHAGNE_MENU_STRING_NUM 37
void ChangeMenuString(void)
{
  char menuStr[1024];

  HMENU menu;
  menu = GetMenu(hwndMain);

  char menuS[4][13] = {"File(&F)", "Edit(&E)", "View(&V)", "Change Value"};

  MENUITEMINFO mii;
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STRING;
  mii.fType = MFT_STRING;
  for (int i=0; i<4; i++) {
    strcpy(menuStr, _(menuS[i]));
    mii.dwTypeData = menuStr;
    mii.cch = (UINT)strlen(menuStr);
    SetMenuItemInfo(menu, i, TRUE, &mii);
  }

  int IDs[CHAGNE_MENU_STRING_NUM] = {
    IDM_READ, IDM_WRITEACT, IDM_WRITEBMP, IDM_WRITESPR, IDM_WRITEACTTEXT, IDM_AUDIT, IDM_EXIT, // count 7
    IDM_UNDO, IDM_REDO, IDM_REPLACEIMAGE, IDM_WRITE1IMAGE, IDM_INSERTBEFORE, IDM_INSERTAFTER, IDM_DELETEIMAGE, IDM_PALLETSAME, // count 8
    IDM_COLSELBG, IDM_COLSELSEL, IDM_COLSELLINE, IDM_HORIZON, IDM_VERTICAL, IDM_NEXTFRAME, IDM_PREVFRAME, // count 7
    IDM_SATRPLUS, IDM_SATRMINUS, IDM_SATGPLUS, IDM_SATGMINUS, IDM_SATBPLUS, IDM_SATBMINUS, IDM_SATAPLUS, IDM_SATAMINUS, IDM_MIRRORCHANGE, // count 9
    IDM_FRAMEEDIT, IDM_ACTCOPY, IDM_MBASSIGN, IDM_PRIORREFSPR, IDM_SOUND, IDM_ABOUT // count 6
    };

  char menuiS[CHAGNE_MENU_STRING_NUM][28] = {
    "Open...(&O)", "Save ACT...\tCtrl+S", "Save BMP...", "Save SPR...", "Save ACT as Text...", "Audit...", "Exit(&X)", // count 7 maxlen 19
    "Undo(&U)\tCtrl+Z", "Redo(&R)\tCtrl+Y", "Replace image...", "Write one image...", "Insert image before...", "Insert image after...", "Delete image", "Set same color empty pallet", // count 8 maxlen 27
    "Background color...", "Selection color...", "Indicator color...", "Horizontal line(&H)\tH", "Vertical line(&V)\tV", "Next frame\tALT+2", "Prev frame\tALT+1", // count 7 maxlen 21
    "Satur.R +5\tAlt+R", "Satur.R -5\tAlt+Shift+R", "Satur.G +5\tAlt+G", "Satur.G -5\tAlt+Shift+G", "Satur.B +5\tAlt+B", "Satur.B -5\tAlt+Shift+B", "Satur.A +5\tAlt+A", "Satur.A -5\tAlt+Shift+A", "Mirror spr\tM", // count 9 maxlen 22
    "Edit Frame...", "ACT(Dir) Copy...", "Change MouseBtn...(&M)", "Change priority ref. spr...", "Sound...", "About..." // count 5 maxlen 27
    };

  for (int i=0; i < CHAGNE_MENU_STRING_NUM; i++) {
    strcpy(menuStr, _(menuiS[i]));
    mii.dwTypeData = menuStr;
    mii.cch = (UINT)strlen(menuStr);
    SetMenuItemInfo(menu, IDs[i], FALSE, &mii);
  }

  MENUITEMINFO mmi;
  mmi.cbSize = sizeof(mmi);
  mmi.fMask = MIIM_STATE;
  mmi.fState = MFS_DISABLED;
  SetMenuItemInfo(GetMenu(hwndMain), IDM_AUDIT, 0, &mmi);

  DrawMenuBar(hwndMain);
}

void SetTextDomain(char *progname)
{
  setlocale(LC_ALL, getenv("LANG"));
#ifndef DISTRIBUTE
  strcpy(NativeLANG, locale_charset());
#endif
                                                                                
#if WIN32
  set_program_name(progname);
#endif
                                                                                
  char *ddir;
                                                                                
  ddir = getenv("TEXTDOMAINDIR");
  if ((ddir = getenv("TEXTDOMAINDIR")) != NULL) {
  } else {
#if WIN32
    bindtextdomain(MYTEXTDOMAIN, relocate(".\\share\\locale\\"));
#else
    bindtextdomain(MYTEXTDOMAIN, "/usr/local/share/locale");
#endif
  }
  textdomain(MYTEXTDOMAIN);
                                                                                
#ifdef WIN32
#ifdef USE_GTK
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
#endif
                                                                                
#ifndef WIN32
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
                                                                                
}


bool GetAgreement(void)
{
  agreementstring = (char *)malloc(AGREEMENTSTRING_ORIG_LEN+1);
  uLong realdestlen;
  uncompress((Bytef *)agreementstring, &realdestlen,
             (const Bytef *)agreementstringcomp, AGREEMENTSTRING_COMP_LEN);
  agreementstring[AGREEMENTSTRING_ORIG_LEN] = '\0';

  int reti;
  reti = MessageBox(hwndMain, _(agreementstring), _("EULA"), MB_ICONINFORMATION | MB_YESNO);
  free(agreementstring);
  
  bool ret;
  reti == IDYES ? ret = true : ret = false;

  return ret;
}

void GetPrefFileName(char *fname, int fname_size)
{
  char *env_home = getenv("HOME");
  if (env_home) {
    strcpy(fname, env_home);
#ifdef WIN32
    strcat(fname, "\\");
#else
    strcat(fname, "/");
#endif
    strcat(fname, "."THIS_APP_NAME".pref");
  } else {
#ifdef WIN32
    strcpy(fname, appPath);
    strcat(fname, "\\."THIS_APP_NAME".pref");
#else
    strcpy(fname, "."THIS_APP_NAME".pref");
#endif
  }
}

void ReadPreference()
{
  agreement = false;
  mouseAssign.pan = MK_LBUTTON;
  mouseAssign.zoom = MK_MBUTTON;
  mouseAssign.rot = MK_RBUTTON;
  colorBG = 0;
  colorSelect = 0xFF;
  colorLine = 0xFF;

  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "r")) == NULL) return;

  char buf[256];
  // 1st line is agreement
  fgets(buf, sizeof(buf), fp);
  atoi(buf) ? agreement = true : agreement = false;
  // 2nd line is panning button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.pan = atoi(buf);
  // 3rd line is zooming button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.zoom = atoi(buf);
  // 4th line is rotation button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.rot = atoi(buf);
  // 5th line is color BG
  fgets(buf, sizeof(buf), fp);
  colorBG = atoi(buf);
  // 6th line is color Selection
  fgets(buf, sizeof(buf), fp);
  colorSelect = atoi(buf);
  // 7th line is color indicator line
  fgets(buf, sizeof(buf), fp);
  colorLine = atoi(buf);

  fclose(fp);
}

void WritePreference()
{
  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "w")) == NULL) return;

  // 1st line is agreement
  if (agreement) {
    fprintf(fp, "1\n");
  } else {
    fprintf(fp, "0\n");
  }
  // 2nd line is panning button
  fprintf(fp, "%d\n", mouseAssign.pan);
  // 3rd line is zooming button
  fprintf(fp, "%d\n", mouseAssign.zoom);
  // 4th line is rotation button
  fprintf(fp, "%d\n", mouseAssign.rot);
  // 5th line is color BG
  fprintf(fp, "%d\n", colorBG);
  // 6th line is color Selection
  fprintf(fp, "%d\n", colorSelect);
  // 7th line is color indicator line
  fprintf(fp, "%d\n", colorLine);

  fclose(fp);
}
