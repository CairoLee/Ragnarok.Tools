#include "about.h"

void OpenDialog_About(void)
{
  HINSTANCE hInst;
  hInst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);

  DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGABOUT), hwndMain, DialogProcAbout);
}

INT_PTR CALLBACK DialogProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    {
      char datetime[256];
      sprintf(datetime, "%s %s", __DATE__, __TIME__);
      SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTBUILD), datetime);

      SetWindowText(hwndDlg, _("About..."));
      SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTLABEL1), _("You can edit ACT of Arcturus and ** by this tool.\n\n"));
      SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTLABEL2), _("Build Date and Time:"));
      SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTLABEL3), "License: GNU GENERAL PUBLIC LICENSE version 2");
    }
    return TRUE;
  case WM_CLOSE:
    EndDialog(hwndDlg, 0);
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      EndDialog(hwndDlg, 0);
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
