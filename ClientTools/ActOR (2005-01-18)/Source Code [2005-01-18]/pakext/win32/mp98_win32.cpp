#include <stdio.h>
#include <direct.h>

#include "../resource.h"
#include "mp98_win32.h"
#include "../../common/pak.h"

#ifdef VIOLATION_CODE
extern CPAK *pak1;
extern char *vsFilename;
extern char *listFilename;
extern int version_patch98;

void MakePatch98(void)
{
  HINSTANCE hinst;
  HWND chwnd;
  char ttl[512];

  if (vsFilename && listFilename) {
    MP98MakeMain(0, true);
    return;
  }

  GetConsoleTitle(ttl, sizeof(ttl));
  chwnd = FindWindowEx(0, 0, "ConsoleWindowClass", ttl);
  hinst = GetModuleHandle(NULL);

  DialogBox(hinst, MAKEINTRESOURCE(IDD_PATCH98), chwnd, (DLGPROC)DialogProcMP98);
}

void MP98OpenVSFile(HWND hwndDlg, int ID_DLGI)
{
  OPENFILENAME ofn;
  char fn[FILENAME_MAX];
  char dir[FILENAME_MAX];

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwndDlg;
  fn[0] = '\0';
  ofn.lpstrFile = fn;
  ofn.nMaxFile = sizeof(fn);
  ofn.lpstrTitle = "‘Î‰žƒtƒ@ƒCƒ‹";
  ofn.lpstrInitialDir = _getcwd(dir, sizeof(dir));

  if (!GetOpenFileName(&ofn)) return;

  SendDlgItemMessage(hwndDlg, ID_DLGI, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
}

void MP98MakeMain(HWND hwndDlg, bool flag)
{
  char filenamel[FILENAME_MAX], filenamev[FILENAME_MAX];
  FILE *fpl, *fpv;


  if (hwndDlg) {
    SendDlgItemMessage(hwndDlg, IDC_LISTFILE, WM_GETTEXT, sizeof(filenamel), (LPARAM)filenamel);
    SendDlgItemMessage(hwndDlg, IDC_VSFILES, WM_GETTEXT, sizeof(filenamev), (LPARAM)filenamev);
    if ((fpl = fopen(filenamev, "r")) == NULL) {
      char buf[FILENAME_MAX*2];
      sprintf(buf, "Can't open\r\n  %s\r\n", filenamel);
      SendDlgItemMessage(hwndDlg, IDC_CONFV, WM_SETTEXT, 0, (LPARAM)buf);
      return;
    }
    if ((fpv = fopen(filenamel, "r")) == NULL) {
      char buf[FILENAME_MAX*2];
      sprintf(buf, "Can't open\r\n  %s\r\n", filenamev);
      SendDlgItemMessage(hwndDlg, IDC_CONFV, WM_SETTEXT, 0, (LPARAM)buf);
      return;
    }
  } else {
    if ((fpl = fopen(vsFilename, "r")) == NULL) {
      printf("Can't open\r\n  %s\r\n", vsFilename);
      return;
    }
    if ((fpv = fopen(listFilename, "r")) == NULL) {
      printf("Can't open\r\n  %s\r\n", listFilename);
      return;
    }
  }


  MP98ParseAndExec(hwndDlg, fpl, fpv, flag);

  fclose(fpl);
  fclose(fpv);
}

void MP98ParseAndExec(HWND hwndDlg, FILE *fpl, FILE *fpv, bool flag)
{
  VSFILES *vf = NULL;
  int numvs = 0;
  int i;
  char *disp;

  MP98ReadAdd(fpl, fpv, &vf, &numvs);

  if (hwndDlg) {
    disp = (char *)malloc(sizeof(VSFILES)*numvs);
    *disp = 0;
    for (i=0; i<numvs; i++) {
      strcat(disp, (vf+i)->sjis);
      strcat(disp, "\r\n -> ");
      strcat(disp, (vf+i)->euckr);
      strcat(disp, "\r\n");
    }
    SendDlgItemMessage(hwndDlg, IDC_CONFV, WM_SETTEXT, 0, (LPARAM)disp);
    free(disp);
  }

  if (flag) {
    MP98MakePatchReal(hwndDlg, vf, numvs);
  }

  free(vf);
}

void MP98MakePatchReal(HWND hwndDlg, VSFILES *vf, int numvs)
{
  WORD v;
  int i;
  FILE *fp;
  BYTE *dat;
  int len;

  if (hwndDlg) {
    if (IsDlgButtonChecked(hwndDlg, IDC_VER1) == BST_CHECKED) {
      v = 0x0103;
    } else {
      v = 0x0200;
    }
  } else {
    if (version_patch98 == 1) {
      v = 0x0103;
    } else {
      v = 0x0200;
    }
  }
  pak1->MakePatchOpen("update.gpf", v);

  for (i=0; i<numvs; i++) {
    if ((fp = fopen((vf+i)->sjis, "rb")) == NULL) {
      continue;
    } else {
      fseek(fp, 0, SEEK_END);
      len = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      dat = (BYTE *)malloc(len);
      fread(dat, 1, len, fp);
      fclose(fp);
      pak1->MakePatchAdd((vf+i)->euckr, dat, len);
      free(dat);
    }
  }
  pak1->MakePatchClose();
}

void MP98ReadAdd(FILE *fpl, FILE *fpv, VSFILES **vf, int *numvs)
{
  int i;
  char buf[512];
  char *bufc;
  char kr[FILENAME_MAX];
  LFILES *lf = NULL;
  int numlf = 0;
  int lfc = 0;

  MP98GetList(fpl, &lf, &numlf);

  while (feof(fpv) == 0) {
    fgets(buf, sizeof(buf), fpv);
    if (strchr(buf, '\t') == NULL) {
      continue;
    } else {
      bufc = &buf[0];
      while (*bufc != '\t')  bufc++;
      *bufc = '\0';
      i = atoi(buf);
      strcpy(kr, bufc+1);
      if ((kr[strlen(kr)-1] == '\r') || (kr[strlen(kr)-1] == '\n')) {
        kr[strlen(kr)-1] = '\0';
      }
      if (i == (lf+lfc)->i) {
        *vf = (VSFILES *)realloc(*vf, sizeof(VSFILES)*((*numvs)+1));
        strcpy((*vf+*numvs)->euckr, kr);
        strcpy((*vf+*numvs)->sjis, (lf+lfc)->sjis);
        (*numvs)++;
        lfc++;
      }
    }
  }

  free(lf);
}

void MP98GetList(FILE *fp, LFILES **lf, int *numlf)
{
  char buf[512];
  char *bufc;
  int i;
  char jp[FILENAME_MAX];

  while (feof(fp) == 0) {
    fgets(buf, sizeof(buf), fp);
    if (strchr(buf, '\t') == NULL) {
      continue;
    } else {
      bufc = &buf[0];
      while (*bufc != '\t')  bufc++;
      *bufc = '\0';
      i = atoi(buf);
      strcpy(jp, bufc+1);
      if ((jp[strlen(jp)-1] == '\r') || (jp[strlen(jp)-1] == '\n')) {
        jp[strlen(jp)-1] = '\0';
      }
      *lf = (LFILES *)realloc(*lf, sizeof(LFILES) * (*numlf+1));
      (*lf+*numlf)->i = i;
      strcpy((*lf+*numlf)->sjis, jp);
      (*numlf)++;
    }
  }
  qsort(*lf, *numlf, sizeof(LFILES), MP98SortLFILES);
}

int MP98SortLFILES(const void *a, const void *b)
{
  if (((LFILES *)a)->i < ((LFILES *)b)->i) {
    return -1;
  } else if (((LFILES *)a)->i > ((LFILES *)b)->i) {
    return 1;
  } else {
    return 0;
  }
}

INT_PTR CALLBACK DialogProcMP98(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    {
      HWND hwnde;
      hwnde = GetDlgItem(hwndDlg, IDC_CONFV);
      LONG tmp;
      tmp = GetWindowLong(hwnde, GWL_STYLE);
      SetWindowLong(hwnde, GWL_STYLE, tmp | ES_MULTILINE);
    }
    SendDlgItemMessage(hwndDlg, IDC_VER1, BM_SETCHECK, BST_CHECKED, 0);
    return FALSE;
  case WM_CLOSE:
    EndDialog(hwndDlg, IDCLOSE);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_VSFILEREF:
      MP98OpenVSFile(hwndDlg, IDC_VSFILES);
      break;
    case IDC_LISTREF:
      MP98OpenVSFile(hwndDlg, IDC_LISTFILE);
      break;
    case IDC_CONF:
      MP98MakeMain(hwndDlg, false);
      break;
    case IDC_MP98EXEC:
      MP98MakeMain(hwndDlg, true);
      break;
    case IDCLOSE:
      EndDialog(hwndDlg, IDCLOSE);
      break;
    default:
      return FALSE;
    }
    break;
  default:
    return FALSE;
  }

  return TRUE;
}
#endif // VIOLATION_CODE
