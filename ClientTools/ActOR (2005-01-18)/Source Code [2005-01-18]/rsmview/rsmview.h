#ifndef __MY_RSMVIEW__
#define __MY_RSMVIEW__

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "../common/rsm.h"
#include "../common/pak.h"
#include "../common/vec.h"

#ifndef WIN32
typedef long LONG;
                                                                                
typedef struct {
  LONG x, y;
} POINT;
#endif

typedef enum {
  NOSTATE,
  OBJ_ROT,
  SCRN_ROT,
  ZOOM,
  PAN,
  ZOOM_SET
} MouseOperationState;

typedef struct {
  Vec n[8];
} BOX;

class CRsmview {
private:
  CPAK *pak;
  void SetBoundBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
  void SetVertex(Vec *v, float x, float y, float z);

public:
  CRsmview(CPAK *pak1);
  virtual ~CRsmview();

  CRSM *rsm;
  int oglwidth, oglheight;
  POINT first_mp, old_mp, new_mp;
  CAM_STATE camera;
  BOX bound_box;
  MouseOperationState opState;
  int dispmode;
  MYOGL_RGB bgcol;

  void OpenRSM(char *fname);
  void RSMFree(void);
  void Draw(void);
  void DrawGuide(void);
  void InitOpenGL(void);
  void SetCameraDefault(bool distchange);
  void CameraSet(void);
  void GLOBJRot(void);
  void GLSCRNRot(void);
  void GLZoom(void);
  void GLPan(void);
  void SearchRotOrigin(void);
  void CalcCenter(void);
};

#endif //  __MY_RSMVIEW__
