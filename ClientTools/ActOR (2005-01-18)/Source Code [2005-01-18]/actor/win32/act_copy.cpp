#include "act_copy.h"

extern HWND hwndMain;
extern CActor *actor;

HWND hwndFrameAC;

void CreateDialogActCopy(HINSTANCE hInst, HWND hwnd)
{
  hwndFrameAC = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGACOPY), hwnd, DialogProcActCopy);
}

void ACDSetInitial(HWND hwnd)
{
  if (actor->IsReady() == false)  return;
  if (actor->GetNumAction() == 1) {
    MessageBox(0, _("This act has only one action."), _("warning"), MB_ICONWARNING | MB_OK);
    return;
  }
  SendDlgItemMessage(hwnd, IDC_RBACCW, BM_SETCHECK, BST_CHECKED, 0);
  ACDSetInitial_Combo(hwnd);
  ACDSetInitial_Radio(hwnd, 0, true);
  ACDSetInitial_Radio(hwnd, 0, false);

  SetWindowText(hwnd, _("act copy"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL1), _("select below"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL2), _("description"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL3), _("<-Any direction"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL4), _("<-Any direction"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL5), _("<-Copy like this.\n(clock wise)"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL6), _("<-Copy like this.\n(count clock wise)"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL7), _("'s"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL8), _("'s direction"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL9), _("'s"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL10), _("'s direction"));
  SetWindowText(GetDlgItem(hwnd, IDC_ACDLABEL11), _("-> Copy ONE direction"));
  SetWindowText(GetDlgItem(hwnd, ID_APPLYA), _("Apply"));
  SetWindowText(GetDlgItem(hwnd, ID_CLOSEA), _("Close"));
}

void ACDSetInitial_Combo(HWND hwnd)
{
  WORD cbs[4] = {IDC_CBAC11, IDC_CBAC12, IDC_CBAC2, IDC_CBAC3};
  int numact8 = actor->GetNumAction() / 8;
  if (actor->GetNumAction() % 8)  numact8++;

  char buf[20];
  for (int i=0; i<4; i++) {
    SendDlgItemMessage(hwnd, cbs[i], CB_RESETCONTENT, 0, 0);
    for (int j=0; j<numact8; j++) {
      SNPRINTF(buf, sizeof(buf), "act%02d", j);
      SendDlgItemMessage(hwnd, cbs[i], CB_INSERTSTRING, -1, (LPARAM)buf);
    }
    SendDlgItemMessage(hwnd, cbs[i], CB_SETCURSEL, 0, 0);
  }
}

void ACDSetInitial_Radio(HWND hwnd, int actno, bool isfrom)
{
  WORD baseID = isfrom ? IDC_RADIOAC0 : IDC_RADIOAC10;

  for (int i = baseID; i <= baseID+7; i++) {
    EnableWindow(GetDlgItem(hwnd, i), TRUE);
  }

  int n = actor->GetNumAction() - actno * 8;
  switch (n) {
  case 1:
    for (int i = baseID+1; i <= baseID+7; i++) {
        EnableWindow(GetDlgItem(hwnd, i), FALSE);
    }
    ACDRadioExclusive(hwnd, baseID, isfrom);
    break;
  case 2:
    EnableWindow(GetDlgItem(hwnd, baseID), TRUE);
    for (int i = baseID+2; i <= baseID+6; i++) {
      EnableWindow(GetDlgItem(hwnd, i), FALSE);
    }
    ACDRadioExclusive(hwnd, baseID+1, isfrom);
    break;
  case 4:
    for (int i = baseID; i <= baseID+7; i++) {
      if ((i-baseID) % 2 == 0) {
        EnableWindow(GetDlgItem(hwnd, i), FALSE);
      } else {
        EnableWindow(GetDlgItem(hwnd, i), TRUE);
      }
    }
    ACDRadioExclusive(hwnd, baseID+1, isfrom);
    break;
  default:
    for (int i = baseID + n; i <= baseID+7; i++) {
      EnableWindow(GetDlgItem(hwnd, i), FALSE);
    }
    ACDRadioExclusive(hwnd, baseID, isfrom);
    break;
  }
}

void ACDDetailComboChange(HWND hwnd, WORD ID)
{
  int i = (int)SendDlgItemMessage(hwnd, ID, CB_GETCURSEL, 0, 0);
  if (ID == IDC_CBAC11) {
    ACDSetInitial_Radio(hwnd, i, true);
  } else {
    ACDSetInitial_Radio(hwnd, i, false);
  }
  if (ID == IDC_CBAC11 || ID == IDC_CBAC12) {
    CheckRadioButton(hwnd, IDC_RBACCCW, IDC_RBACDETAIL, IDC_RBACDETAIL);
  } else if (ID == IDC_CBAC2) {
    CheckRadioButton(hwnd, IDC_RBACCCW, IDC_RBACDETAIL, IDC_RBACCW);
  } else if (ID == IDC_CBAC3) {
    CheckRadioButton(hwnd, IDC_RBACCCW, IDC_RBACDETAIL, IDC_RBACCCW);
  }
}

void ACDRadioExclusive(HWND hwnd, WORD ID, bool isfrom)
{
  CheckRadioButton(hwnd, IDC_RBACCCW, IDC_RBACDETAIL, IDC_RBACDETAIL);
  if (isfrom) {
    CheckRadioButton(hwnd, IDC_RADIOAC0, IDC_RADIOAC7, ID);
  } else {
    CheckRadioButton(hwnd, IDC_RADIOAC10, IDC_RADIOAC17, ID);
  }
}

void ACDProcAny(HWND hwnd)
{
  if (actor->IsActReady() == false)  return;
  if (autoPlay) {
    OnBtnStop();
    Sleep(500);
  }
  if (SendDlgItemMessage(hwnd, IDC_RBACCW, BM_GETCHECK, 0, 0) == BST_CHECKED) {
    ACDProcCopy4to8(hwnd, true);
  } else if (SendDlgItemMessage(hwnd, IDC_RBACCCW, BM_GETCHECK, 0, 0) == BST_CHECKED) {
    ACDProcCopy4to8(hwnd, false);
  } else if (SendDlgItemMessage(hwnd, IDC_RBACDETAIL, BM_GETCHECK, 0, 0) == BST_CHECKED) {
    ACDProcCopyDetail(hwnd);
  }
  ProcPatternChange(true);
}

void ACDProcCopy4to8(HWND hwnd, bool cw)
{
  int id = cw ? IDC_CBAC2 : IDC_CBAC3;
  int noact = (int)SendDlgItemMessage(hwnd, id, CB_GETCURSEL, 0, 0);
  int numact = actor->GetNumAction() - noact * 8;
  if (numact < 8) {
    MessageBox(0, _("This act doesn't have 8 actions."), _("error"), MB_ICONSTOP | MB_OK);
    return;
  }

  int baseact = noact * 8;
  for (int i=0; i<4; i++) {
    if (cw) {
      actor->CopyAction(baseact+i*2+1, baseact+i*2);
    } else {
      actor->CopyAction(baseact+i*2, baseact+i*2+1);
    }
  }
}

void ACDProcCopyDetail(HWND hwnd)
{
  int srcact;
  srcact = 8 * (int)SendDlgItemMessage(hwnd, IDC_CBAC11, CB_GETCURSEL, 0, 0);
  int i;
  for (i = IDC_RADIOAC0; i <= IDC_RADIOAC7; i++) {
    if (SendDlgItemMessage(hwnd, i, BM_GETCHECK, 0, 0) == BST_CHECKED) {
      break;
    }
  }
  srcact += i-IDC_RADIOAC0;

  int destact;
  destact = 8 * (int)SendDlgItemMessage(hwnd, IDC_CBAC12, CB_GETCURSEL, 0, 0);
  for (i = IDC_RADIOAC10; i <= IDC_RADIOAC17; i++) {
    if (SendDlgItemMessage(hwnd, i, BM_GETCHECK, 0, 0) == BST_CHECKED) {
      break;
    }
  }
  destact += i-IDC_RADIOAC10;

  actor->CopyAction(destact, srcact);
}

INT_PTR CALLBACK DialogProcActCopy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_SHOWWINDOW:
    if (wParam == TRUE) {
      ACDSetInitial(hwndDlg);
    }
    return 0;
  case WM_CLOSE:
    ShowWindow(hwndFrameAC, SW_HIDE);
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
      ACDDetailComboChange(hwndDlg, LOWORD(wParam));
      return TRUE;
    }
    switch (LOWORD(wParam)) {
    case IDC_RADIOAC0:
    case IDC_RADIOAC1:
    case IDC_RADIOAC2:
    case IDC_RADIOAC3:
    case IDC_RADIOAC4:
    case IDC_RADIOAC5:
    case IDC_RADIOAC6:
    case IDC_RADIOAC7:
      ACDRadioExclusive(hwndDlg, LOWORD(wParam), true);
      return TRUE;
    case IDC_RADIOAC10:
    case IDC_RADIOAC11:
    case IDC_RADIOAC12:
    case IDC_RADIOAC13:
    case IDC_RADIOAC14:
    case IDC_RADIOAC15:
    case IDC_RADIOAC16:
    case IDC_RADIOAC17:
      ACDRadioExclusive(hwndDlg, LOWORD(wParam), false);
      return TRUE;
    case ID_APPLYA:
      ACDProcAny(hwndDlg);
      return TRUE;
    case ID_CLOSEA:
      ShowWindow(hwndFrameAC, SW_HIDE);
      return TRUE;
    }
    break;
  }
  return 0;
}
