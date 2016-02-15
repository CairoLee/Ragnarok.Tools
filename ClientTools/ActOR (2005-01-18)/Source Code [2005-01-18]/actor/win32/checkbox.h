#ifndef __MY_CHECKBOX__
#define __MY_CHECKBOX__

#include <windows.h>

#include "../winmain.h"

extern HWND hwndHead;
extern HWND hwndBody;
extern HWND hwndEtc;
extern HWND hwndNeigh;

void BuildCheckboxes(HINSTANCE hInst);
void RepositionCheckBoxes(RECT *rc);
void OwnerDrawCheckBox(DRAWITEMSTRUCT *lpd);
void OnBtnHead(void);
void OnBtnBody(void);
void OnBtnEtc(void);
void OnBtnNeigh(void);
void CheckRefHead(void);
void CheckRefBody(void);
void CheckRefEtc(void);
void CheckRefNeigh(void);

#endif // __MY_CHECKBOX__
