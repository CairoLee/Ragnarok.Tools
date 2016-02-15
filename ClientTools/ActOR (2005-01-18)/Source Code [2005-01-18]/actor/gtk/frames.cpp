#include <stdlib.h>

#include "frames.h"

void SetupOpenGL(GtkWidget *widget)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return;
  }

  actor->SetupOpenGL(widget->allocation.width, widget->allocation.height);

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers(gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
}

void CameraDefault(GtkWidget *widget, CAM_STATE *camera, int *width, int *height, GLfloat *ratio)
{
  camera->eye_v.x = 0;
  camera->eye_v.y = 0;
  camera->eye_v.z = -1;
  camera->c_p.x = (float)(widget->allocation.width / 2.);
  camera->c_p.y = (float)(widget->allocation.height / 2.);
  camera->c_p.z = (float)(-widget->allocation.width / 4.);
  camera->v_v.x = 0;
  camera->v_v.y = 1;
  camera->v_v.z = 0;
  camera->u_v.x = 1;
  camera->u_v.y = 0;
  camera->u_v.z = 0;
  camera->dist = widget->allocation.width / 4.f;
  *width = widget->allocation.width;
  *height = widget->allocation.height;
  *ratio = *width / (GLfloat)*height;

  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return;
  }

  glViewport(0, 0, *width, *height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, *width, 0, *height, -10, 10);
  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers(gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
}

void DrawFrameAll(void)
{
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  GdkGLContext *glcontext = gtk_widget_get_gl_context(oglAllImg);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(oglAllImg);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (IsActReady() == false) {
    if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
      gdk_gl_drawable_swap_buffers(gldrawable);
    } else {
      glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    return;
  }

  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  int numS = gtk_tree_selection_count_selected_rows(selection);
  int *iItems = NULL;
  if (numS) {
    iItems = (int *)malloc(numS * sizeof(int));
    GetSelectedTableItem(iItems);
  }

  actor->DrawFrameAll(iItems, numS);

  if (numS)  free(iItems);

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers(gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
}

void SetImages(int no)
{
  GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
  GdkGLContext *glcontext = gtk_widget_get_gl_context(oglFrmImg);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(oglFrmImg);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (IsSprReady() == false) {
    if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
      gdk_gl_drawable_swap_buffers(gldrawable);
    } else {
      glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    return;
  }

  actor->SetImages(no);

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers(gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
}

int GetClickImgNo(int x)
{
  return actor->GetClickImgNo(x);  
}
