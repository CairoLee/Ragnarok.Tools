#include "chrview.h"

CChrView::CChrView(CPAK *pak)
{
  pak1 = pak;
  curPat = 0;
  numPat = 0;
  inMDragging = inRDragging = false;
  SetNullSprites();
}

CChrView::~CChrView()
{
  DestroySpriteAll();

  DestroyNFD(&NFDMob);
  DestroyNFD(&NFDPCBodyM);
  DestroyNFD(&NFDPCBodyF);
  DestroyNFD(&NFDPCAccM);
  DestroyNFD(&NFDPCAccF);
  DestroyNFD_WeapShield();
  DestroyNFD(&NFDNPC);
}

void CChrView::DestroySpriteAll(void)
{
  DestroySprite(&sprite_Body);
  DestroySprite(&sprite_Head);
  DestroySprite(&sprite_Acc1);
  DestroySprite(&sprite_Acc2);
  DestroySprite(&sprite_Acc3);
  DestroySprite(&sprite_Weapon);
  DestroySprite(&sprite_WpAftimg);
  DestroySprite(&sprite_Shield);
}

void CChrView::DestroySprite(CSprite **sprite)
{
  if (*sprite) {
    delete *sprite;
    *sprite = NULL;
  }
}

void CChrView::DestroyNFD(NFD *nfd)
{
  for (int i=0; i<nfd->num; i++) {
    free( (nfd->name + i)->dispname );
    free( (nfd->name + i)->filename );
  }
}

void CChrView::DestroyNFD_WeapShield(void)
{
  DestroyNFD(&NFDPCWeapon);
  DestroyNFD(&NFDPCShield);

  NFDPCWeapon.num = NFDPCShield.num = 0;
  NFDPCWeapon.name = NFDPCShield.name = NULL;
}


void CChrView::ClearNFD(void)
{
  NFDMob.num = NFDPCBodyM.num = NFDPCBodyF.num = 0;
  NFDPCAccM.num = NFDPCAccF.num = NFDNPC.num = 0;

  NFDMob.name = NFDPCBodyM.name = NFDPCBodyF.name = NULL;
  NFDPCAccM.name = NFDPCAccF.name = NFDNPC.name = NULL;
}

void CChrView::InitNFD(void)
{
  char *fnp, *fnp2;

  ClearNFD();

  for (int i=0; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    fnp = pak1->Entries[i].fn;
    fnp2 = &fnp[strlen(fnp)-3];
    if (STRCASECMP(fnp2, "act") != 0)  continue;
    if (strstr(fnp, MOB_DIR) != 0) {
      InitNFDMob(fnp, &NFDMob);
    } else if (strstr(fnp, PCBODY_M_DIR) != 0) {
      InitNFDPCBody(fnp, &NFDPCBodyM, "\137\263\262");
    } else if (strstr(fnp, PCBODY_F_DIR) != 0) {
      InitNFDPCBody(fnp, &NFDPCBodyF, "\137\277\251");
    } else if (strstr(fnp, PCACC_M_DIR) != 0) {
      InitNFDPCAcc(fnp, &NFDPCAccM);
    } else if (strstr(fnp, PCACC_F_DIR) != 0) {
      InitNFDPCAcc(fnp, &NFDPCAccF);
    } else if (strstr(fnp, NPC_DIR) != 0) {
      InitNFDNPC(fnp, &NFDNPC);
    }
  }
  qsort(NFDMob.name, NFDMob.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(NFDPCBodyM.name, NFDPCBodyM.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(NFDPCBodyF.name, NFDPCBodyF.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(NFDPCAccM.name, NFDPCAccM.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(NFDPCAccF.name, NFDPCAccF.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(NFDNPC.name, NFDNPC.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
}

void CChrView::InitNFDMob(char *fnp, NFD *nfd)
{
  char n[128], n2[128];

  TrimDirSuffix(n2, fnp, sizeof(MOB_DIR));
  strcpy(n, _(n2));
  InitNFDADD(n, n2, nfd);
}

void CChrView::InitNFDPCBody(char *fnp, NFD *nfd, char *suffix)
{
  char n[128], n2[128], n3[128];

  TrimDirSuffix(n2, fnp, sizeof(PCBODY_M_DIR));
  strcpy(n3, n2);
  if (strcmp(&n2[strlen(n2)-3], suffix) == 0) {
    n2[strlen(n2)-3] = '\0';
  }
  strcpy(n, _(n2));
  InitNFDADD(n, n3, nfd);
}

void CChrView::InitNFDPCAcc(char *fnp, NFD *nfd)
{
  char n[128], n2[128], n3[128], *n2_1;

  TrimDirSuffix(n2, fnp, sizeof(PCACC_M_DIR));
  strcpy(n3, n2);
  n2_1 = &n2[3];
  strcpy(n, _(n2_1));
  InitNFDADD(n, n3, nfd);
}

void CChrView::InitNFDNPC(char *fnp, NFD *nfd)
{
  char n[128], n2[128];

  TrimDirSuffix(n2, fnp, sizeof(NPC_DIR));
  strcpy(n, _(n2));
  InitNFDADD(n, n2, nfd);
}

void CChrView::InitNFDADD(char *dn, char *fn, NFD *nfd)
{
  nfd->name = (NAME_FvsD *)realloc(nfd->name, sizeof(NAME_FvsD) * (nfd->num+1));
#ifdef DISTRIBUTE
  (nfd->name + nfd->num)->dispname = (char *)malloc(strlen(dn)+1);
  strcpy((nfd->name + nfd->num)->dispname, dn);
#else
  char dn2[FILENAME_MAX];
  PO_STR_SET(dn2, dn);
  (nfd->name + nfd->num)->dispname = (char *)malloc(strlen(dn2)+1);
  strcpy((nfd->name + nfd->num)->dispname, dn2);
#endif
  (nfd->name + nfd->num)->filename = (char *)malloc(strlen(fn)+1);
  strcpy((nfd->name + nfd->num)->filename, fn);
  (nfd->num)++;
}

void CChrView::TrimDirSuffix(char *to, char *from, int leftlen)
{
  strcpy(to, &from[leftlen-1]);
  to[strlen(to)-4] = '\0';
}

void CChrView::InitControlsWeaponShieldPre(void)
{
  NFDPCWeapon.num = NFDPCShield.num = 0;
  NFDPCWeapon.name = NFDPCShield.name = NULL;
}

void CChrView::InitOpenGL()
{
  glViewport(0, 0, GL_WIN_WIDTH, GL_WIN_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, GL_WIN_WIDTH, 0, GL_WIN_HEIGHT, -10, 10);
  glMatrixMode(GL_MODELVIEW);
  float r, g, b;
  r = GETCOL_R(colBG) / 255.f;
  g = GETCOL_G(colBG) / 255.f;
  b = GETCOL_B(colBG) / 255.f;
  glClearColor(r, g, b, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  //glEnable(GL_ALPHA_TEST);
#ifndef USE_LINE
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#else
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  glFlush();
}

void CChrView::SetCameraDefault(void)
{
  camera.eye_v.x = 0;
  camera.eye_v.y = 0;
  camera.eye_v.z = -1;
  camera.c_p.x = GL_WIN_WIDTH / 2.;
  camera.c_p.y = GL_WIN_HEIGHT / 2.;
  camera.c_p.z = -GL_WIN_WIDTH / 4.f;
  camera.v_v.x = 0;
  camera.v_v.y = 1;
  camera.v_v.z = 0;
  camera.u_v.x = 1;
  camera.u_v.y = 0;
  camera.u_v.z = 0;
  camera.dist = GL_WIN_WIDTH / 4.f;
}


void CChrView::CameraSet(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-camera.dist*2 , camera.dist*2,
          -camera.dist*2 , camera.dist*2,
          //-camera.dist * 10, camera.dist * 10);
          -5000, 5000);
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

void CChrView::GLZoom(void)
{
  GLfloat olddist;

  olddist = camera.dist;
  camera.dist += (GLfloat)((old_mp.y - new_mp.y) * 0.005 * camera.dist);
  if (camera.dist < 0.001) camera.dist = (GLfloat)0.001;

  camera.c_p = camera.c_p
             - camera.u_v*(first_mp.x - GL_WIN_WIDTH/2.f)
             / (float)GL_WIN_WIDTH * (camera.dist - olddist)*4;
  camera.c_p = camera.c_p
             + camera.v_v*(first_mp.y - GL_WIN_HEIGHT/2.f)
             / (float)GL_WIN_HEIGHT * (camera.dist - olddist)*4 / (GL_WIN_WIDTH / (float)GL_WIN_WIDTH);
}

void CChrView::GLPan(void)
{
  camera.c_p = camera.c_p
             + camera.u_v * ((old_mp.x - new_mp.x) * camera.dist*4
             / (float)GL_WIN_WIDTH);
  camera.c_p = camera.c_p
             - camera.v_v * ((old_mp.y - new_mp.y) * camera.dist*4
             / (float)GL_WIN_HEIGHT  / (GL_WIN_WIDTH / (float)GL_WIN_WIDTH) );
}

bool CChrView::SpriteSetMob(CSprite **sprite, char *dispname)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];
  char *fname_base;

  fname_base = GetNFDFnameFromDname(&NFDMob, dispname);
  if (fname_base == NULL)  return false;
  sprintf(actname, "%s%s.act", MOB_DIR, fname_base);
  ChangeToSpr(fname_base, sprname);
  return SpriteSet(sprite, actname, sprname);
}

bool CChrView::SpriteSetPCBody(CSprite **sprite, char *dispname, char *fname_base, bool male)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];

  DestroySprite(sprite);

  if (male) {
    fname_base = GetNFDFnameFromDname(&NFDPCBodyM, dispname);
    sprintf(actname, "%s%s.act", PCBODY_M_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCBODY_M_DIR, fname_base);
  } else {
    fname_base = GetNFDFnameFromDname(&NFDPCBodyF, dispname);
    sprintf(actname, "%s%s.act", PCBODY_F_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCBODY_F_DIR, fname_base);
  }
  if (SpriteSet(sprite, actname, sprname) == false)  return false;

  return true;
}

void CChrView::DestroySprites_GM2(void)
{
  DestroySprite(&sprite_Head);
  DestroySprite(&sprite_Acc1);
  DestroySprite(&sprite_Acc2);
  DestroySprite(&sprite_Acc3);
}

bool CChrView::SpriteSetPCHead(CSprite **sprite, bool male)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];
  char fname_base[100];

  DestroySprite(sprite);
  if (curHead == 0) {
    return true;
  }

  if (male) {
    sprintf(fname_base, "%d_\263\262", curHead);
    sprintf(actname, "%s%s.act", PCHEAD_M_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCHEAD_M_DIR, fname_base);
  } else {
    sprintf(fname_base, "%d_\277\251", curHead);
    sprintf(actname, "%s%s.act", PCHEAD_F_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCHEAD_F_DIR, fname_base);
  }
  return SpriteSet(sprite, actname, sprname);
}

bool CChrView::SpriteSetPCAcc(CSprite **sprite, char *dispname, int no, bool male)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];
  char *fname_base;

  DestroySprite(sprite);
  switch (no) {
  case 1:
    if (curAcc1 == 0)  return true;
    break;
  case 2:
    if (curAcc2 == 0)  return true;
    break;
  case 3:
    if (curAcc3 == 0)  return true;
    break;
  }

  if (male) {
    fname_base = GetNFDFnameFromDname(&NFDPCAccM, dispname);
    sprintf(actname, "%s%s.act", PCACC_M_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCACC_M_DIR, fname_base);
  } else {
    fname_base = GetNFDFnameFromDname(&NFDPCAccF, dispname);
    sprintf(actname, "%s%s.act", PCACC_F_DIR, fname_base);
    sprintf(sprname, "%s%s.spr", PCACC_F_DIR, fname_base);
  }
  return SpriteSet(sprite, actname, sprname);
}

bool CChrView::SpriteSetPCWeapon(CSprite **sprite, char *dispname, char *bodyname, char *sexname)
{
  char *fname_base;
  char fname[FILENAME_MAX];

  if (strcmp(dispname, " nothing") == 0) {
    DestroySprite(sprite);
    return false;
  }
  fname_base = GetNFDFnameFromDname(&NFDPCWeapon, dispname);
  sprintf(fname, "%s%s\\%s_%s_%s", WEAPON_DIR, bodyname, bodyname, sexname, fname_base);

  char actname[FILENAME_MAX], sprname[FILENAME_MAX];

  sprintf(actname, "%s.act", fname);
  sprintf(sprname, "%s.spr", fname);
  if (SpriteSet(sprite, actname, sprname) == false)  return false;

  return SpriteSetPCWeaponAftImage(fname, bodyname, sexname, atoi(fname_base));
}

bool CChrView::SpriteSetPCShield(CSprite **sprite, char *bodyname, char *sexname, char *dispname)
{
  char *fname_base;
  char fname[FILENAME_MAX];

  if (strcmp(dispname, " nothing") == 0) {
    DestroySprite(sprite);
    return false;
  }
  fname_base = GetNFDFnameFromDname(&NFDPCShield, dispname);
  sprintf(fname, "%s%s\\%s_%s_%s", SHIELD_DIR, bodyname, bodyname, sexname, fname_base);

  char actname[FILENAME_MAX], sprname[FILENAME_MAX];

  sprintf(actname, "%s.act", fname);
  sprintf(sprname, "%s.spr", fname);
  return SpriteSet(sprite, actname, sprname);
}

bool CChrView::SpriteSetPCWeaponAftImage(char *fname, char *bodyname, char *sexname, int idWeapon)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];

  if (idWeapon == 0) {
    sprintf(actname, "%s%s.act", fname, AFTIMG_ADD_STR);
    sprintf(sprname, "%s%s.spr", fname, AFTIMG_ADD_STR);
  } else {
    sprintf(fname, "%s%s\\%s_%s_", WEAPON_DIR, bodyname, bodyname, sexname);
    GetItemNoToWeaponType(fname, idWeapon);
    sprintf(actname, "%s.act", fname, AFTIMG_ADD_STR);
    sprintf(sprname, "%s.spr", fname, AFTIMG_ADD_STR);
  }
  return SpriteSet(&sprite_WpAftimg, actname, sprname);
}

bool CChrView::SpriteSetNPC(CSprite **sprite, char *dispname)
{
  char actname[FILENAME_MAX], sprname[FILENAME_MAX];
  char *fname_base;

  fname_base = GetNFDFnameFromDname(&NFDNPC, dispname);
  sprintf(actname, "%s%s.act", NPC_DIR, fname_base);
  sprintf(sprname, "%s%s.spr", NPC_DIR, fname_base);
  return SpriteSet(sprite, actname, sprname);
}

bool CChrView::SpriteSet(CSprite **sprite, char *actname, char *sprname)
{
  BYTE *tmpd;

  pak1->Uncomp2tmp(actname);
  if (pak1->tlen == 0) return false;
  tmpd = (BYTE *)malloc(pak1->tlen);
  memcpy(tmpd, pak1->tdata, pak1->tlen);
  pak1->Uncomp2tmp(sprname);
  if (pak1->tlen == 0) {
    free(tmpd);
    return false;
  }

  DestroySprite(sprite);
  *sprite = new CSprite(tmpd, pak1->tdata, pak1->tlen);

  free(tmpd);
  return true;
}

void CChrView::DrawSprites(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  CameraSet();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

#ifndef USE_LINE
  glEnable(GL_TEXTURE_2D);
#endif
  if (2 <= curdirection && curdirection <= 5) {
    if (sprite_Shield) {
      sprite_Shield->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 - 100, curAct, curPat);
    }
  }

  int extx, exty;
  extx = exty = 0;
  if (sprite_Body) {
    sprite_Body->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 - 100, curAct, curPat);
    sprite_Body->GetExtOffsPoint(&extx, &exty, curAct, curPat);
  }
  if (sprite_Head) {
    DrawSpritesHead(sprite_Head, extx, exty);
  }
  if (sprite_Acc1) {
    DrawSpritesHead(sprite_Acc1, extx, exty);
  }
  if (sprite_Acc2) {
    DrawSpritesHead(sprite_Acc2, extx, exty);
  }
  if (sprite_Acc3) {
    DrawSpritesHead(sprite_Acc3, extx, exty);
  }
  if (sprite_Weapon) {
    int m;
    m = sprite_Weapon->GetNumPattern(curAct) - 1;
    if (curPat > m) {
      sprite_Weapon->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 - 100, curAct, m);
    } else {
      sprite_Weapon->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 - 100, curAct, curPat);
    }
  }
  if (sprite_WpAftimg) {
    int m;
    m = sprite_WpAftimg->GetNumPattern(curAct) - 1;
    if (curPat > m) {
      sprite_WpAftimg->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 -100, curAct, m);
    } else {
      sprite_WpAftimg->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 -100, curAct, curPat);
    }
  }
  if (curdirection < 2 || 5 < curdirection) {
    if (sprite_Shield) {
      sprite_Shield->DrawPattern(GL_WIN_WIDTH / 2, GL_WIN_HEIGHT / 2 - 100, curAct, curPat);
    }
  }
#ifndef USE_LINE
  glDisable(GL_TEXTURE_2D);
#endif
  glFlush();
}

void CChrView::DrawSpritesHead(CSprite *sprite, int x, int y)
{
  sprite->DrawPatternHead(GL_WIN_WIDTH / 2 + x,
                          GL_WIN_HEIGHT / 2 - 100 - y,
                          curAct, curPat);
}

void CChrView::SetNullSprites(void)
{
  sprite_Body = sprite_Head = NULL;
  sprite_Acc1 = sprite_Acc2 = NULL;
  sprite_Acc3 = sprite_Weapon = NULL;
  sprite_WpAftimg = sprite_Shield = NULL;
}

void CChrView::InitNumActPat(void)
{
  if (sprite_Body) {
    numAct = sprite_Body->GetNumAction();
    numPat = sprite_Body->GetNumPattern(0);
  } else {
    numAct = numPat = 0;
  }
  curdirection = 0;
  curAct = 0;
  curPat = 0;
}

void CChrView::SelectAct(int idAct)
{
  sprready = false;
  curAct = curdirection + 8 * idAct;
  numPat = sprite_Body->GetNumPattern(curAct);
  curPat = 0;
  sprready = true;
}

void CChrView::SelectDirection(int idAct, int idDirection)
{
  sprready = false;
  curdirection = idDirection;
  curAct = curdirection + 8 * idAct;
  if (numAct == 1)  curAct = 0;
  if (numAct == 2)  curAct = curdirection % 2;
  numPat = sprite_Body->GetNumPattern(curAct);
  curPat = 0;
  sprready = true;
}

void CChrView::SetWeaponName(char *fnp, char *bodynameW, char *bodyname2)
{
  char name[100];
  char name2[100];
  char dispname[100];

  strcpy(name2, &fnp[strlen(bodynameW)+strlen(bodyname2)+1+1+2]);  // '(body)\(body)_s_'
  name2[strlen(name2)-4] = '\0';
  if (atoi(name2) == 0) {
    if (strstr(name2, AFTERIMAGE_STR) == 0) {
      strcpy(dispname, name2);
    } else {
      return;
    }
  } else {
    sprintf(dispname, "item%s", name2);
  }
  strcpy(name, _(dispname));
  InitNFDADD(name, name2, &NFDPCWeapon);
}

void CChrView::SetShieldName(char *fnp, char *bodynameS, char *bodyname2)
{
  char name[100];
  char name2[100];
  strcpy(name2, &fnp[strlen(bodynameS)+strlen(bodyname2)+1+1+2]);  // '(body)\(body)_s_'
  name2[strlen(name2)-4] = '\0';
  strcpy(name, _(name2));
  InitNFDADD(name, name2, &NFDPCShield);
}

char *CChrView::GetNFDFnameFromDname(NFD *nfd, char *dispname)
{
  NAME_FvsD tmpfd, *nfdret;

  tmpfd.dispname = (char *)malloc(strlen(dispname)+1);
  strcpy(tmpfd.dispname, dispname); 
  if (nfdret = (NAME_FvsD *)bsearch(&tmpfd, nfd->name, nfd->num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp)) {
    return nfdret->filename;
  }
  return NULL;
}

void CChrView::ChangeToSpr(char *fname, char *sprname)
{
  if (strcmp(fname, BAPHOMET_PET) == 0) {
    sprintf(sprname, "%sbaphomet_.spr", MOB_DIR);
  } else if (strcmp(fname, BON_GON_PET) == 0) {
    sprintf(sprname, "%sbon_gun.spr", MOB_DIR);
  } else if (strcmp(fname, CHOCHO_PET) == 0) {
    sprintf(sprname, "%schocho.spr", MOB_DIR);
  } else if (strcmp(fname, DESERT_WOLF_B_PET) == 0) {
    sprintf(sprname, "%sdesert_wolf_b.spr", MOB_DIR);
  } else if (strcmp(fname, DEVIRUCHI_PET) == 0) {
    sprintf(sprname, "%sdeviruchi.spr", MOB_DIR);
  } else if (strcmp(fname, DOKEBI_PET) == 0) {
    sprintf(sprname, "%sdokebi.spr", MOB_DIR);
  } else if (strcmp(fname, ISIS_PET) == 0) {
    sprintf(sprname, "%sisis.spr", MOB_DIR);
  } else if (strcmp(fname, LUNATIC_PET) == 0) {
    sprintf(sprname, "%slunatic.spr", MOB_DIR);
  } else if (strcmp(fname, MUNAK_PET) == 0) {
    sprintf(sprname, "%smunak.spr", MOB_DIR);
  } else if (strcmp(fname, ORK_WARRIOR_PET) == 0) {
    sprintf(sprname, "%sork_warrior.spr", MOB_DIR);
  } else if (strcmp(fname, PECOPECO_PET) == 0) {
    sprintf(sprname, "%specopeco.spr", MOB_DIR);
  } else if (strcmp(fname, PETIT_PET) == 0) {
    sprintf(sprname, "%spetit.spr", MOB_DIR);
  } else if (strcmp(fname, PICKY_PET) == 0) {
    sprintf(sprname, "%spicky.spr", MOB_DIR);
  } else if (strcmp(fname, PORING_PET) == 0) {
    sprintf(sprname, "%sporing.spr", MOB_DIR);
  } else if (strcmp(fname, ROCKER_PET) == 0) {
    sprintf(sprname, "%srocker.spr", MOB_DIR);
  } else if (strcmp(fname, SAVAGE_BABE_PET) == 0) {
    sprintf(sprname, "%ssavage_babe.spr", MOB_DIR);
  } else if (strcmp(fname, SMOKIE_PET) == 0) {
    sprintf(sprname, "%ssmokie.spr", MOB_DIR);
  } else if (strcmp(fname, SOHEE_PET) == 0) {
    sprintf(sprname, "%ssohee.spr", MOB_DIR);
  } else if (strcmp(fname, SPORE_PET) == 0) {
    sprintf(sprname, "%sspore.spr", MOB_DIR);
  } else if (strcmp(fname, YOYO_PET) == 0) {
    sprintf(sprname, "%syoyo.spr", MOB_DIR);
  } else {
    sprintf(sprname, "%s%s.spr", MOB_DIR, fname);
  }
}

void CChrView::GetItemNoToWeaponType(char *fname, int no)
{
  int num;
  num = sizeof(weaponid2type) / sizeof(int) / 2;

  for (int i=0; i<num; i++) {
    if (weaponid2type[i][0] == no) {
      switch (weaponid2type[i][1]) {
      case 0:  // short sword
        strcat(fname, SHORTSWORD_AFTIMG_STR);
        break;
      case 1:  // one hand sword
      case 2:  // two hand sword
        strcat(fname, SWORD_AFTIMG_STR);
        break;
      case 3:  // axe
        strcat(fname, AXE_AFTIMG_STR);
        break;
      case 4:  // spear
        strcat(fname, SPEAR_AFTIMG_STR);
        break;
      case 5:  // rod
        strcat(fname, ROD_AFTIMG_STR);
        break;
      case 6:  // crub
        strcat(fname, CRUB_AFTIMG_STR);
        break;
      case 7:  // dummy case
      case 8:  // claw
        strcat(fname, CLAW_AFTIMG_STR);
        break;
      }
      break;
    }
  }

  return;
}

void CChrView::GetPCBodyName(char *bodyname, char *sexname, char *dispname, bool male)
{
  char *fname_base;

  if (male) {
    fname_base = GetNFDFnameFromDname(&NFDPCBodyM, dispname);
    strcpy(sexname, MALE_STR);
  } else {
    fname_base = GetNFDFnameFromDname(&NFDPCBodyF, dispname);
    strcpy(sexname, FEMALE_STR);
  }
  if (fname_base) {
    strcpy(bodyname, fname_base);
    if (bodyname[strlen(bodyname)-3] == '_')  bodyname[strlen(bodyname)-3] = '\0';
  } else {
    bodyname[0] = '\0';
  }
}

int CChrView::GetTimeInterval(void)
{
  int timeInterval = -1;
  if (sprite_Body) {
    timeInterval = (int)sprite_Body->GetInterval(curAct);
  } else if (sprite_Head) {
    timeInterval = (int)sprite_Head->GetInterval(curAct);
  } else if (sprite_Acc1) {
    timeInterval = (int)sprite_Acc1->GetInterval(curAct);
  } else if (sprite_Acc2) {
    timeInterval = (int)sprite_Acc2->GetInterval(curAct);
  } else if (sprite_Acc3) {
    timeInterval = (int)sprite_Acc3->GetInterval(curAct);
  } else if (sprite_Weapon) {
    timeInterval = (int)sprite_Weapon->GetInterval(curAct);
  } else if (sprite_WpAftimg) {
    timeInterval = (int)sprite_WpAftimg->GetInterval(curAct);
  } else if (sprite_Shield) {
    timeInterval = (int)sprite_Shield->GetInterval(curAct);
  }
  if (timeInterval <= 0) timeInterval = INT_MAX;
  return timeInterval;
}

int NAME_FvsDSortFuncByDisp(const void *a, const void *b)
{
  char *da, *db;
  da = ((NAME_FvsD *)a)->dispname;
  db = ((NAME_FvsD *)b)->dispname;
  return strcmp(da, db);
}

