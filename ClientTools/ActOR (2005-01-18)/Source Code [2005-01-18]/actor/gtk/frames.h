#ifndef __MY_FRAMES__
#define __MY_FRAMES__

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "../main.h"
#include "../../common/global.h"
#include "table.h"

void SetupOpenGL(GtkWidget *widget);
void CameraDefault(GtkWidget *widget, CAM_STATE *camera, int *width, int *height, GLfloat *ratio);
void DrawFrameAll(void);
void SetImages(int no);
int GetClickImgNo(int x);

#endif  // __MY_FRAMES__
