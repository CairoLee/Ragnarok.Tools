#ifndef __MY_EASYEXT_WIN32__
#define __MY_EASYEXT_WIN32__

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <stdio.h>
#include <libintl.h>

#include "../main.h"

typedef struct {
  int root;
  HTREEITEM hi;
  char *disp;
  char *file;
  int child;
  int next;
} LOCALTREE;

extern int EasyExt(int argc, char **argv);

void EEInitDialog(void);
void EEExitDialog(int ID);
void EEDestroyLocalTree(LOCALTREE *lt);
void EESetTreeItemAll(void);
HTREEITEM EESetRootTree(void);
void EESetRootLocalTree(int num);
void EESetItemDirCheck(HTREEITEM hti, int lno, char *fname);
void EESetItemMakeBranch(HTREEITEM hti, int lno, char *node, char *fname);
HTREEITEM EEAddTreeItem(HTREEITEM parent, char *name, bool haveChild);
int EEAddLocalTreeItem(int root, HTREEITEM hi, char *disp, char *file);
bool EESetItemMakeBranchIsExist(HTREEITEM hti, HTREEITEM *hti_c, int lno, int *lno_c, char *ch_n);
HTREEITEM EESetItemFinalItem(HTREEITEM hti, char *fname);
void EESetItemLocalFinalItem(HTREEITEM hti, int lno, char *fname);

void EEAnyProc(void);
bool EEExtractTemp(char *tmpfname, char *origfname);

void EEExtractSelection(void);
LOCALTREE *EEFindLocalTreeItem(HTREEITEM hi);
bool EEGetFolder(char *path);
void EEExtractMain(LOCALTREE *lt);
void EEExtractChild(LOCALTREE *lt);
void EEExtractFile(LOCALTREE *lt);
void EEMakeOrigFilename(char *fname, LOCALTREE *lt);

#ifdef VIOLATION_CODE
void EEMakePatch(int ver);
#endif // VIOLATION_CODE
bool EEGetFilename(char *filename, int fn_size, char *title, bool bopen);

INT_PTR CALLBACK DialogProcEasyExt(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcEETreeView(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __MY_EASYEXT_WIN32__
