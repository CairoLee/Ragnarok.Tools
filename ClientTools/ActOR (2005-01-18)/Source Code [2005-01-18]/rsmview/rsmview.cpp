#include <float.h>

#include "rsmview.h"

CRsmview::CRsmview(CPAK *pak1)
{
  rsm = NULL;
  pak = pak1;
  dispmode = 2;
  bgcol.r = 0.75;
  bgcol.g = 0.75;
  bgcol.b = 0.75;
}

CRsmview::~CRsmview()
{
  RSMFree();
}

void CRsmview::RSMFree(void)
{
  if (rsm)  delete rsm;
  rsm = NULL;
}


void CRsmview::OpenRSM(char *fname)
{
  if (rsm)  delete rsm;
  rsm = new CRSM();
  rsm->pak = pak;
  rsm->camera = &camera;

  BYTE *dat;
  pak->Uncomp2tmp(fname);
  if (pak->tlen == 0)  return;
  dat = new BYTE[pak->tlen];
  memcpy(dat, pak->tdata, pak->tlen);
  rsm->DecodeAndSet(dat);
  delete dat;

  float xmin, xmax, ymin, ymax, zmin, zmax;
  float x, y, z;
  xmin = ymin = zmin = FLT_MAX;
  xmax = ymax = zmax = FLT_MIN;
  for (int i=0; i<rsm->numofvert; i++) {
    x = rsm->vert[i].x;
    y = rsm->vert[i].y;
    z = rsm->vert[i].z;
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
    if (z < zmin) zmin = z;
    if (z > zmax) zmax = z;
  }
  SetBoundBox(xmin, xmax, ymin, ymax, zmin, zmax);
  InitOpenGL();
  SetCameraDefault(true);
}

void CRsmview::SetBoundBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
  BOX *box = &bound_box;
                                                                                
  SetVertex(&box->n[0], xmin, ymin, zmin);
  SetVertex(&box->n[1], xmax, ymin, zmin);
  SetVertex(&box->n[2], xmax, ymax, zmin);
  SetVertex(&box->n[3], xmin, ymax, zmin);
  SetVertex(&box->n[4], xmin, ymin, zmax);
  SetVertex(&box->n[5], xmax, ymin, zmax);
  SetVertex(&box->n[6], xmax, ymax, zmax);
  SetVertex(&box->n[7], xmin, ymax, zmax);
}

void CRsmview::SetVertex(Vec *v, float x, float y, float z)
{
  v->x = x;
  v->y = y;
  v->z = z;
}

void CRsmview::Draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (rsm == NULL)  return;
  CameraSet();

  if (dispmode <= 1) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_FALSE);
    rsm->OGLExecFill(true);
    glDepthMask(GL_TRUE);
    rsm->OGLExecFill(false);
  }
  if (dispmode >= 1) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3ub(0xFF, 0xFF, 0xFF);
    rsm->OGLExecLine();
  }
  DrawGuide();
}

void CRsmview::DrawGuide(void)
{
  Vec cp, tmp;
  CAM_STATE *cs;
                                                                                
  if (opState == NOSTATE || opState == PAN) return;
                                                                                
  glDisable(GL_DEPTH_TEST);
  cs = &camera;
  if (opState != OBJ_ROT) {
    cp = cs->rot_center;
  } else {
    cp = cs->rot_center2;
  }
                                                                                
  if (opState != SCRN_ROT) {
    glBegin(GL_LINES);
      glColor3ub(255, 0, 0);
      // +
      tmp = cp - cs->u_v * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp + cs->u_v * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp - cs->v_v * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp + cs->v_v * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
    glEnd();
  } else {
    glBegin(GL_LINES);
      glColor3ub(255, 0, 0);
      // X
      tmp = cp - (cs->u_v + cs->v_v) * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp + (cs->u_v + cs->v_v) * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp - (cs->u_v - cs->v_v) * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      tmp = cp + (cs->u_v - cs->v_v) * cs->dist/30.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      // |
      glVertex3f(cp.x, cp.y, cp.z);
      tmp = cp - cs->v_v * cs->dist/10.f;
      glVertex3f(tmp.x, tmp.y, tmp.z);
      // --
      cp = tmp - cs->u_v * cs->dist/60.f;
      glVertex3f(cp.x, cp.y, cp.z);
      tmp = cp + cs->u_v*cs->dist/60.f*2;
      glVertex3f(tmp.x, tmp.y, tmp.z);
    glEnd();
  }
  glEnable(GL_DEPTH_TEST);
}

void CRsmview::InitOpenGL(void)
{
  glViewport(0, 0, oglwidth, oglheight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, oglwidth, 0, oglheight, -10, 10);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(bgcol.r, bgcol.g, bgcol.b, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
#ifndef USE_LINE
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#else
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  glFlush();
}

void CRsmview::SetCameraDefault(bool distchange)
{
  camera.eye_v.x = 0;
  camera.eye_v.y = 0;
  camera.eye_v.z = -1;
  camera.c_p.x = (GLfloat)((bound_box.n[1].x + bound_box.n[0].x) / 2.);
  camera.c_p.y = (GLfloat)((bound_box.n[2].y + bound_box.n[1].y) / 2.);
  camera.c_p.z = bound_box.n[4].z;
  camera.v_v.x = 0;
  camera.v_v.y = 1;
  camera.v_v.z = 0;
  camera.u_v.x = 1;
  camera.u_v.y = 0;
  camera.u_v.z = 0;
  if (distchange) {
    if (fabs(bound_box.n[0].x-bound_box.n[1].x) > fabs(bound_box.n[1].y-bound_box.n[2].y)) {
      camera.dist = (GLfloat)(fabs(bound_box.n[0].x-bound_box.n[1].x)/2.);
    } else {
      camera.dist = (GLfloat)(fabs(bound_box.n[1].y-bound_box.n[2].y)/2.);
    }
  }
}

void CRsmview::CameraSet(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-camera.dist*2 , camera.dist*2,
          -camera.dist*2 / (oglwidth / (float)oglheight),
           camera.dist*2 / (oglwidth / (float)oglheight),
          //-camera.dist * 10, camera.dist * 10);
          -5000, 5000);
  /*
  glOrtho(-oglwidth, oglwidth, -oglheight, oglheight,
          -5000, 5000);
  */
  {
    GLfloat ex, ey, ez, cx, cy, cz, ux, uy, uz;
    cx = camera.c_p.x;
    cy = camera.c_p.y;
    cz = camera.c_p.z;
    ex = cx - camera.eye_v.x * camera.dist;
    ey = cy - camera.eye_v.y * camera.dist;
    ez = cz - camera.eye_v.z * camera.dist;
    ux = camera.v_v.x;
    uy = camera.v_v.y;
    uz = camera.v_v.z;
    gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);
  }
}

void CRsmview::GLOBJRot(void)
{
  Vec tmp;
                                                                                
  GLfloat rotangle_deg;
  rotangle_deg = (GLfloat)(old_mp.x - new_mp.x)*(old_mp.x - new_mp.x);
  rotangle_deg += (old_mp.y - new_mp.y)*(old_mp.y - new_mp.y);
  rotangle_deg = (GLfloat)sqrt(rotangle_deg);
                                                                                
  Vec RotAxis;
  GLfloat axisangle_deg;
  axisangle_deg = (GLfloat)atan2((double)-(new_mp.y - old_mp.y), (double)-(new_mp.x - old_mp.x));
  axisangle_deg -= M_PI / 2.f;
  axisangle_deg *= 180.f / M_PI;
  RotAxis = camera.u_v;
                                                                                
  camera.eye_v.ThisAxisRot(&RotAxis, axisangle_deg);
                                                                                
  tmp = camera.rot_center2 - camera.c_p;
  RotAxis.ThisAxisRot(&tmp, rotangle_deg);
  camera.c_p = camera.c_p + (camera.rot_center2 - camera.c_p) - tmp;
  RotAxis.ThisAxisRotOffNorm(&camera.eye_v, rotangle_deg, &camera.rot_center);
  RotAxis.ThisAxisRotOffNorm(&camera.u_v, rotangle_deg, &camera.rot_center);
  RotAxis.ThisAxisRotOffNorm(&camera.v_v, rotangle_deg, &camera.rot_center);
}

void CRsmview::GLSCRNRot(void)
{
  Vec tmp;
                                                                                
  GLfloat rotangle_deg = (GLfloat)-(old_mp.x - new_mp.x);
                                                                                
  tmp = camera.rot_center - camera.c_p;
  camera.eye_v.ThisAxisRot(&tmp, rotangle_deg);
  camera.c_p = camera.c_p + (camera.rot_center - camera.c_p) - tmp;
                                                                                
  camera.eye_v.ThisAxisRot(&camera.v_v, rotangle_deg);
  camera.eye_v.ThisAxisRot(&camera.u_v, rotangle_deg);
  camera.u_v.normalize();
  camera.v_v.normalize();
}

void CRsmview::GLZoom(void)
{
  GLfloat olddist;
                                                                                
  olddist = camera.dist;
  camera.dist += (GLfloat)((old_mp.y - new_mp.y) * 0.005 * camera.dist);
  if (camera.dist < 0.001) camera.dist = (GLfloat)0.001;
                                                                                
  camera.c_p = camera.c_p
             - camera.u_v*(first_mp.x - oglwidth/2.f)
             / (float)oglwidth * (camera.dist - olddist)*4;
  camera.c_p = camera.c_p
             + camera.v_v*(first_mp.y - oglheight/2.f)
             / (float)oglheight * (camera.dist - olddist)*4 / (oglwidth / (float)oglwidth);
}

void CRsmview::GLPan(void)
{
  camera.c_p = camera.c_p
             + camera.u_v * ((old_mp.x - new_mp.x) * camera.dist*4
             / (float)oglwidth);
  camera.c_p = camera.c_p
             - camera.v_v * ((old_mp.y - new_mp.y) * camera.dist*4
             / (float)oglheight  / (oglwidth / (float)oglwidth) );
}

void CRsmview::SearchRotOrigin(void)
{
  Vec norm3pplane;
  Vec intersect_p;
  float mindist = FLT_MAX;
  float tmpdist;
  Vec a, b, c;

  if (rsm == NULL)  return;
 
  // 各面とクリック点を通るcamera.eye_vベクトルとの最短点を求める  
  for (int i=0; i<rsm->numofsurf; i++) {
    a = rsm->vert[rsm->surf[i].sv[0]];
    b = rsm->vert[rsm->surf[i].sv[1]];
    c = rsm->vert[rsm->surf[i].sv[2]];
    norm3pplane = a.CalcPlaneNorm(b, c);
    intersect_p = camera.first_p.CalcIntersectPoint(norm3pplane, a, camera.eye_v);
    if (intersect_p.CheckInTriangle(a, b, c)) {
      tmpdist = intersect_p.CalcDistance2(camera.rot_center);
      if (mindist > tmpdist) {
        mindist = tmpdist;
        camera.rot_center2 = intersect_p;
      }
    }
  }
 
  if (mindist == FLT_MAX) {
    camera.rot_center2 = (camera.first_p + camera.rot_center) / 2.f;
  }
}

void CRsmview::CalcCenter(void)
{
    camera.rot_center = camera.c_p
                   + camera.u_v * (old_mp.x - oglwidth/2.f)
                   * camera.dist*4 / (float)oglwidth;
    camera.rot_center = camera.rot_center
                   - camera.v_v * (old_mp.y - oglheight/2.f)
                   * camera.dist*4 / (float)oglheight / (oglwidth/(float)oglheight);
    camera.first_p = camera.rot_center - camera.eye_v * camera.dist;
}

