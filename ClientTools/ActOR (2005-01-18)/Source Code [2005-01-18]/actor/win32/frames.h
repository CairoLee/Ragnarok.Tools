#ifndef __MY_FRAMES__
#define __MY_FRAMES__

#include <windows.h>
#include "../winmain.h"
#include "../../common/global.h"
#include "vsplit.h"

extern HDC hdcAll;
extern HGLRC hglrcAll;
extern HWND hwndAll;
extern HDC hdcFrmImg;
extern HGLRC hglrcFrmImg;
extern HWND hwndFrmImg;

void BuildFrames(HINSTANCE hInst);
void RepositionFrames(RECT *rc);
void OwnerDrawFrames(DRAWITEMSTRUCT *lpd);
void SetupOpenGL(HWND hwnd, HDC *hdc, HGLRC *hglrc);
void CameraDefault(HWND hwnd, HDC hdc, HGLRC hglrc, CAM_STATE *camera, int *width, int *height, GLfloat *ratio);
void DrawFrameAll(void);
void SetImages(int no);
int GetClickImgNo(LPARAM lParam);

#endif  // __MY_FRAMES__