#include "mod_sound.h"
void OpenDialog_ModifySound(void)
{
  if (actor->act == NULL)  return;

  HINSTANCE hInst;
  hInst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);

  DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGSOUND), hwndMain, DialogProcModSound);
}

void MSSetInitial(HWND hwnd)
{
  MSRedrawList(hwnd);

  SetWindowText(hwnd, _("Add/Delete sound"));
  SetWindowText(GetDlgItem(hwnd, IDC_BTN_SADD), _("Add last"));
  SetWindowText(GetDlgItem(hwnd, IDC_BTN_SDEL), _("Del. selected"));
  SetWindowText(GetDlgItem(hwnd, IDC_BTN_SUP), _("Up"));
  SetWindowText(GetDlgItem(hwnd, IDC_BTN_SDOWN), _("Down"));
}

void MSRedrawList(HWND hwnd)
{
  SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_RESETCONTENT, 0, 0);
  int num = actor->GetNumSounds();
  for (int i=0; i<num; i++) {
    SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_ADDSTRING, 0, (LPARAM)actor->GetSoundFN(i));
  }
  SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_SETCURSEL, -1, 0);
}

void MSProcAddLast(HWND hwnd)
{
  char fname[FILENAME_MAX];
  if (GetFileName(fname, sizeof(fname), _("Select file that is added"), "wav file\0*.wav\0", true)
      == false)  return;

  int len = (int)strlen(fname);
  int i;
  for (i=len; i>=0; i--) {
    if (fname[i] == '\\')  break;
  }
  if (i != 0)  i++;

  actor->AddSound(&fname[i]);
  MSRedrawList(hwnd);
}

void MSProcDelSel(HWND hwnd)
{
  int no = MSGetCurSel(hwnd);
  if (no < 0)  return;

  actor->DelSound(no);
  MSRedrawList(hwnd);
}

void MSProcUpSel(HWND hwnd)
{
  int no = MSGetCurSel(hwnd);
  if (no < 0)  return;

  if (no == 0)  return;
  actor->SwapSound(no, no-1);
  MSRedrawList(hwnd);
  SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_SETCURSEL, no-1, 0);
}

void MSProcDownSel(HWND hwnd)
{
  int no = MSGetCurSel(hwnd);
  if (no < 0)  return;

  if (no == actor->GetNumSounds()-1)  return;
  actor->SwapSound(no, no+1);
  MSRedrawList(hwnd);
  SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_SETCURSEL, no+1, 0);
}

int MSGetCurSel(HWND hwnd)
{
  int no = (int)SendDlgItemMessage(hwnd, IDC_LIST_SOUND, LB_GETCURSEL, 0, 0);
  if (no == LB_ERR)  return -1;
  return no;
}

INT_PTR CALLBACK DialogProcModSound(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    MSSetInitial(hwndDlg);
    return TRUE;
  case WM_CLOSE:
    EndDialog(hwndDlg, 0);
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDCLOSE:
      EndDialog(hwndDlg, 0);
      return TRUE;
    case IDC_BTN_SADD:
      MSProcAddLast(hwndDlg);
      return TRUE;
    case IDC_BTN_SDEL:
      MSProcDelSel(hwndDlg);
      return TRUE;
    case IDC_BTN_SUP:
      MSProcUpSel(hwndDlg);
      return TRUE;
    case IDC_BTN_SDOWN:
      MSProcDownSel(hwndDlg);
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
