#include "frames.h"

//frame all image
HDC hdcAll;
HGLRC hglrcAll;
HWND hwndAll;

//image list
HDC hdcFrmImg;
HGLRC hglrcFrmImg;
HWND hwndFrmImg;

void BuildFrames(HINSTANCE hInst)
{
  //frame all image
  hwndAll = CreateWindow("STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_FRAMEALL, hInst, NULL);
  SetupOpenGL(hwndAll, &hdcAll, &hglrcAll);
  //image list
  actor->curImg = 0;
  hwndFrmImg = CreateWindow("STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0, 0, 1, 1, hwndMain, (HMENU)IDC_FRAMEIMG, hInst, NULL);
  SetupOpenGL(hwndFrmImg, &hdcFrmImg, &hglrcFrmImg);
}

void RepositionFrames(RECT *rc)
{
  //frame all image
  int frmImgHeight = rc->bottom * (100-vSplitPercent) / 100 - 2 - 6;
  MoveWindow(hwndAll, 3, 21, rc->right-366, rc->bottom * vSplitPercent / 100 - 3 - 5 - 18, TRUE);
  CameraDefault(hwndAll, hdcAll, hglrcAll, &actor->cameraAll, &actor->widthAll, &actor->heightAll, &actor->ratioAll);
  DrawFrameAll();

  //image list
  CameraDefault(hwndFrmImg, hdcFrmImg, hglrcFrmImg, &actor->cameraFrmImg, &actor->widthImage, &actor->heightImage, &actor->ratioFrmImg);
  if (IsSprReady()) {
    SetScrollPos_(hwndFrmImgScr, actor->curImg, actor->GetNumImage()-1);
    MoveWindow(hwndFrmImg, 3, rc->bottom-frmImgHeight, rc->right-6, frmImgHeight - 16, TRUE);
    SetImages(GetScrollImgPos());                            
  } else {
    SetScrollPos_(hwndFrmImgScr, actor->curImg, 0);
  }
}

void OwnerDrawFrames(DRAWITEMSTRUCT *lpd)
{
  if (actor->drawStop)  return;

  switch (lpd->CtlID) {
  case IDC_FRAMEIMG:
    SetImages(actor->curImg);
    break;
  case IDC_FRAMEALL:
    DrawFrameAll();
    break;
  }
}

void SetupOpenGL(HWND hwnd, HDC *hdc, HGLRC *hglrc)
{
  PIXELFORMATDESCRIPTOR pfd;
  int npf;

  if (*hglrc != 0) {
    wglDeleteContext(*hglrc);
  }
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_GENERIC_ACCELERATED
              | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 8;
  pfd.iLayerType = PFD_MAIN_PLANE;

  *hdc = GetDC(hwnd);
  npf = ChoosePixelFormat(*hdc, &pfd);
  SetPixelFormat(*hdc, npf, &pfd);
  *hglrc = wglCreateContext(*hdc);
  if (*hglrc == 0) {
    return;
  }
  wglMakeCurrent(*hdc, *hglrc);
  RECT rc;
  GetClientRect(hwnd, &rc);

  actor->SetupOpenGL(rc.right, rc.bottom);

  SwapBuffers(*hdc);
  wglMakeCurrent(*hdc, 0);
}

void CameraDefault(HWND hwnd, HDC hdc, HGLRC hglrc, CAM_STATE *camera, int *width, int *height, GLfloat *ratio)
{
  RECT rc;
  GetClientRect(hwnd, &rc);
  camera->eye_v.x = 0;
  camera->eye_v.y = 0;
  camera->eye_v.z = -1;
  camera->c_p.x = (float)(rc.right / 2.);
  camera->c_p.y = (float)(rc.bottom / 2.);
  camera->c_p.z = (float)(-rc.right / 4.);
  camera->v_v.x = 0;
  camera->v_v.y = 1;
  camera->v_v.z = 0;
  camera->u_v.x = 1;
  camera->u_v.y = 0;
  camera->u_v.z = 0;
  camera->dist = rc.right / 4.f;
  *width = rc.right;
  *height = rc.bottom;
  *ratio = rc.right / (GLfloat)rc.bottom;
  wglMakeCurrent(hdc, hglrc);
  glViewport(0, 0, rc.right, rc.bottom);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, rc.right, 0, rc.bottom, -10, 10);
  glFlush();
  SwapBuffers(hdc);
  wglMakeCurrent(hdc, 0);
}

void DrawFrameAll(void)
{
  wglMakeCurrent(hdcAll, hglrcAll);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (IsActReady() == false) {
    SwapBuffers(hdcAll);
    wglMakeCurrent(hdcAll, 0);
    return;
  }

  int numS = (int)SendMessage(hwndTable, LVM_GETSELECTEDCOUNT, 0, 0);
  int *iItems = NULL;
  if (numS) {
    iItems = (int *)malloc(numS * sizeof(int));
    GetSelectedTableItem(iItems, numS);
  }

  actor->DrawFrameAll(iItems, numS);

  if (numS)  free(iItems);

  SwapBuffers(hdcAll);
  wglMakeCurrent(hdcAll, 0);
}

void SetImages(int no)
{
  wglMakeCurrent(hdcFrmImg, hglrcFrmImg);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (IsSprReady() == false) {
    SwapBuffers(hdcFrmImg);
    wglMakeCurrent(hdcFrmImg, 0);
    return;
  }

  actor->SetImages(no);

  SwapBuffers(hdcFrmImg);
  wglMakeCurrent(hdcFrmImg, 0);
}

int GetClickImgNo(LPARAM lParam)
{
  POINT mp;
  mp.x = LOWORD(lParam);
  mp.y = HIWORD(lParam);

  ClientToScreen(hwndMain, &mp);
  ScreenToClient(hwndFrmImg, &mp);

  return actor->GetClickImgNo(mp.x);  
}
