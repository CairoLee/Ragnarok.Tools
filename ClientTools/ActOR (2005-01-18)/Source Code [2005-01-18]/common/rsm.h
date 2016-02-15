#ifndef __GRFVIEWER_RSM__
#define __GRFVIEWER_RSM__

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "global.h"
#include "vec.h"
#include "pak.h"

typedef struct {
  GLfloat r;
  GLfloat g;
  GLfloat b;
} MYOGL_RGB;

typedef struct {
  WORD sv[3];
  WORD tv[3];
  WORD texid;
  WORD xxx[5];
} SURFACE;

typedef struct {
  float dist;
  int id;
} SURFZSORT;

typedef struct {
  int width;
  int height;
  int texwidth;
  int texheight;
} SprSize;

class CRSM {
private:
  char objname[40];
  int numoftex;
  DWORD **textures;
  GLuint *gltextures;
  SprSize *texsizes;
  int numoftexv;
  Vec *texv;
  int nowtexno;
  SURFZSORT *zsortstore;

  void ObjectFree(void);
  void LoadTexture(BYTE *dat);
  void LoadVertSurf(BYTE *dat);
  void Set1Texture(char *filename, int no);

public:
  CRSM(void);
  virtual ~CRSM(void);

  CPAK *pak;
  int numofvert;
  Vec *vert;
  int numofsurf;
  SURFACE *surf;

  CAM_STATE *camera;
  MYOGL_RGB linergb;

  void DecodeAndSet(BYTE *dat);
  void OGLExecFill(bool sortflag);
  void OGLExecLine(void);

  void SaveModelAsX(char *fname);
};

extern "C" {
int zsortfunc(const void *arg1, const void *arg2);
int Get2NSize(int size);
}
#endif //__GRFVIEWER_RSM__
