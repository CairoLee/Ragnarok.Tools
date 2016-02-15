#ifndef __MY_MAIN__
#define __MY_MAIN__

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <mbstring.h>
#include <direct.h>
#include <io.h>
#endif

#ifdef linux
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include "../common/global.h"
#include "../common/pak.h"
#include "../common/codeconv.h"

#ifdef WIN32
#ifndef USE_GTK
#include "win32/chrview_win32.h"
#include "win32/mp98_win32.h"
#include "win32/easyext_win32.h"
#else
#include "gtk/chrview_gtk.h"
#include "gtk/mp98_gtk.h"
#include "gtk/easyext_gtk.h"
#endif
#else
#include "gtk/chrview_gtk.h"
#include "gtk/mp98_gtk.h"
#include "gtk/easyext_gtk.h"
#endif

#define THIS_APP_NAME "pakext"

typedef struct {
  int subtract;
  char subFilename[FILENAME_MAX];
  bool easymode;
  bool chrviewer;
  bool patch98;
  bool FN_ID;
  bool forceWrite;
  char outPath[FILENAME_MAX];
  bool useData;
  bool useMatch;
  char strMatch[FILENAME_MAX];
  bool listOnly;
  bool useUnicode;
  int makePatch;
  bool appendPatch;
  char fnPatch[FILENAME_MAX];
} MY_FLAGS;

void SetTextDomain(char *progname);
void ParseOptionAddFile(int argc, char **argv, MY_FLAGS *flag);
void InitFlags(MY_FLAGS *flag);
void PrintUsageExit(char *pname);
void ExtractFiles(MY_FLAGS *flag);
void CheckOutputDirectoryFailExit(MY_FLAGS *flag);
void CheckDir(char *filename, int flen, char *origname, MY_FLAGS *flag);
void CreateSubDir(char *dirname, char *filename, int flen, char *origname, MY_FLAGS *flag);
void DecodeWriteFile(char *filename, int flen, char *origname, MY_FLAGS *flag);
bool INLINE CheckFileExist(char *origname, char *filename, MY_FLAGS *flag);
void WriteFileList(MY_FLAGS *flag);
void WriteFileListMsgFmt(MY_FLAGS *flag);
void WriteHangleHTML(MY_FLAGS *flag);
void WritePosLen(char *filename);
bool IsStrMatch(int id, MY_FLAGS *flag);

#ifdef VIOLATION_CODE
void MakePatch(MY_FLAGS *flag);
void SubtractEntry(void);
#ifdef WIN32
void AddPatchDir(char *dir);
void AddPatchReadFile(char *filename);
void WAddPatchDir(wchar_t *dir);
void WAddPatchReadFile(char *filename);
#else  // WIN32
void AddPatchDir(char *dir, char *dosname);  // (-_-)
void AddPatchReadFile(char *filename, char *dosname);
#endif // WIN32
#endif // VIOLATION_CODE

bool GetAgreement(void);
void GetPrefFileName(char *fname, int fname_size);
void ReadPreference(void);
void WritePreference(void);

#endif // __MY_MAIN__
