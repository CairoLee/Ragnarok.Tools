#ifndef __MY_GLOBAL_H__
#define __MY_GLOBAL_H__

#pragma pack(1)
#pragma warning(disable:4103) // pack warning
#pragma warning(disable:4311) // small size pointer cast
#pragma warning(disable:4312) // large size cast

#define DISTRIBUTE
//#define USE_LINE

#if 0  // 0:can make patch, 1:can NOT make patch
#undef VIOLATION_CODE
#else
#define VIOLATION_CODE
#endif

#ifndef WIN32
#define USE_GTK
#endif

typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef struct {
  int pan;
  int zoom;
  int rot;
} MOUSE_ASSIGN;

#include "vec.h"

typedef struct {
  Vec eye_v;  /* view vector. always normalized */
  Vec u_v;    /* right vector on 2D screen */
  Vec v_v;    /* up vector on 2D screen */
  Vec c_p;    /* center position */
  float dist;      /* distance */
  Vec first_p;
  Vec rot_center;
  Vec rot_center2;
} CAM_STATE;

#ifdef WIN32
#define INLINE __inline
#define CHDIR _chdir
#define GETCWD _getcwd
#define MKDIR(a) _mkdir(a)
#define SNPRINTF _snprintf
#define WFOPEN(filename, mode) _wfopen((wchar_t *)filename, _TEXT(mode))
#define WMKDIR(dirname) _wmkdir((wchar_t *)dirname)
#define WCHDIR(dirname) _wchdir((wchar_t *)dirname)
#define WGETCWD(buffer, maxlen) _wgetcwd((wchar_t *)buffer, maxlen)
#define STRCASECMP(s1, s2) stricmp(s1, s2)
#define STRNCASECMP(s1, s2, n) strnicmp(s1, s2, n)
#define EXECLP _execlp
#include <process.h>
#endif

#ifdef linux
#define INLINE inline
#define CHDIR chdir
#define GETCWD getcwd
#define MKDIR(a) mkdir(a, 00755)
#define SNPRINTF snprintf
#define _TEXT(a) a
#define WFOPEN(filename, mode) fopen(filename, mode)
#define WMKDIR(dirname) MKDIR(dirname)
#define WCHDIR(dirname) CHDIR(dirname)
#define WGETCWD(buffer, maxlen) GETCWD(buffer, maxlen)
#define STRCASECMP(s1, s2) strcasecmp(s1, s2)
#define STRNCASECMP(s1, s2, n) strncasecmp(s1, s2, n)
#define EXECLP execlp
#endif

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265359f
#endif

#define MYTEXTDOMAIN "pakviewer.l"
#define _(String) gettext(String)

#define PO_STR_SET(to, from) NativetoUTF8(to, sizeof(to), from, strlen(from));

#define GETCOL_R(data) ((BYTE)(data))
#define GETCOL_G(data) ((BYTE)(data >> 8))
#define GETCOL_B(data) ((BYTE)(data >> 16))

#ifdef WIN32
#define LIBGLADE_SIGNAL_FUNC extern "C" __declspec(dllexport) 
#else
#define LIBGLADE_SIGNAL_FUNC extern "C"
#endif

#endif // __MY_GLOBAL_H__

