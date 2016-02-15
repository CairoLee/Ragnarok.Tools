#ifndef __MY_EDIT__
#define __MY_EDIT__

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <imm.h>

#include "../winmain.h"
#include "../actor.h"

extern HWND hwndEditInter;
extern WNDPROC wndprocOrigEditInter;

void BuildEdits(HINSTANCE hInst);
void RepositionEdits(RECT *rc);
void SetEditInterval(int inter);
void SetIntervalValue(void);

LRESULT CALLBACK WndProcEditInter(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//void OnNotifyTable(WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK WndProcTable(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//VOID CALLBACK TimerTableEdit(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#endif  // __MY_EDIT__
