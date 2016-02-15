#include "frame_edit.h"

extern HWND hwndMain;
extern CActor *actor;

HWND hwndFrameE;

int numMaxFE;

void CreateDialogFrameEdit(HINSTANCE hInst, HWND hwnd)
{
  hwndFrameE = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGFRAME), hwnd, DialogProcFrameE);
}

void FEDSetInitial(HWND hwnd)
{
  CheckRadioButton(hwnd, IDC_RADIO_FDEL, IDC_RADIO_FSWAP, IDC_RADIO_FCOPY);

  if (actor->IsReady()) {
    numMaxFE = actor->GetNumPattern(actor->curAct);
    FEDSetInitial_Spin();
    char buf[10];
    sprintf(buf, "%d", numMaxFE);
    SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FSWAP2), buf);
  } else {
    SendDlgItemMessage(hwnd, IDC_SPIN_FDEL, UDM_SETRANGE32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FDEL, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FCOPY, UDM_SETRANGE32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FCOPY, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FSWAP1, UDM_SETRANGE32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FSWAP1, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FSWAP2, UDM_SETRANGE32, 0, 0);
    SendDlgItemMessage(hwnd, IDC_SPIN_FSWAP2, UDM_SETPOS32, 0, 0);
    SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FSWAP2), "1");
  }

  SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FDEL), "1");
  SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FCOPY), "1");
  SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FSWAP1), "1");

  SetWindowText(hwnd, _("Frame edit"));
  SetWindowText(GetDlgItem(hwnd, IDC_FELABEL1), _("frame is erased."));
  SetWindowText(GetDlgItem(hwnd, IDC_FELABEL2), _("frame is copied after this."));
  SetWindowText(GetDlgItem(hwnd, IDC_FELABEL3), _(","));
  SetWindowText(GetDlgItem(hwnd, IDC_FELABEL4), _("is swapped."));
  SetWindowText(GetDlgItem(hwnd, ID_BTN_FAPPLY), _("Apply"));
  SetWindowText(GetDlgItem(hwnd, IDCLOSE), _("Close"));
}

void FEDSetInitial_Spin(void)
{
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FDEL, UDM_SETRANGE32, 0, numMaxFE-1);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FDEL, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FCOPY, UDM_SETRANGE32, 0, numMaxFE-1);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FCOPY, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP1, UDM_SETRANGE32, 0, numMaxFE-1);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP1, UDM_SETPOS32, 0, 0);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP2, UDM_SETRANGE32, 0, numMaxFE-1);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP2, UDM_SETPOS32, 0, numMaxFE-1);
}

void FEDSpinProc(WPARAM wParam, HWND hwndscr)
{
  int pos;
  pos = HIWORD(wParam);
  char buf[10];
  if (pos < 0 ) pos = 0;
  if (pos >= numMaxFE)  pos = numMaxFE-1;
  sprintf(buf, "%d", pos+1);
  if (hwndscr == GetDlgItem(hwndFrameE, IDC_SPIN_FDEL)) {
    SetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FDEL), buf);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FDEL, UDM_SETPOS32, 0, pos);
  } else if (hwndscr == GetDlgItem(hwndFrameE, IDC_SPIN_FCOPY)) {
    SetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FCOPY), buf);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FCOPY, UDM_SETPOS32, 0, pos);
  } else if (hwndscr == GetDlgItem(hwndFrameE, IDC_SPIN_FSWAP1)) {
    SetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FSWAP1), buf);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP1, UDM_SETPOS32, 0, pos);
  } else if (hwndscr == GetDlgItem(hwndFrameE, IDC_SPIN_FSWAP2)) {
    SetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FSWAP2), buf);
    SendDlgItemMessage(hwndFrameE, IDC_SPIN_FSWAP2, UDM_SETPOS32, 0, pos);
  }
}

void FEDCheckInt(WORD ID)
{
  char buf[10];
  GetWindowText(GetDlgItem(hwndFrameE, ID), buf, sizeof(buf));
  int n = atoi(buf);
  if (n > numMaxFE)  n = numMaxFE;
  if (n < 1)  n = 1;
  sprintf(buf, "%d", n);
  SetWindowText(GetDlgItem(hwndFrameE, ID), buf);
  SendDlgItemMessage(hwndFrameE, ID-1, UDM_SETPOS32, 0, n-1);
}

void FEDProcAny(void)
{
  if (actor->IsActReady() == false)  return;
  if (autoPlay) {
    OnBtnStop();
    Sleep(500);
  }
  char bufA[10], bufB[10];
  int idA, idB;
  if (SendDlgItemMessage(hwndFrameE, IDC_RADIO_FDEL, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    if (numMaxFE == 1)  return;
    if (actor->curPat == numMaxFE-1)  actor->curPat--;
    GetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FDEL), bufA, sizeof(bufA));
    idA = atoi(bufA);
    actor->DelPat(actor->curAct, idA-1);
    numMaxFE--;
    FEDSetInitial_Spin();
    FEDCheckMaxInEdit();
  } else if (SendDlgItemMessage(hwndFrameE, IDC_RADIO_FCOPY, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    GetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FCOPY), bufA, sizeof(bufA));
    idA = atoi(bufA);
    actor->AddPat(actor->curAct, idA-1);
    for (int i=numMaxFE-1; i>=idA; i--) {
      actor->SwapPat(actor->curAct, i, i+1);
    }
    numMaxFE++;
    FEDSetInitial_Spin();
  } else if (SendDlgItemMessage(hwndFrameE, IDC_RADIO_FSWAP, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    GetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FSWAP1), bufA, sizeof(bufA));
    idA = atoi(bufA);
    GetWindowText(GetDlgItem(hwndFrameE, IDC_EDIT_FSWAP2), bufB, sizeof(bufB));
    idB = atoi(bufB);
    actor->SwapPat(actor->curAct, idA-1, idB-1);
  }
  UndoRedoInit();
  ProcPatternChange(true);
}

void FEDCheckMaxInEdit(void)
{
  char buf[10];
  int n;
  for (int i=IDC_EDIT_FDEL; i<=IDC_EDIT_FSWAP2; i+=2) {
    GetWindowText(GetDlgItem(hwndFrameE, i), buf, sizeof(buf));
    n = atoi(buf);
    if (n >= numMaxFE) {
      sprintf(buf, "%d", numMaxFE);
      SetWindowText(GetDlgItem(hwndFrameE, i), buf);
    }
  }
}

INT_PTR CALLBACK DialogProcFrameE(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_SHOWWINDOW:
    if (wParam == TRUE) {
      FEDSetInitial(hwndDlg);
    }
    return 0;
  case WM_CLOSE:
    ShowWindow(hwndFrameE, SW_HIDE);
    return TRUE;
  case WM_VSCROLL:
    if (LOWORD(wParam) == SB_THUMBPOSITION) {
      FEDSpinProc(wParam, (HWND)lParam);
    }
    return 0;
  case WM_COMMAND:
    switch (HIWORD(wParam)) {
    case EN_KILLFOCUS:
      switch (LOWORD(wParam)) {
      case IDC_EDIT_FDEL:
      case IDC_EDIT_FCOPY:
      case IDC_EDIT_FSWAP1:
      case IDC_EDIT_FSWAP2:
        FEDCheckInt(LOWORD(wParam));
        return 0;
      }
      return 0;
    }
    switch (LOWORD(wParam)) {
    case ID_BTN_FAPPLY:
      FEDProcAny();
      return 0;
    case IDCLOSE:
      ShowWindow(hwndFrameE, SW_HIDE);
      return TRUE;
    }
    break;
  }

  return 0;

}
