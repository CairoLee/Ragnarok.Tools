#include <stdio.h>
#include <direct.h>

#include "../resource.h"
#include "easyext_win32.h"
#include "../../common/pak.h"

extern CPAK *pak1;
extern MY_FLAGS flag;

HWND hwndD;
LOCALTREE *ltree;
HTREEITEM hiSelected;
int numltree;

#define PAKTEMP "pakext_temp"
static char suffixEnableProc[][4] = {"bmp", "txt", "wav", "tga", "xml", "spr", "jpg"};

int EasyExt(int argc, char **argv)
{
  HINSTANCE hinst;
  HWND chwnd;
  char ttl[512];

  GetConsoleTitle(ttl, sizeof(ttl));
  chwnd = FindWindowEx(0, 0, "ConsoleWindowClass", ttl);
  hinst = GetModuleHandle(NULL);

  printf("Now initializing...\n");
  if (DialogBox(hinst, MAKEINTRESOURCE(IDD_EASYEXT), chwnd, (DLGPROC)DialogProcEasyExt) == IDCLOSE) {
    return 0;
  } else {
    return 2;
  }
}

void EEInitDialog(void)
{
  EESetTreeItemAll();
}

void EEExitDialog(int ID)
{
  EEDestroyLocalTree(ltree);
  free(ltree);
  EndDialog(hwndD, ID);

  char fname[FILENAME_MAX];
  for (int i=0; i<sizeof(suffixEnableProc) / 4; i++) {
    SNPRINTF(fname, sizeof(fname), "%s.%s", PAKTEMP, suffixEnableProc[i]);
    remove(fname);
  }
}

void EEDestroyLocalTree(LOCALTREE *lt)
{
  if (lt->child != -1)  EEDestroyLocalTree(ltree + lt->child);
  while (lt->next != -1) {
    free(lt->disp);
    free(lt->file);
    if (lt->next == -1) break;
    lt = ltree + lt->next;
  }
}

void EESetTreeItemAll(void)
{
  EESetRootLocalTree(pak1->NumEntry + 300);  // 300 : number of directory  (approx)
  ltree->hi = EESetRootTree();

  for (int i=0, j=1; i<pak1->NumEntry; i++) {
    if (i == pak1->NumEntry / 10 * j) {
      printf("tree item %d%% set...\n", j*10);
      j++;
    }
    if (pak1->Entries[i].type == 2)  continue;
    if (flag.useMatch) {
      if (strstr( pak1->Entries[i].fn, flag.strMatch ) == NULL)  continue;
    }
    EESetItemDirCheck(NULL, -1, pak1->Entries[i].fn);
  }
  SendDlgItemMessage(hwndD, IDC_TREE, TVM_EXPAND, TVE_EXPAND, (LPARAM)ltree->hi);
}

HTREEITEM EESetRootTree(void)
{
  TVINSERTSTRUCT tis;
  tis.hParent = TVI_ROOT;
  tis.hInsertAfter = TVI_LAST;
  tis.item.mask = TVIF_CHILDREN | TVIF_TEXT;
  tis.item.pszText = "data";
  tis.item.cchTextMax = 5;
  tis.item.cChildren = 1;
  return (HTREEITEM)SendDlgItemMessage(hwndD, IDC_TREE, TVM_INSERTITEM, 0, (LPARAM)&tis);
}

void EESetRootLocalTree(int num)
{
  ltree = (LOCALTREE *)malloc(sizeof(LOCALTREE)*num);
  ltree->root = -1;
  ltree->disp = (char *)malloc(5);  // data\0
  strcpy(ltree->disp, "data");
  ltree->file = (char *)malloc(5);  // data\0
  strcpy(ltree->file, "data");
  ltree->next = -1;
  ltree->child = -1;
  numltree = 1;
}

void EESetItemDirCheck(HTREEITEM hti, int lno, char *fname)
{
  char *st;
  char buf[FILENAME_MAX];
  HTREEITEM hi;

  int ver = (int)pak1->pakversion[0];
  if (ver <= 0) {  // pak or b1
    st = strchr(fname, '/');
  } else {
    st = strchr(fname, '\\');
  }
  if (lno == -1) {
    EESetItemDirCheck(ltree->hi, 0, st+1);
    return;
  }
  if (st) {  // include subdir
    strncpy(buf, fname, st-fname);
    buf[st-fname] = '\0';
    EESetItemMakeBranch(hti, lno, buf, fname);
  } else {
    hi = EESetItemFinalItem(hti, fname);
    EESetItemLocalFinalItem(hi, lno, fname);
  }
}

void EESetItemMakeBranch(HTREEITEM hti, int lno, char *node, char *fname)
{
  char *st;
  char buf[FILENAME_MAX];
  HTREEITEM hti_c;
  int lno_c;
  LOCALTREE *lt;

  int ver = (int)(pak1->pakversion[0]);
  if (ver <= 0) {  // pak or b1
    st = strchr(fname, '/');
  } else {
    st = strchr(fname, '\\');
  }
  strcpy(buf, _(node));
  if (EESetItemMakeBranchIsExist(hti, &hti_c, lno, &lno_c, node)) { // already exist
    EESetItemDirCheck(hti_c, lno_c, st+1);
  } else {
    hti_c = EEAddTreeItem(hti, buf, true);
    lno_c = EEAddLocalTreeItem(lno, hti_c, buf, node);
    if ((ltree+lno)->child == -1) {
      (ltree+lno)->child = lno_c;
    } else {
      lt = ltree + (ltree+lno)->child;
      while (lt->next != -1) {
        lt = ltree + lt->next;
      }
      lt->next = lno_c;
    }
    EESetItemDirCheck(hti_c, lno_c, st+1);
  }
}

HTREEITEM EEAddTreeItem(HTREEITEM parent, char *name, bool haveChild)
{
  TVINSERTSTRUCT tis;

  tis.hParent = parent;
  tis.hInsertAfter = TVI_LAST;
  tis.item.pszText = name;
  tis.item.cchTextMax = (int)strlen(name);
  if (haveChild) {
    tis.item.mask = TVIF_CHILDREN | TVIF_TEXT;
    tis.item.cChildren = 1;
  } else {
    tis.item.mask = TVIF_TEXT;
    tis.item.cChildren = 0;
  }
  return (HTREEITEM)SendDlgItemMessage(hwndD, IDC_TREE, TVM_INSERTITEM, 0, (LPARAM)&tis);
}

int EEAddLocalTreeItem(int root, HTREEITEM hi, char *disp, char *file)
{
  (ltree+numltree)->root = root;
  (ltree+numltree)->hi = hi;
  (ltree+numltree)->disp = (char *)malloc(strlen(disp)+1);
  if ((ltree+numltree)->disp != NULL) {
    strcpy( (ltree+numltree)->disp, disp );
  }
  (ltree+numltree)->file = (char *)malloc(strlen(file)+1);
  if ((ltree+numltree)->file != NULL) {
    strcpy( (ltree+numltree)->file, file );
  }
  (ltree+numltree)->child = -1;
  (ltree+numltree)->next = -1;
  numltree++;

  return numltree-1;
}

bool EESetItemMakeBranchIsExist(HTREEITEM hti, HTREEITEM *hti_c, int lno, int *lno_c, char *ch_n)
{
  LOCALTREE *lt;
  int tmp;
  //HTREEITEM hi;

  if (hti == 0)  return false;
  if ((ltree+lno)->child == -1) {
    return false;
  } else {
    lt = ltree + (ltree+lno)->child;
    if (strcmp(lt->file, ch_n) == 0) {
      *hti_c = lt->hi;
      *lno_c = (ltree+lno)->child;
      return true;
    }
    while (lt->next != -1) {
      //hi = lt->hi;
      tmp = lt->next;
      lt = ltree + tmp;
      if (strcmp(lt->file, ch_n) == 0) {
        *hti_c = lt->hi;
        *lno_c = tmp;
        return true;
      }
    }
  }

  return false;
}

HTREEITEM EESetItemFinalItem(HTREEITEM hti, char *fname)
{
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strncpy(buf2, fname, strlen(fname)-4);
  buf2[strlen(fname)-4] = '\0';
  strcpy(buf, _(buf2));
  strcat(buf, &fname[strlen(fname)-4]);
  return EEAddTreeItem(hti, buf, false);
}

void EESetItemLocalFinalItem(HTREEITEM hti, int lno, char *fname)
{
  LOCALTREE *lt;
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strncpy(buf2, fname, strlen(fname)-4);
  buf2[strlen(fname)-4] = '\0';
  strcpy(buf, _(buf2));
  strcat(buf, &fname[strlen(fname)-4]);
  if ((ltree+lno)->child == -1) {
    (ltree+lno)->child = EEAddLocalTreeItem(lno, hti, buf, fname);
  } else {
    lt = ltree + (ltree+lno)->child;
    while (lt->next != -1) {
      lt = ltree + lt->next;
    }
    lt->next = EEAddLocalTreeItem(lno, hti, buf, fname);
  }
}

void EEExtractSelection(void)
{
  LOCALTREE *lt;
  char path[MAX_PATH];

  lt = EEFindLocalTreeItem(hiSelected);
  if (EEGetFolder(path) == false)  return;
  chdir(path);
  EEExtractMain(lt);
}

LOCALTREE *EEFindLocalTreeItem(HTREEITEM hi)
{
  for (int i=0; i<numltree; i++) {
    if ((ltree+i)->hi == hi)  break;
  }
  return ltree+i;
}

bool EEGetFolder(char *path)
{
  BROWSEINFO bi;
  ITEMIDLIST *idl;

  CoInitialize(NULL);
  bi.lpszTitle = "出力フォルダを選んでください。";
  bi.pidlRoot = NULL;
  bi.hwndOwner = hwndD;
  bi.ulFlags = BIF_RETURNONLYFSDIRS;
  bi.lpfn = NULL;
  bi.lParam = 0;
  bi.iImage = 0;
  bi.pszDisplayName = path;
  idl = SHBrowseForFolder(&bi);
  if (idl == NULL)  return false;
  SHGetPathFromIDList(idl, path);

  return true;
}

void EEExtractMain(LOCALTREE *lt)
{
  if (lt->child != -1) {
    EEExtractChild(lt);
  } else {
    EEExtractFile(lt);
  }
}

void EEExtractChild(LOCALTREE *lt)
{
  LOCALTREE *ltl;
  char curdir[MAX_PATH];

  getcwd(curdir, sizeof(curdir));
  mkdir(lt->disp);
  chdir(lt->disp);
  ltl = ltree + lt->child;
  EEExtractMain(ltl);
  while (ltl->next != -1) {
    ltl = ltree + ltl->next;
    EEExtractMain(ltl);
  }
  chdir(curdir);
}

void EEExtractFile(LOCALTREE *lt)
{
  char fname[FILENAME_MAX];

  EEMakeOrigFilename(fname, lt);
  printf("Now Extract %s\n", fname);
  pak1->Uncomp2tmp(fname);

  FILE *fp;
  fp = fopen(lt->disp, "wb");
  if (fp == NULL)  return;
  fwrite(pak1->tdata, 1, pak1->tlen, fp);
  fclose(fp);
}

void EEMakeOrigFilename(char *fname, LOCALTREE *lt)
{
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strcpy(buf, lt->file);
  strrev(buf);
  lt = ltree + lt->root;
  while (lt->root != -1) {
    strcpy(buf2, lt->file);
    strrev(buf2);
    strcat(buf, "\\");
    strcat(buf, buf2);
    lt = ltree + lt->root;
  }
  strcat(buf, "\\atad");
  strrev(buf);
  strcpy(fname, buf);
}

#ifdef VIOLATION_CODE
void EEMakePatch(int ver)
{
  LOCALTREE *lt;
  char srcfn[FILENAME_MAX];
  char destfn[FILENAME_MAX];
  char origfn[FILENAME_MAX];

  lt = EEFindLocalTreeItem(hiSelected);
  if (lt->child != -1) {
    MessageBox(0, "ツリーから対象ファイルを選んでください。\r(ディレクトリはダメ)", NULL, MB_ICONSTOP | MB_OK);
    return;
  }


  if (EEGetFilename(srcfn, sizeof(srcfn), "パッチにするファイル", true) == false)  return;
  if (EEGetFilename(destfn, sizeof(destfn), "セーブファイル名の設定", false) == false)  return;

  if (ver == 1) {
    pak1->MakePatchOpen(destfn, 0x102);
  } else if (ver == 2) {
    pak1->MakePatchOpen(destfn, 0x200);
  }

  FILE *fp;
  BYTE *dat;
  int len;

  fp = fopen(srcfn, "rb");
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = (BYTE *)malloc(len);
  fread(dat, 1, len, fp);
  fclose(fp);
  EEMakeOrigFilename(origfn, lt);
  pak1->MakePatchAdd(origfn, dat, len);
  pak1->MakePatchClose();
  free(dat);
}
#endif // VIOLATION_CODE

bool EEGetFilename(char *filename, int fn_size, char *title, bool bopen)
{
  OPENFILENAME ofn;
  char dir[FILENAME_MAX];

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwndD;
  ofn.lpstrFile = filename;
  filename[0] = '\0';
  ofn.nMaxFile = fn_size;
  ofn.lpstrTitle = title;
  ofn.lpstrInitialDir = _getcwd(dir, sizeof(dir));

  if (bopen) {
    if (!GetOpenFileName(&ofn)) return false;
  } else {
    if (!GetSaveFileName(&ofn)) return false;
  }

  return true;
}

void EEAnyProc(void)
{
  LOCALTREE *lt;
  char fname[FILENAME_MAX];

  lt = EEFindLocalTreeItem(hiSelected);
  if (lt->child != -1) {
    MessageBox(0, "ツリーから対象ファイルを選んでください。\r(ディレクトリはダメ)", NULL, MB_ICONSTOP | MB_OK);
    return;
  }

  EEMakeOrigFilename(fname, lt);
  char efname[FILENAME_MAX];
  for (int i=0; i<sizeof(suffixEnableProc) / 4; i++) {
    if (STRNCASECMP(&fname[strlen(fname)-3], suffixEnableProc[i], 3) == 0) {
      SNPRINTF(efname, sizeof(efname), "%s.%s", PAKTEMP, suffixEnableProc[i]);
      if (EEExtractTemp(efname, fname) == false)  return;
      ShellExecute(0, NULL, efname, NULL, NULL, SW_SHOWDEFAULT);
    }
  }
}

bool EEExtractTemp(char *tmpfname, char *origfname)
{
  FILE *fp;
  int flen;

  if ((fp = fopen(tmpfname, "wb")) == NULL)  return false;
  pak1->Uncomp2tmp(origfname);
  flen = pak1->tlen;
  fwrite(pak1->tdata, 1, flen, fp);
  fclose(fp);

  return true;
}

INT_PTR CALLBACK DialogProcEasyExt(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_INITDIALOG:
    InitCommonControls();
    hwndD = hwndDlg;
    EEInitDialog();
    return FALSE;
  case WM_CLOSE:
    EEExitDialog(IDCLOSE);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDC_BTNEXTRACT:
      EEExtractSelection();
      break;
#ifdef VIOLATION_CODE
    case IDC_BTNEEMAKE:
      EEMakePatch(1);
      break;
    case IDC_BTNEEMAKE2:
      EEMakePatch(2);
      break;
#endif // VIOLATION_CODE
    case IDCLOSE:
      EEExitDialog(IDCLOSE);
      break;
    case IDC_BTNTOCV:
      EEExitDialog(IDC_BTNTOCV);
      break;
    case IDC_BTNANYPROC:
      EEAnyProc();
      break;
    default:
      return FALSE;
    }
    break;
  case WM_NOTIFY:
    switch (wParam) {
    case IDC_TREE:
      {
        NMTREEVIEW *ltv = (NMTREEVIEW *)lParam;
        switch (ltv->hdr.code) {
        case TVN_SELCHANGED:
          NMTREEVIEW *nmtv;
          nmtv = (NMTREEVIEWA *)lParam;
          hiSelected = nmtv->itemNew.hItem;
        break;
        }
      }
      break;
    }
    break;
  default:
    return FALSE;
  }

  return TRUE;
}
