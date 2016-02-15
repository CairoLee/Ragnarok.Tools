#ifndef __MY_MP98_WIN32__
#define __MY_MP98_WIN32__

#include <windows.h>
#include <stdio.h>

#include "../../common/global.h"

typedef struct {
  int i;
  char sjis[FILENAME_MAX];
} LFILES;

typedef struct {
  char euckr[FILENAME_MAX];
  char sjis[FILENAME_MAX];
} VSFILES;

#ifdef VIOLATION_CODE
extern void MakePatch98(void);

void MP98OpenVSFile(HWND hwndDlg, int ID_DLGI);
void MP98MakeMain(HWND hwndDlg, bool flag);
void MP98ParseAndExec(HWND hwndDlg, FILE *fpl, FILE *fpv, bool flag);
void MP98ReadAdd(FILE *fpl, FILE *fpv, VSFILES **vf, int *numvs);
void MP98GetList(FILE *fp, LFILES **lf, int *numlf);
int MP98SortLFILES(const void *a, const void *b);
void MP98MakePatchReal(HWND hwndDlg, VSFILES *vf, int numvs);
INT_PTR CALLBACK DialogProcMP98(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif // VIOLATION_CODE

#endif // __MY_MP98_WIN32__
