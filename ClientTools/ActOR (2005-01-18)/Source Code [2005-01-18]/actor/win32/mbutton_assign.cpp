#include "mbutton_assign.h"

void OpenDialog_MouseButtonAssign(void)
{
  HINSTANCE hInst;
  hInst = (HINSTANCE)GetWindowLong(hwndMain, GWL_HINSTANCE);

  DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGMBASSIGN), hwndMain, DialogProcMBAssign);
}

void MBASetInitial(HWND hwnd)
{
  if (mouseAssign.pan == MK_LBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
  } else if (mouseAssign.rot == MK_LBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
  } else if (mouseAssign.zoom == MK_LBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);
  }
  if (mouseAssign.pan == MK_MBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO4, IDC_RADIO6, IDC_RADIO4);
  } else if (mouseAssign.rot == MK_MBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO4, IDC_RADIO6, IDC_RADIO5);
  } else if (mouseAssign.zoom == MK_MBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO4, IDC_RADIO6, IDC_RADIO6);
  }
  if (mouseAssign.pan == MK_RBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO7, IDC_RADIO9, IDC_RADIO7);
  } else if (mouseAssign.rot == MK_RBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO7, IDC_RADIO9, IDC_RADIO8);
  } else if (mouseAssign.zoom == MK_RBUTTON) {
    CheckRadioButton(hwnd, IDC_RADIO7, IDC_RADIO9, IDC_RADIO9);
  }

  SetWindowText(hwnd, _("Reassign mouse button"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRLABEL1), _("L button"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRLABEL2), _("M button"));
  SetWindowText(GetDlgItem(hwnd, IDC_GRLABEL3), _("R button"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO1), _("Move"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO2), _("Rot"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO3), _("Zoom"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO4), _("Move"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO5), _("Rot"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO6), _("Zoom"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO7), _("Move"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO8), _("Rot"));
  SetWindowText(GetDlgItem(hwnd, IDC_RADIO9), _("Zoom"));
  SetWindowText(GetDlgItem(hwnd, IDCANCEL), _("Cancel"));

}

void MBAProcRadio(HWND hwnd, WORD ID)
{
  int id1 = (ID - IDC_RADIO1) / 3;
  int id = ID - IDC_RADIO1;
  int ca[3][2] = {{1, 2}, {-1, 1}, {-2, -1}};
  int cb[3][2] = {{3, 6}, {-3, 3}, {-6, -3}};

  if (ID < IDC_RADIO4) {
    CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, ID);
  } else if (ID < IDC_RADIO7) {
    CheckRadioButton(hwnd, IDC_RADIO4, IDC_RADIO6, ID);
  } else {
    CheckRadioButton(hwnd, IDC_RADIO7, IDC_RADIO9, ID);
  }
  // exclusive procedure
  if (SendDlgItemMessage(hwnd, ID+cb[id1][0], BM_GETSTATE, 0, 0) == BST_CHECKED) {
    for (int i=0; i<2; i++) {
      if (SendDlgItemMessage(hwnd, ID+cb[id1][1]+ca[id%3][i], BM_GETSTATE, 0, 0) != BST_CHECKED) {
        break;
      }
    }
    CheckRadioButton(hwnd, IDC_RADIO1 + id1*3 + cb[id1][0], IDC_RADIO1 + id1*3 + cb[id1][0] + 2, ID + cb[id1][0] + ca[id%3][i]);
  } else if (SendDlgItemMessage(hwnd, ID+cb[id1][1], BM_GETSTATE, 0, 0) == BST_CHECKED) {
    for (int i=0; i<2; i++) {
      if (SendDlgItemMessage(hwnd, ID+cb[id1][0]+ca[id%3][i], BM_GETSTATE, 0, 0) != BST_CHECKED) {
        break;
      }
    }
    CheckRadioButton(hwnd, IDC_RADIO1 + id1*3 + cb[id1][1], IDC_RADIO1 + id1*3 + cb[id1][1] + 2, ID + cb[id1][1] + ca[id%3][i]);
  }
}

void MBASetState(HWND hwnd)
{
  if (SendDlgItemMessage(hwnd, IDC_RADIO1, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.pan = MK_LBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO2, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.rot = MK_LBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO3, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.zoom = MK_LBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO4, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.pan = MK_MBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO5, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.rot = MK_MBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO6, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.zoom = MK_MBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO7, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.pan = MK_RBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO8, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.rot = MK_RBUTTON;
  }
  if (SendDlgItemMessage(hwnd, IDC_RADIO9, BM_GETSTATE, 0, 0) == BST_CHECKED) {
    mouseAssign.zoom = MK_RBUTTON;
  }

  EndDialog(hwnd, 0);
}

INT_PTR CALLBACK DialogProcMBAssign(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    MBASetInitial(hwndDlg);
    return TRUE;
  case WM_CLOSE:
    EndDialog(hwndDlg, 0);
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      MBASetState(hwndDlg);
      return TRUE;
    case IDCANCEL:
      EndDialog(hwndDlg, 0);
      return TRUE;
    case IDC_RADIO1:
    case IDC_RADIO2:
    case IDC_RADIO3:
    case IDC_RADIO4:
    case IDC_RADIO5:
    case IDC_RADIO6:
    case IDC_RADIO7:
    case IDC_RADIO8:
    case IDC_RADIO9:
      MBAProcRadio(hwndDlg, LOWORD(wParam));
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
