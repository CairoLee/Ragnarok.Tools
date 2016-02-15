#include "stdafx.h"
#include "Form1.h"
#include <windows.h>
#include <stdio.h>
#include <iconv.h>
#include <libintl.h>

#include "../common/relocate/progname.h"
#include "../common/relocate/relocatable.h"

using namespace rsmview;

extern "C" {
  extern
#ifdef WIN32
  __declspec(dllimport)
#endif
  const char *locale_charset(void);
}
char NativeLANG[20];
MOUSE_ASSIGN *mouseAssign;

int argc;
char **argv;
extern int __argc;
extern char **__argv;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	Application::Run(new FrmMain());
	return 0;
}

void FrmMain::MainClose(void)
{
  if (pakread) {
    for (int i=0; i<numltree; i++) {
      delete (ltree+i)->disp;
      delete (ltree+i)->file;
    }
    free(ltree);
  }
  delete mouseAssign;
}

void FrmMain::InitLocalValues(void)
{
  argc = __argc;
  argv = __argv;

  firstpaint = true;
  shiftState = false;
  hglrc = 0;
  hdc = 0;
  mouseAssign = new MOUSE_ASSIGN;
  mouseAssign->pan = MK_RBUTTON;
  mouseAssign->zoom = MK_MBUTTON;
  mouseAssign->rot = MK_LBUTTON;
  ltree = NULL;
  numltree = 0;

  pak1 = NULL;
  rsmview = NULL;
  pakread = __argc > 1 ? true : false;
  SetTextDomain(__argv[0]);
  if (pakread) {
    OpenPak(__argv[1]);
  }
}

void FrmMain::Draw(void)
{
  wglMakeCurrent(hdc, hglrc);

  if (rsmview)  rsmview->Draw();

  SwapBuffers(hdc);
  wglMakeCurrent(hdc, 0);
}

void FrmMain::OGLResize(void)
{
  if (rsmview == NULL)  return;

  rsmview->oglwidth = this->pictureBox1->Width;
  rsmview->oglheight = this->pictureBox1->Height;
  wglMakeCurrent(hdc, hglrc);
  rsmview->InitOpenGL();
  rsmview->SetCameraDefault(false);
  rsmview->Draw();
  SwapBuffers(hdc);
  wglMakeCurrent(hdc, 0);
}

void FrmMain::SetupOpenGLWin(void)
{
  HWND hwnd;

  hwnd = (HWND)this->pictureBox1->get_Handle().ToInt32();

  PIXELFORMATDESCRIPTOR pfd;
  int npf;

  if (hglrc != 0) {
    wglDeleteContext(hglrc);
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

  hdc = GetDC(hwnd);
  npf = ChoosePixelFormat(hdc, &pfd);
  SetPixelFormat(hdc, npf, &pfd);
  hglrc = wglCreateContext(hdc);
  if (hglrc == 0) {
    return;
  }
  wglMakeCurrent(hdc, hglrc);

  if (rsmview) {
    rsmview->oglwidth = this->pictureBox1->Width;
    rsmview->oglheight = this->pictureBox1->Height;
    rsmview->InitOpenGL();
    rsmview->SetCameraDefault(false);
    rsmview->Draw();
  }

  SwapBuffers(hdc);
  wglMakeCurrent(hdc, 0);
}

bool FrmMain::OpenPak(char *fname)
{
  char fnbuf[FILENAME_MAX];

  GetFullPathName(fname, sizeof(fnbuf), fnbuf, NULL);
  pak1 = new CPAK();
  pak1->AddPakFile(fnbuf);
  rsmview = new CRsmview(pak1);
  for (int i=2; i < argc; i++) {
    GetFullPathName(argv[i], sizeof(fnbuf), fnbuf, NULL);
    pak1->AddPakFile(fnbuf);
  }

  this->menuSaveAsX->Enabled = true;

  return true;
}

void FrmMain::BuildTree(void)
{
  SetRootLocalTree();
  ltree->handle = SetRootTree();

  for (int i=0, j=1; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    if (STRNCASECMP(&pak1->Entries[i].fn[strlen(pak1->Entries[i].fn)-4], ".rsm", 4) != 0)  continue;
    SetItemDirCheck(ltree->handle, -1, pak1->Entries[i].fn);
  }
}

int FrmMain::SetRootTree(void)
{
  TreeNode *tn;
  tn = new TreeNode(S"data");
  this->treeView1->Nodes->Add(tn);

  return tn->get_Handle().ToInt32();
}

void FrmMain::SetRootLocalTree(void)
{
  ltree = (LOCALTREE *)malloc(EVERY_LOCALTREE * sizeof(LOCALTREE));
  ltree->root = -1;
  ltree->disp = (char *)malloc(5); // data\0
  strcpy(ltree->disp, "data");
  ltree->file = (char *)malloc(5); // data\0
  strcpy(ltree->file, "data");
  ltree->next = -1;
  ltree->child = -1;
  numltree = 1;
}

void FrmMain::SetItemDirCheck(int handle, int lno, char *fname)
{
  char *st;
  char buf[FILENAME_MAX];
  int m_handle;

  int ver = (int)pak1->pakversion[0];
  if (ver <= 0) {  // pak or b1
    st = strchr(fname, '/');
  } else {
    st = strchr(fname, '\\');
  }
  if (lno == -1) {
    SetItemDirCheck(ltree->handle, 0, st+1);
    return;
  }
  if (st) { // include subdir
    strncpy(buf, fname, st-fname);
    buf[st-fname] = '\0';
    SetItemMakeBranch(handle, lno, buf, fname);
  } else {
    m_handle = SetItemFinalItem(handle, fname);
    SetItemLocalFinalItem(m_handle, lno, fname);
  }
}

void FrmMain::SetItemMakeBranch(int handle, int lno, char *node, char *fname)
{
  char *st;
  char buf[FILENAME_MAX];
  int c_handle;
  int lno_c;
  LOCALTREE *lt;

  int ver = (int)(pak1->pakversion[0]);
  if (ver <= 0) { // pak or b1
    st = strchr(fname, '/');
  } else {
    st = strchr(fname, '\\');
  }
  strcpy(buf, _(node));
  if (SetItemMakeBranchIsExist(handle, &c_handle, lno, &lno_c, node)) {  // already exist
    SetItemDirCheck(c_handle, lno_c, st+1);
  } else {
    c_handle = AddTreeItem(handle, buf);
    lno_c = AddLocalTreeItem(lno, c_handle, buf, node);
    if ((ltree+lno)->child == -1) {
      (ltree+lno)->child = lno_c;
    } else {
      lt = ltree + (ltree+lno)->child;
      while (lt->next != -1) {
        lt = ltree + lt->next;
      }
      lt->next = lno_c;
    }
    SetItemDirCheck(c_handle, lno_c, st+1);
  }
}

int FrmMain::AddTreeItem(int parent, char *name)
{
  TreeNode *tn = TreeNode::FromHandle(this->treeView1, parent);
  TreeNode *c = new TreeNode(name);
  tn->Nodes->Add(c);

  return tn->LastNode->get_Handle().ToInt32();
}

int FrmMain::AddLocalTreeItem(int root, int handle, char *disp, char *file)
{
  if ((numltree % EVERY_LOCALTREE) == 0) {
    ltree = (LOCALTREE *)realloc(ltree, (numltree+EVERY_LOCALTREE)*sizeof(LOCALTREE));
  }
  (ltree+numltree)->root = root;
  (ltree+numltree)->handle = handle;
  (ltree+numltree)->disp = new char[strlen(disp)+1];
  if ((ltree+numltree)->disp != NULL) {
    strcpy( (ltree+numltree)->disp, disp );
  }
  (ltree+numltree)->file = new char[strlen(file)+1];
  if ((ltree+numltree)->file != NULL) {
    strcpy( (ltree+numltree)->file, file );
  }
  (ltree+numltree)->child = -1;
  (ltree+numltree)->next = -1;
  numltree++;

  return numltree-1;
}

bool FrmMain::SetItemMakeBranchIsExist(int handle, int *c_handle, int lno, int *lno_c, char *ch_n)
{
  LOCALTREE *lt;
  int tmp;

  if (handle == 0)  return false;
  if ((ltree+lno)->child == -1) {
    return false;
  } else {
    lt = ltree + (ltree+lno)->child;
    if (strcmp(lt->file, ch_n) == 0) {
      *c_handle = lt->handle;
      *lno_c = (ltree+lno)->child;
      return true;
    }
    while (lt->next != -1) {
      tmp = lt->next;
      lt = ltree + tmp;
      if (strcmp(lt->file, ch_n) == 0) {
        *c_handle = lt->handle;
        *lno_c = tmp;
        return true;
      }
    }
  }

  return false;
}

int FrmMain::SetItemFinalItem(int handle, char *fname)
{
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strncpy(buf2, fname, strlen(fname)-4);
  buf2[strlen(fname)-4] = '\0';
  strcpy(buf, _(buf2));
  //strcat(buf, &fname[strlen(fname)-4]);

  return AddTreeItem(handle, buf);
}

void FrmMain::SetItemLocalFinalItem(int handle, int lno, char *fname)
{
  LOCALTREE *lt;
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strncpy(buf2, fname, strlen(fname)-4);
  buf2[strlen(fname)-4] = '\0';
  strcpy(buf, _(buf2));
  strcat(buf, &fname[strlen(fname)-4]);
  if ((ltree+lno)->child == -1) {
    (ltree+lno)->child = AddLocalTreeItem(lno, handle, buf, fname);
  } else {
    lt = ltree + (ltree+lno)->child;
    while (lt->next != -1) {
      lt = ltree + lt->next;
    }
    lt->next = AddLocalTreeItem(lno, handle, buf, fname);
  }
}

void FrmMain::TreeViewSelect(int handle)
{
  LOCALTREE *rt = NULL;

  if (TreeViewSelectCheck(handle, &rt) == false) return;

  char fname[FILENAME_MAX];
  MakeOrigFilename(fname, rt);
  if (fname[0] != '\0') {
    wglMakeCurrent(hdc, hglrc);
    rsmview->OpenRSM(fname);
    rsmview->Draw();
    SwapBuffers(hdc);
    wglMakeCurrent(hdc, 0);
  }
}

bool FrmMain::TreeViewSelectCheck(int handle, LOCALTREE **rt)
{
  LOCALTREE st;
  st.handle = handle;

  *rt = NULL;
  for (int i=0; i<numltree; i++) {
    if ((ltree+i)->handle == handle) {
      *rt = ltree+i;
      break;
    }
  }
  if (*rt == NULL)  return false;
  if ((*rt)->child != -1)  return false;

  return true;
}

void FrmMain::MakeOrigFilename(char *fname, LOCALTREE *lt)
{
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];

  strcpy(buf, lt->file);
  strrev(buf);
  lt = ltree + lt->root;
  while (lt->root != -1) {
    strcpy(buf2, lt->file);
    strrev(buf2);
    if ((pak1->pakversion[0] & 0xFF00) == 0xFF00) {
      strcat(buf, "/");
    } else {
      strcat(buf, "\\");
    }
    strcat(buf, buf2);
    lt = ltree + lt->root;
  }
  if ((pak1->pakversion[0] & 0xFF00) == 0xFF00) {
    strcat(buf, "/atad");
  } else {
    strcat(buf, "\\atad");
  }
  strrev(buf);
  strcpy(fname, buf);
}

void FrmMain::SetTextDomain(char *progname)
{
  setlocale(LC_ALL, getenv("LANG"));
#ifndef DISTRIBUTE
  strcpy(NativeLANG, locale_charset());
#endif

#if WIN32
  set_program_name(progname);
#endif

  char *ddir;

  ddir = getenv("TEXTDOMAINDIR");
  if ((ddir = getenv("TEXTDOMAINDIR")) != NULL) {
  } else {
#if WIN32
    bindtextdomain(MYTEXTDOMAIN, relocate(".\\share\\locale\\"));
#else
    bindtextdomain(MYTEXTDOMAIN, "/usr/local/share/locale");
#endif
  }
  textdomain(MYTEXTDOMAIN);

#ifdef WIN32
#ifdef USE_GTK
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
#endif

#ifndef WIN32
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
}
