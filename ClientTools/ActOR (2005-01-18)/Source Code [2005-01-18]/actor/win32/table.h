#ifndef __MY_TABLE__
#define __MY_TABLE__

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <imm.h>

#include "../winmain.h"
#include "../actor.h"
#include "vsplit.h"

#define NUM_1SPR_PROP 9

extern HWND hwndTable;
extern HWND hwndTableEdit;
extern WNDPROC wndprocOrigTable;
extern WNDPROC wndprocOrigTableEdit;

void BuildTable(HINSTANCE hInst);
void RepositionTable(RECT *rc);
DWORD HextToDWORD(char *hex);
BYTE SaturateByte5(BYTE orig, bool plus);
void GetSelectedTableItem(int *sel, int max);
void SwapTableItemText(HWND hwnd, int arow, int acol, int brow, int bcol);
void SetTableFromEdit(char *str);
void SetTableItemAll(int noAct, int noPat);
void SetTableItem(char *txt, int iItem, int iSubItem);
void DownSelectedTableItem(HWND hwnd, int num);
void UpSelectedTableItem(HWND hwnd, int num);
void ChangeSaturationABGRValue(int type, bool plus);
void ChangeMirror(void);
void OnKeyUpInTable(WPARAM wParam, LPARAM lParam);
void OnSysKeyUpInTable(WPARAM wParam, LPARAM lParam);
void DeleteSelectedSpriteInTable(void);
void UnSelectSelection(void);

void OnNotifyTable(WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcTable(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcTableEdit(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK TimerTableEdit(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#endif  // __MY_TABLE__
