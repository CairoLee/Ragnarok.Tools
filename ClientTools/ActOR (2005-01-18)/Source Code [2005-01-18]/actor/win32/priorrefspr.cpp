#include "mbutton_assign.h"

void OpenDialog_PriorRefSpr(void)
{
  HINSTANCE hInst;
  hInst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);

  DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGPRIOR), hwndMain, DialogProcPriorRefSpr);
}

void PRFSetInitial(HWND hwnd)
{
  if (actor->prioRefSpr.head == true) {
    CheckRadioButton(hwnd, IDC_RADIOHEADFRONT, IDC_RADIOHEADBACK, IDC_RADIOHEADFRONT);
  } else {
    CheckRadioButton(hwnd, IDC_RADIOHEADFRONT, IDC_RADIOHEADBACK, IDC_RADIOHEADBACK);
  }
  if (actor->prioRefSpr.body == true) {
    CheckRadioButton(hwnd, IDC_RADIOBODYFRONT, IDC_RADIOBODYBACK, IDC_RADIOBODYFRONT);
  } else {
    CheckRadioButton(hwnd, IDC_RADIOBODYFRONT, IDC_RADIOBODYBACK, IDC_RADIOBODYBACK);
  }
  if (actor->prioRefSpr.etc == true) {
    CheckRadioButton(hwnd, IDC_RADIOETCFRONT, IDC_RADIOETCBACK, IDC_RADIOETCFRONT);
  } else {
    CheckRadioButton(hwnd, IDC_RADIOETCFRONT, IDC_RADIOETCBACK, IDC_RADIOETCBACK);
  }
  if (actor->prioRefSpr.neigh == true) {
    CheckRadioButton(hwnd, IDC_RADIONEIGHFRONT, IDC_RADIONEIGHBACK, IDC_RADIONEIGHFRONT);
  } else {
    CheckRadioButton(hwnd, IDC_RADIONEIGHFRONT, IDC_RADIONEIGHBACK, IDC_RADIONEIGHBACK);
  }

  SetWindowText(hwnd, _("Change reference sprite priority"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRPLABEL1), _("head"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRPLABEL2), _("body"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRPLABEL3), _("etc"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRPLABEL4), _("neighbor"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOHEADFRONT), _("front"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOHEADBACK), _("back"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOBODYFRONT), _("front"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOBODYBACK), _("back"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOETCFRONT), _("front"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIOETCBACK), _("back"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIONEIGHFRONT), _("front"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIONEIGHBACK), _("back"));
}

void PRFProcRadioHead(HWND hwnd, WORD ID)
{
  CheckRadioButton(hwnd, IDC_RADIOHEADFRONT, IDC_RADIOHEADBACK, ID);

  if (ID == IDC_RADIOHEADFRONT) {
    actor->prioRefSpr.head = true;
  } else {
    actor->prioRefSpr.head = false;
  }
}

void PRFProcRadioBody(HWND hwnd, WORD ID)
{
  CheckRadioButton(hwnd, IDC_RADIOBODYFRONT, IDC_RADIOBODYBACK, ID);

  if (ID == IDC_RADIOBODYFRONT) {
    actor->prioRefSpr.body = true;
  } else {
    actor->prioRefSpr.body = false;
  }
}
void PRFProcRadioEtc(HWND hwnd, WORD ID)
{
  CheckRadioButton(hwnd, IDC_RADIOETCFRONT, IDC_RADIOETCBACK, ID);

  if (ID == IDC_RADIOETCFRONT) {
    actor->prioRefSpr.etc = true;
  } else {
    actor->prioRefSpr.etc = false;
  }
}
void PRFProcRadioNeigh(HWND hwnd, WORD ID)
{
  CheckRadioButton(hwnd, IDC_RADIONEIGHFRONT, IDC_RADIONEIGHBACK, ID);

  if (ID == IDC_RADIONEIGHFRONT) {
    actor->prioRefSpr.neigh = true;
  } else {
    actor->prioRefSpr.neigh = false;
  }
}

INT_PTR CALLBACK DialogProcPriorRefSpr(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    PRFSetInitial(hwndDlg);
    return TRUE;
  case WM_CLOSE:
    EndDialog(hwndDlg, 0);
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      EndDialog(hwndDlg, 0);
      return TRUE;
    case IDC_RADIOHEADFRONT:
    case IDC_RADIOHEADBACK:
      PRFProcRadioHead(hwndDlg, LOWORD(wParam));
      return TRUE;
    case IDC_RADIOBODYFRONT:
    case IDC_RADIOBODYBACK:
      PRFProcRadioBody(hwndDlg, LOWORD(wParam));
      return TRUE;
    case IDC_RADIOETCFRONT:
    case IDC_RADIOETCBACK:
      PRFProcRadioEtc(hwndDlg, LOWORD(wParam));
      return TRUE;
    case IDC_RADIONEIGHFRONT:
    case IDC_RADIONEIGHBACK:
      PRFProcRadioNeigh(hwndDlg, LOWORD(wParam));
      return TRUE;
    default:
      return FALSE;
    }
    break;
  default:
    return FALSE;
  }

  return FALSE;

}
