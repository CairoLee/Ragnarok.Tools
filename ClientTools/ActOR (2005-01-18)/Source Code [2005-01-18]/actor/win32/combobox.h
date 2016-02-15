#ifndef __MY_COMBOBOX__
#define __MY_COMBOBOX__

#include <windows.h>
#include "../winmain.h"
#include "../../common/global.h"

extern HWND hwndAct;
extern HWND hwndSnd;

void BuildComboBox(HINSTANCE hInst);
void RepositionComboBox(RECT *rc);
void SetStringComboSnd(void);
void OwnerDrawComboBox(DRAWITEMSTRUCT *lpd);
void OnComboActChange(void);
void OnComboSndChange(void);
void OnCommandCombo(WPARAM wParam);
void OnMeasureCombo(MEASUREITEMSTRUCT *mi);

#endif  // __MY_COMBOBOX__
