#include "rsm.h"

CRSM::CRSM(void)
{
  numoftex = 0;
  textures = NULL;
  gltextures = NULL;
  texsizes = NULL;
  vert = NULL;
  texv = NULL;
  surf = NULL;
  zsortstore = NULL;
  linergb.r = 1;
  linergb.b = 1;
  linergb.g = 1;
}

CRSM::~CRSM(void)
{
  ObjectFree();
}

void CRSM::ObjectFree(void)
{
  if (!this) {
    return;
  }
  glDeleteTextures(numoftex, gltextures);
  delete gltextures;
  for (int i=0; i<numoftex; i++) {
    delete textures[i];
  }
  delete textures;
  delete texsizes;
  delete vert;
  delete texv;
  delete surf;
  delete zsortstore;
}

void CRSM::OGLExecFill(bool sortflag)
{
  Vec *sa, *sb, *sc;
  Vec *ta, *tb, *tc;
  int sid;
  int w=0, h=0, tw=1, th=1;
  float tx, ty;

  if (sortflag) {
    Vec eyepoint;
    eyepoint = camera->c_p - camera->dist * camera->eye_v;
    for (int i=0; i<numofsurf; i++) {
      sa = &vert[ surf[i].sv[0] ];
      sb = &vert[ surf[i].sv[1] ];
      sc = &vert[ surf[i].sv[2] ];
      zsortstore[i].dist = eyepoint.CalcLineParam(camera->eye_v, *sa, *sb, *sc);
      zsortstore[i].id = i;
    }
    qsort(zsortstore, numofsurf, sizeof(SURFZSORT), zsortfunc);
  }

  glEnable(GL_TEXTURE_2D);
  nowtexno = -1;
  for (int i=0; i<numofsurf; i++) {
    if (sortflag) {
      sid = zsortstore[i].id;
    } else {
      sid = i;
    }
    sa = &vert[ surf[sid].sv[0] ];
    sb = &vert[ surf[sid].sv[1] ];
    sc = &vert[ surf[sid].sv[2] ];
    ta = &texv[ surf[sid].tv[0] ];
    tb = &texv[ surf[sid].tv[1] ];
    tc = &texv[ surf[sid].tv[2] ];
    if (nowtexno != surf[sid].texid) {
      nowtexno = surf[sid].texid;
      glBindTexture(GL_TEXTURE_2D, gltextures[ nowtexno ]);
      w = texsizes[ nowtexno ].width;
      h = texsizes[ nowtexno ].height;
      tw = texsizes[ nowtexno ].texwidth;
      th = texsizes[ nowtexno ].texheight;
    }
    glColor4f(1, 1, 1, 1);
    glBegin(GL_TRIANGLES);
      tx = ta->y*w/(float)tw;
      ty = (1-ta->z)*h/(float)th;
      glTexCoord2f(tx, ty);
      glVertex3f(sa->x, sa->y, sa->z);
      tx = tb->y*w/(float)tw;
      ty = (1-tb->z)*h/(float)th;
      glTexCoord2f(tx, ty);
      glVertex3f(sb->x, sb->y, sb->z);
      tx = tc->y*w/(float)tw;
      ty = (1-tc->z)*h/(float)th;
      glTexCoord2f(tx, ty);
      glVertex3f(sc->x, sc->y, sc->z);
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

void CRSM::OGLExecLine(void)
{
  Vec sa, sb, sc;

  for (int i=0; i<numofsurf; i++) {
    sa = vert[surf[i].sv[0]];
    sb = vert[surf[i].sv[1]];
    sc = vert[surf[i].sv[2]];
    glLineWidth(surf[i].xxx[3]);
    glBegin(GL_TRIANGLES);
      glColor3f(linergb.r, linergb.g, linergb.b);
      glVertex3f(sa.x, sa.y, sa.z);
      glVertex3f(sb.x, sb.y, sb.z);
      glVertex3f(sc.x, sc.y, sc.z);
    glEnd();
    glLineWidth(1.f);
  }
}

void CRSM::DecodeAndSet(BYTE *dat)
{
  WORD version;
  DWORD tmp;

  ObjectFree();

  dat += 4;  //magic
  version = *(WORD *)dat;  dat += 2;
  if (version != 0x0201 && version != 0x0301 && version != 0x0401) {
    printf("can't view version %04X\n", (WORD)(version << 8 | version >> 8));
    return;
  }

  if (version == 0x0201 || version == 0x0301) {
    dat += 24;
  } else {
    dat += 25;
  }

  LoadTexture(dat);
  dat += 4+numoftex*40;

  memcpy(objname, (const char *)dat, 40);
  dat += 40;
  dat += 4+40;
  dat += 4+36;
  tmp = *(DWORD *)dat;
  dat += tmp*4+4;
  dat += 0x30+0xc+0x4+0xc+0xc;

  LoadVertSurf(dat);
  try {
    zsortstore = new SURFZSORT[numofsurf];
  } catch (...) {
    printf("can't allocate memory.\n");
  }
}

void CRSM::LoadVertSurf(BYTE *dat)
{
  numofvert = *(DWORD *)dat;
  dat += 4;
  try {
    vert = new Vec[numofvert];
  } catch (...) {
    printf("can't allocate memory.\n");
  }

  for (int i=0; i<numofvert; i++, dat+=sizeof(float)*3) {
    memcpy(&vert[i], dat, sizeof(float)*3);
  }

  numoftexv = *(DWORD *)dat;
  dat += 4;
  try {
    texv = new Vec[numoftexv];
  } catch (...) {
    printf("can't allocate memory.\n");
  }
  for (int i=0; i<numoftexv; i++, dat+=sizeof(float)*3) {
    memcpy(&texv[i], dat, sizeof(float)*3);
  }

  numofsurf = *(DWORD *)dat;
  dat += 4;
  try {
    surf = new SURFACE[numofsurf];
  } catch (...) {
    printf("can't allocate memory.\n");
  }
  for (int i=0; i<numofsurf; i++, dat+=sizeof(SURFACE)) {
    memcpy(&surf[i], dat, sizeof(SURFACE));
  }
}

void CRSM::SaveModelAsX(char *fname)
{
  FILE *fp;

  fp = fopen(fname, "w");
  fprintf(fp, "xof 0303txt 0032\n");
  fprintf(fp, "Mesh Mesh1 {\n");
  fprintf(fp, "  %d;\n", numofvert);
  for (int i=0; i<numofvert-1; i++) {
    fprintf(fp, "  %f;%f;%f;,\n", vert[i].x, vert[i].y, vert[i].z);
  }
  fprintf(fp, "  %f;%f;%f;;\n", vert[numofvert-1].x, vert[numofvert-1].y, vert[numofvert-1].z);
  fprintf(fp, "  %d;\n", numofsurf);
  for (int i=0; i<numofsurf-1; i++) {
    fprintf(fp, "  3;%d;%d;%d;,\n", surf[i].sv[0], surf[i].sv[1], surf[i].sv[2]);
  }
  fprintf(fp, "  3;%d;%d;%d;;\n", surf[numofsurf-1].sv[0], surf[numofsurf-1].sv[1], surf[numofsurf-1].sv[2]);
  fprintf(fp, "}\n");

  /*
  for (int i=0; i<numofvert; i++) {
    fprintf(fp, "%d %e %e %e\n", i, vert[i].x, vert[i].y, vert[i].z);
  }
  for (int i=0; i<numoftexv; i++) {
    fprintf(fp, "%d %e %e %e\n", i, texv[i].x, texv[i].y, texv[i].z);
  }
  for (int i=0; i<numofsurf; i++) {
    fprintf(fp, "%3d %3d %3d %3d, %3d %3d %3d, %2d, %04x %d %d %d %d\n", i,
      surf[i].sv[0], surf[i].sv[1], surf[i].sv[2],
      surf[i].tv[0], surf[i].tv[1], surf[i].tv[2],
      surf[i].texid, surf[i].xxx[0], surf[i].xxx[1],
      surf[i].xxx[2], surf[i].xxx[3], surf[i].xxx[4]);
  }
  */
  fclose(fp);

}

void CRSM::LoadTexture(BYTE *dat)
{
  char texname[FILENAME_MAX];

  numoftex = *(DWORD *)dat;
  dat += 4;
  try {
    textures = new DWORD *[numoftex];
    texsizes = new SprSize[numoftex];
  } catch (...) {
    printf("can't allocate memory.\n");
  }
  char strr[FILENAME_MAX];
  for (int i=0; i<numoftex; i++, dat+=40) {
    if ((pak->pakversion[0] & 0xFF00) == 0xFF00) {
      strcpy(texname, "data/texture/");
      for (int j=0; j<40; j++) {
        if (*(dat+j) == '\\')  *(dat+j) = '/';
        if (*(dat+j) == '\0')  break;
      }
    } else {
      strcpy(texname, "data\\texture\\");
    }
    strcpy(strr, (const char *)dat);
    for (int j=(int)strlen(strr)-1; j>=0; j--) {
      if ('A' <= strr[j] && strr[j] <= 'Z')  strr[j] += 'a' - 'A';
      if (strr[j] == '\\')  break;
    }
    strcat(texname, strr);
    textures[i] = NULL;
    Set1Texture(texname, i); 
  }

  try {
    gltextures = new GLuint[numoftex];
  } catch (...) {
    printf("can't allocate memory.\n");
  }
  glGenTextures(numoftex, gltextures);
  for (int i=0; i<numoftex; i++) {
    glBindTexture(GL_TEXTURE_2D, gltextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, 4,
                 texsizes[ i ].texwidth,
                 texsizes[ i ].texheight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 textures[ i ]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
}

void CRSM::Set1Texture(char *filename, int no)
{
  BYTE *dat, *datp, *pal;
  BYTE r, g, b;
  int w, h, tw, th;
  DWORD val;

  if (pak->SearchEntry(filename) == NULL) {
    printf("texture %s is not found.\n", filename);
    return;
  }
  pak->Uncomp2tmp(filename);
  dat = pak->tdata;
  datp = dat;

  if (STRNCASECMP(&filename[strlen(filename)-3], "bmp", 3) == 0) {
    w = texsizes[no].width = *(DWORD *)(dat+0x12);
    w = w + w%2;
    h = texsizes[no].height = *(DWORD *)(dat+0x16);
    tw = texsizes[no].texwidth = Get2NSize(w);
    th = texsizes[no].texheight = Get2NSize(h);
    if (*(WORD *)(dat+0x1c) == 8) {
      try {
        textures[no] = new DWORD[tw*th];
      } catch (...) {
        printf("can't allocate memory.\n");
      }
      pal = dat+0x36;
      dat += *(DWORD *)(dat+0xa);
      for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++, dat++) {
          b = *(pal+*dat*4);
          g = *(pal+*dat*4+1);
          r = *(pal+*dat*4+2);
          if (r >= 0xfe && b >= 0xfe && g == 0) {
            val = 0x00000000;
          } else {
            val = 0xff000000 | b << 16 | g << 8 | r;
          }
          textures[no][y*tw+x] = val;
        }
      }
    } else if (*(WORD *)(dat+0x1c) == 24) {
      try {
        textures[no] = new DWORD[tw*th];
      } catch (...) {
        printf("can't allocate memory.\n");
      }
      dat += 0x36;
      for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++, dat+=3) {
          b = *(dat+0);
          g = *(dat+1);
          r = *(dat+2);

          if (r == 0xff && b == 0xff) {
            val = 0x00000000;
          } else {
            val = 0xff000000 | b << 16 | g << 8 | r;
          }
  
          textures[no][y*tw+x] = val;
        }
      }
    }
  } else if (STRNCASECMP(&filename[strlen(filename)-3], "tga", 3) == 0) {
    w = texsizes[no].width = *(WORD *)(dat+0xC);
    h = texsizes[no].height = *(WORD *)(dat+0xE);
    tw = texsizes[no].texwidth = Get2NSize(w);
    th = texsizes[no].texheight = Get2NSize(h);
    try {
      textures[no] = new DWORD[tw*th];
    } catch (...) {
      printf("can't allocate memory.\n");
    }
    int offs = 0x12;
    BYTE a;
    for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
        b = *(dat + y*w*4 + x*4 + offs + 0);
        g = *(dat + y*w*4 + x*4 + offs + 1);
        r = *(dat + y*w*4 + x*4 + offs + 2);
        a = *(dat + y*w*4 + x*4 + offs + 3);
        val = a << 24 | b << 16 | g << 8 | r;
        //textures[no][(h-y-1)*tw+x] = val;
        textures[no][y*tw+x] = val;
      }
    }
  } else {
    printf("#%d texture %s read fail.", no, filename);
  }
}

int zsortfunc(const void *arg1, const void *arg2)
{
  SURFZSORT *a = (SURFZSORT *)arg1;
  SURFZSORT *b = (SURFZSORT *)arg2;

  return a->dist > b->dist ? -1 : 1;
  /*
  if (a->dist > b->dist) {
    return -1;
  } else if (a->dist == b->dist) {
    return 0;
  } else {
    return 1;
  }
  */
}

int Get2NSize(int size)
{
  if (size <= 64) {
    return 64;
  } else if (size <= 128) {
    return 128;
  } else if (size <= 256) {
    return 256;
  } else if (size <= 512) {
    return 512;
  }
  return 1024;
}

