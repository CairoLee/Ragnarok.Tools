#include "chrview_gtk.h"

#include <vector>
#include <string>
using namespace std;

CChrView *cv1;

extern CPAK *pak1;

GtkWidget *windowChr;
GSList *rbtnTYPE_group = NULL;
GtkWidget *rbtnPC;
GtkWidget *rbtnMob;
GtkWidget *rbtnNPC;
GtkWidget *btnBack;
GtkWidget *btnViewReset;
GtkWidget *btnToEasy;
GtkWidget *btnCloseChr;
GSList *rbtnSEX_group = NULL;
GtkWidget *labelSex;
GtkWidget *rbtnMale;
GtkWidget *rbtnFemale;
GtkWidget *comboBody;
GtkWidget *comboHead;
GtkWidget *comboAcc1;
GtkWidget *comboAcc2;
GtkWidget *comboAcc3;
GdkGLConfig *glconfig;
GtkWidget *oglarea;
GtkWidget *comboShield;
GtkWidget *comboWeapon;
GtkWidget *comboAction;
GtkWidget *btnNW;
GtkWidget *btnN;
GtkWidget *btnNE;
GtkWidget *btnW;
GtkWidget *btnE;
GtkWidget *btnSW;
GtkWidget *btnS;
GtkWidget *btnSE;
GtkWidget *btnPlay;
GtkWidget *btnStop;
GtkWidget *hscr;
GtkWidget *labelFrame;

guint idTimer = 0;
int returnCV;

int CharacterViewer(int argc, char **argv)
{
  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);

  cv1 = new CChrView(pak1);

  glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_ALPHA | GDK_GL_MODE_DOUBLE));
  if (glconfig == NULL) {
      g_print("failed: OpenGL alpha blend\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE));
    if (glconfig == NULL) {
      g_print("failed: OpenGL double buffer\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA));
    }
  }

  cv1->colBG = 0x00C0C0C0;
  ChrVBuildWindow();
  ChrVSignalConnect();
  ChrVInitControls();
  gtk_widget_show_all(GTK_WIDGET(windowChr));
  gtk_window_resize(GTK_WINDOW(windowChr), 600, 400);
  gtk_window_set_resizable(GTK_WINDOW(windowChr), FALSE);
  gtk_main();

  ExitCharacterViewer();

  return returnCV;
}

void ExitCharacterViewer(void)
{
  cv1->sprready = false;
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }
  gtk_widget_destroy(windowChr);
  rbtnTYPE_group = NULL;
  rbtnSEX_group = NULL;

  delete cv1;
}

void ChrVInitNFD(void)
{
  cv1->InitNFD();
  ChrVInitControlsWeaponShieldPre();
}

void ChrVGLZoom(void)
{
  cv1->GLZoom();
  ChrVDrawSprites();
}

void ChrVGLPan(void)
{
  cv1->GLPan();
  ChrVDrawSprites();
}

bool ChrVSpriteSetPCBody(CSprite **sprite, char *dispname)
{
  char *fname_base = NULL;
  bool ret;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    ret = cv1->SpriteSetPCBody(sprite, dispname, fname_base, true);
  } else {
    ret = cv1->SpriteSetPCBody(sprite, dispname, fname_base, false);
  }
  if (ret == false)  return false;

  if (fname_base) {
    if (strstr(fname_base, "\277\356\277\265\300\332\62")) {  // GM2
      ChrVEnableComboAlmost(false);
      cv1->DestroySprites_GM2();
    } else {
      ChrVEnableComboAlmost(true);
    }
  }

  return true;
}

bool ChrVSpriteSetPCHead(CSprite **sprite)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    return cv1->SpriteSetPCHead(sprite, true);
  } else {
    return cv1->SpriteSetPCHead(sprite, false);
  }
}

bool ChrVSpriteSetPCAcc(CSprite **sprite, char *dispname, int no)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    return cv1->SpriteSetPCAcc(sprite, dispname, no, true);
  } else {
    return cv1->SpriteSetPCAcc(sprite, dispname, no, false);
  }
}

bool ChrVSpriteSetPCWeapon(CSprite **sprite)
{
  char sexname[3], bodyname[100];
  ChrVGetPCBodyName(bodyname, sexname);

  char dispname[100];
  strncpy(dispname, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboWeapon)->entry)), sizeof(dispname));

  return cv1->SpriteSetPCWeapon(sprite, dispname, bodyname, sexname);
}

bool ChrVSpriteSetPCShield(CSprite **sprite)
{
  char sexname[3], bodyname[100];
  ChrVGetPCBodyName(bodyname, sexname);

  char dispname[100];
  strncpy(dispname, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboShield)->entry)), sizeof(dispname));

  return cv1->SpriteSetPCShield(sprite, bodyname, sexname, dispname);
}

void ChrVDrawSprites(void)
{
  char buf[20];
  sprintf(buf, "%3d / %3d", cv1->curPat+1, cv1->numPat);
  gtk_label_set_text(GTK_LABEL(labelFrame), buf);
  gtk_widget_queue_draw(GTK_WIDGET(oglarea));
}

void ChrVSetScrollBar(int min, int max, int pos)
{
  //GtkObject *adjust;
  GTK_ADJUSTMENT(GTK_RANGE(hscr)->adjustment)->lower = min-1;
  GTK_ADJUSTMENT(GTK_RANGE(hscr)->adjustment)->upper = max;
  GTK_ADJUSTMENT(GTK_RANGE(hscr)->adjustment)->value = pos-1;
  GTK_ADJUSTMENT(GTK_RANGE(hscr)->adjustment)->page_size = 1;//(max-min)/5;
  //adjust = gtk_adjustment_new(pos, min, max, 1, 5, (max-min)/5+1);

  gtk_adjustment_value_changed(GTK_ADJUSTMENT(GTK_RANGE(hscr)->adjustment));
}

void ChrVInitControls(void)
{
  cv1->SetNullSprites();
  ChrVInitNFD();
  ChrVInitControlsHead();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnPC), TRUE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnMale), TRUE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btnPlay), TRUE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btnStop), FALSE);
  gtk_label_set_text(GTK_LABEL(labelFrame), "0 / 0");
  ChrVInitControlsSub();
}

void ChrVInitControlsSub(void)
{
  cv1->sprready = false;
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }
  cv1->SetCameraDefault();
  cv1->DestroySpriteAll();
  cv1->curAcc1 = cv1->curAcc2 = cv1->curAcc3 = 0;
  gboolean f = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnPC));
  if (f == (gboolean)TRUE) {
    gtk_widget_set_sensitive(labelSex, true);
    gtk_widget_set_sensitive(rbtnMale, true);
    gtk_widget_set_sensitive(rbtnFemale, true);
  } else {
    gtk_widget_set_sensitive(labelSex, false);
    gtk_widget_set_sensitive(rbtnMale, false);
    gtk_widget_set_sensitive(rbtnFemale, false);
  }

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboBody)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboBody)->list), (GtkCallback)RemoveComboItems, NULL);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAcc1)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAcc1)->list), (GtkCallback)RemoveComboItems, NULL);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAcc2)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAcc2)->list), (GtkCallback)RemoveComboItems, NULL);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAcc3)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAcc3)->list), (GtkCallback)RemoveComboItems, NULL);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboShield)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboShield)->list), (GtkCallback)RemoveComboItems, NULL);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboWeapon)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboWeapon)->list), (GtkCallback)RemoveComboItems, NULL);
  ChrVInitControlsBody();
  ChrVInitControlsAccPC();
}

void ChrVInitControlsHead(void)
{
  char *fnp, *fnp2;
  int n = 1;
  char buf[5];

  GtkWidget *list_item;
  list_item = gtk_list_item_new_with_label("nothing");
  gtk_object_set_user_data(GTK_OBJECT(list_item), (gpointer)0);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboHead)->list), list_item);
  gtk_widget_show(list_item);
  for (int i=0; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    fnp = pak1->Entries[i].fn;
    fnp2 = &fnp[strlen(fnp)-3];
    if (STRCASECMP(fnp2, "act") != 0)  continue;
    if (strstr(fnp, PCHEAD_M_DIR) != 0) {
      sprintf(buf, "%d", n);
      list_item = gtk_list_item_new_with_label(buf);
      gtk_object_set_user_data(GTK_OBJECT(list_item), (gpointer)(n));
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboHead)->list), list_item);
      gtk_widget_show(list_item);
      n++;
    }
  }
  cv1->curHead = 0;
}

void ChrVInitControlsBody(void)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnPC))) {
    ChrVInitControlsBodyPC();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMob))) {
    ChrVInitControlsBodyMob();
  } else {
    ChrVInitControlsBodyNPC();
  }
  gtk_widget_queue_draw(comboBody);
}

void ChrVInitControlsBodyPC(void)
{
  GtkWidget *list_item;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    for (int i=0; i<cv1->NFDPCBodyM.num; i++) {
      list_item = gtk_list_item_new_with_label((cv1->NFDPCBodyM.name + i)->dispname);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboBody)->list), list_item);
      gtk_widget_show(list_item);
    }
  } else {
    for (int i=0; i<cv1->NFDPCBodyF.num; i++) {
      list_item = gtk_list_item_new_with_label((cv1->NFDPCBodyF.name + i)->dispname);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboBody)->list), list_item);
      gtk_widget_show(list_item);
    }
  }
}

void ChrVInitControlsBodyMob(void)
{
  GtkWidget *list_item;
  for (int i=0; i<cv1->NFDMob.num; i++) {
    list_item = gtk_list_item_new_with_label((cv1->NFDMob.name + i)->dispname);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboBody)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVInitControlsBodyNPC(void)
{
  GtkWidget *list_item;
  for (int i=0; i<cv1->NFDNPC.num; i++) {
    list_item = gtk_list_item_new_with_label((cv1->NFDNPC.name + i)->dispname);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboBody)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVInitControlsAccPC(void)
{
  GtkWidget *list_item1, *list_item2, *list_item3;
  list_item1 = gtk_list_item_new_with_label("nothing");
  list_item2 = gtk_list_item_new_with_label("nothing");
  list_item3 = gtk_list_item_new_with_label("nothing");
  gtk_object_set_user_data(GTK_OBJECT(list_item1), (gpointer)0);
  gtk_object_set_user_data(GTK_OBJECT(list_item2), (gpointer)0);
  gtk_object_set_user_data(GTK_OBJECT(list_item3), (gpointer)0);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc1)->list), list_item1);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc2)->list), list_item2);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc3)->list), list_item3);
  gtk_widget_show(list_item1);
  gtk_widget_show(list_item2);
  gtk_widget_show(list_item3);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    for (int i=0; i<cv1->NFDPCAccM.num; i++) {
      list_item1 = gtk_list_item_new_with_label((cv1->NFDPCAccM.name + i)->dispname);
      list_item2 = gtk_list_item_new_with_label((cv1->NFDPCAccM.name + i)->dispname);
      list_item3 = gtk_list_item_new_with_label((cv1->NFDPCAccM.name + i)->dispname);
      gtk_object_set_user_data(GTK_OBJECT(list_item1), (gpointer)(i+1));
      gtk_object_set_user_data(GTK_OBJECT(list_item2), (gpointer)(i+1));
      gtk_object_set_user_data(GTK_OBJECT(list_item3), (gpointer)(i+1));
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc1)->list), list_item1);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc2)->list), list_item2);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc3)->list), list_item3);
      gtk_widget_show(list_item1);
      gtk_widget_show(list_item2);
      gtk_widget_show(list_item3);
    }
  } else {
    for (int i=0; i<cv1->NFDPCAccF.num; i++) {
      list_item1 = gtk_list_item_new_with_label((cv1->NFDPCAccF.name + i)->dispname);
      list_item2 = gtk_list_item_new_with_label((cv1->NFDPCAccF.name + i)->dispname);
      list_item3 = gtk_list_item_new_with_label((cv1->NFDPCAccF.name + i)->dispname);
      gtk_object_set_user_data(GTK_OBJECT(list_item1), (gpointer)(i+1));
      gtk_object_set_user_data(GTK_OBJECT(list_item2), (gpointer)(i+1));
      gtk_object_set_user_data(GTK_OBJECT(list_item3), (gpointer)(i+1));
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc1)->list), list_item1);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc2)->list), list_item2);
      gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAcc3)->list), list_item3);
      gtk_widget_show(list_item1);
      gtk_widget_show(list_item2);
      gtk_widget_show(list_item3);
    }
  }
}

void ChrVInitControlsActPC(void)
{
  char actname[50], actnamed[50];
  int n;

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAction)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAction)->list), (GtkCallback)RemoveComboItems, NULL);

  n = cv1->numAct/8;
  if (n == 0) n = 1;

  GtkWidget *list_item;
  for (int i=0; i<n; i++) {
    sprintf(actname, PC_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
#ifdef DISTRIBUTE
    list_item = gtk_list_item_new_with_label(actnamed);
#else
    PO_STR_SET(actname, actnamed);
    list_item = gtk_list_item_new_with_label(actname);
#endif
    gtk_object_set_user_data(GTK_OBJECT(list_item), (gpointer)i);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAction)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVInitControlsActMob(void)
{
  char actname[50], actnamed[50];
  int n;

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAction)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAction)->list), (GtkCallback)RemoveComboItems, NULL);

  n = cv1->numAct/8;
  if (n == 0) n = 1;
  GtkWidget *list_item;
  for (int i=0; i<n; i++) {
    sprintf(actname, MOB_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
    list_item = gtk_list_item_new_with_label(actnamed);
    gtk_object_set_user_data(GTK_OBJECT(list_item), (gpointer)i);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAction)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVInitControlsActNPC(void)
{
  char actname[50], actnamed[50];
  int n;

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(comboAction)->entry), "");
  gtk_container_foreach(GTK_CONTAINER(GTK_COMBO(comboAction)->list), (GtkCallback)RemoveComboItems, NULL);

  n = cv1->numAct/8;
  if (n == 0) n = 1;
  GtkWidget *list_item;
  for (int i=0; i<n; i++) {
    sprintf(actname, MOB_ACTNAME_INT, i);
    strncpy(actnamed, _(actname), sizeof(actnamed));
    list_item = gtk_list_item_new_with_label(actnamed);
    gtk_object_set_user_data(GTK_OBJECT(list_item), (gpointer)i);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboAction)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVInitControlsWeaponShieldPre(void)
{
  cv1->InitControlsWeaponShieldPre();

  GList *items = NULL;
  items = g_list_append(items, (void *)" nothing");
  gtk_combo_set_popdown_strings(GTK_COMBO(comboShield), items);
  gtk_combo_set_popdown_strings(GTK_COMBO(comboWeapon), items);
}

void ChrVInitControlsWeaponShield(void)
{
  char *fnp, *fnp2;
  char bodynameW[100], bodynameS[100], bodyname2[50], sexname[3];

  ChrVGetPCBodyName(bodyname2, sexname);
  sprintf(bodynameW, "%s%s\\", WEAPON_DIR, bodyname2);
  sprintf(bodynameS, "%s%s\\", SHIELD_DIR, bodyname2);

  for (int i=0; i<pak1->NumEntry; i++) {
    if (pak1->Entries[i].type == 2)  continue;
    fnp = pak1->Entries[i].fn;
    if (strstr(fnp, "act") == 0)  continue;
    if (strstr(fnp, bodynameW)) {
      fnp2 = &fnp[strlen(bodynameW)];
      if (strstr(fnp2, bodyname2) == 0) continue;
      if (strstr(fnp2, sexname) == 0) continue;
      cv1->SetWeaponName(fnp, bodynameW, bodyname2);
    } else if (strstr(fnp, bodynameS)) {
      fnp2 = &fnp[strlen(bodynameS)];
      if (strstr(fnp2, bodyname2) == 0) continue;
      if (strstr(fnp2, sexname) == 0) continue;
      cv1->SetShieldName(fnp, bodynameS, bodyname2);
    }
  }
  qsort(cv1->NFDPCWeapon.name, cv1->NFDPCWeapon.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);
  qsort(cv1->NFDPCShield.name, cv1->NFDPCShield.num, sizeof(NAME_FvsD), NAME_FvsDSortFuncByDisp);

  GtkWidget *list_item;
  for (int i=0; i<cv1->NFDPCWeapon.num; i++) {
    list_item = gtk_list_item_new_with_label((cv1->NFDPCWeapon.name + i)->dispname);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboWeapon)->list), list_item);
    gtk_widget_show(list_item);
  }
  for (int i=0; i<cv1->NFDPCShield.num; i++) {
    list_item = gtk_list_item_new_with_label((cv1->NFDPCShield.name + i)->dispname);
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(comboShield)->list), list_item);
    gtk_widget_show(list_item);
  }
}

void ChrVSelectComboBody(GtkEntry *entry, gpointer user_data)
{
  char dispname[FILENAME_MAX];
  strncpy(dispname, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboBody)->entry)), sizeof(dispname));
  if (dispname[0] == '\0')  return;

  cv1->sprready = false;
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnPC))) {
    ChrVSelectComboBody_PC();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMob))) {
    if (cv1->SpriteSetMob(&cv1->sprite_Body, dispname) == false)  return;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnNPC))) {
    if (cv1->SpriteSetNPC(&cv1->sprite_Body, dispname) == false)  return;
  }

  cv1->InitNumActPat();
  if (cv1->numAct == 0) {
    cv1->sprready = false;
    return;
  }
  ChrVSetScrollBar(1, cv1->numPat, 1);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnPC))) {
    ChrVInitControlsActPC();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMob))) {
    ChrVInitControlsActMob();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnNPC))) {
    ChrVInitControlsActNPC();
  }
  gtk_widget_queue_draw(comboAction);
  cv1->sprready = true;
  ChrVDrawSprites();
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnPlay))) {
    idTimer = gtk_timeout_add(cv1->GetTimeInterval(), ChrVTimerProc, NULL);
  }
}

void ChrVSelectComboBody_PC(void)
{
  char dispnameA[FILENAME_MAX], dispnameB[FILENAME_MAX];

  strncpy(dispnameB, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboBody)->entry)), sizeof(dispnameB));
  if (ChrVSpriteSetPCHead(&cv1->sprite_Head) == false)  return;
  if (ChrVSpriteSetPCBody(&cv1->sprite_Body, dispnameB) == false)  return;

  strncpy(dispnameA, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc1)->entry)), sizeof(dispnameA));
  if (ChrVSpriteSetPCAcc(&cv1->sprite_Acc1, dispnameA, 1) == false)  return;
  strncpy(dispnameA, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc2)->entry)), sizeof(dispnameA));
  if (ChrVSpriteSetPCAcc(&cv1->sprite_Acc2, dispnameA, 2) == false)  return;
  strncpy(dispnameA, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc3)->entry)), sizeof(dispnameA));
  if (ChrVSpriteSetPCAcc(&cv1->sprite_Acc3, dispnameA, 3) == false)  return;

  cv1->DestroySprite(&cv1->sprite_Weapon);
  cv1->DestroySprite(&cv1->sprite_WpAftimg);
  cv1->DestroySprite(&cv1->sprite_Shield);
  ChrVInitControlsWeaponShieldPre();
  cv1->DestroyNFD_WeapShield();
  ChrVInitControlsWeaponShield();
}

void ChrVSelectComboAct(GtkEntry *entry, gpointer user_data)
{
  char nameact[20];
  strncpy(nameact, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAction)->entry)), sizeof(nameact));
  if (nameact[0] == '\0')  return;

  if (!cv1->sprready) return;
  bool tmpPlay = false;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnPlay))) {
    tmpPlay = true;
  }
  if (tmpPlay) {
    ChrVButtonStop(NULL, NULL);
  }

  GList *items = GTK_LIST(GTK_COMBO(comboAction)->list)->selection;
  int idAct = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
  cv1->SelectAct(idAct);
  ChrVSetScrollBar(1, cv1->numPat, 1);
  ChrVDrawSprites();

  if (tmpPlay) {
    ChrVButtonPlay(NULL, NULL);
  }
}

void ChrVSelectComboHead(GtkEntry *entry, gpointer user_data)
{
  char namehead[10];
  strncpy(namehead, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboHead)->entry)), sizeof(namehead));
  if (namehead[0] == '\0')  return;
  GList *items = GTK_LIST(GTK_COMBO(comboHead)->list)->selection;
  cv1->curHead = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
  ChrVSelectComboBody(NULL, NULL);
}

void ChrVSelectComboAcc(GtkEntry *entry, gpointer user_data)
{
  char name[50];
  GList *items;
  switch ((int)user_data) {
  case 1:
    strncpy(name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc1)->entry)), sizeof(name));
    if (name[0] == '\0')  return;
    items = GTK_LIST(GTK_COMBO(comboAcc1)->list)->selection;
    if (items) {
      cv1->curAcc1 = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
    }
    break;
  case 2:
    strncpy(name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc2)->entry)), sizeof(name));
    if (name[0] == '\0')  return;
    items = GTK_LIST(GTK_COMBO(comboAcc2)->list)->selection;
    if (items) {
      cv1->curAcc2 = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
    }
    break;
  case 3:
    strncpy(name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboAcc3)->entry)), sizeof(name));
    if (name[0] == '\0')  return;
    items = GTK_LIST(GTK_COMBO(comboAcc3)->list)->selection;
    if (items) {
      cv1->curAcc3 = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
    }
    break;
  }
  ChrVSelectComboBody(NULL, NULL);
}

void ChrVSelectComboWeapon(GtkEntry *entry, gpointer user_data)
{
  char name[50];
  strncpy(name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboWeapon)->entry)), sizeof(name));
  if (name[0] == '\0')  return;
  ChrVSpriteSetPCWeapon(&cv1->sprite_Weapon);
}

void ChrVSelectComboShield(GtkEntry *entry, gpointer user_data)
{
  char name[50];
  strncpy(name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboShield)->entry)), sizeof(name));
  if (name[0] == '\0')  return;
  ChrVSpriteSetPCShield(&cv1->sprite_Shield);
}

void ChrVButtonPC(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnPC))) {
    ChrVEnableComboAlmost(true);
    ChrVInitControlsSub();
  }
}

void ChrVButtonMob(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMob))) {
    ChrVEnableComboAlmost(false);
    ChrVInitControlsSub();
  }
}

void ChrVButtonNPC(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnNPC))) {
    ChrVEnableComboAlmost(false);
    ChrVInitControlsSub();
  }
}

void ChrVButtonMale(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    ChrVInitControlsSub();
  }
}

void ChrVButtonFemale(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnFemale))) {
    ChrVInitControlsSub();
  }
}

void ChrVButtonPlay(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnStop))) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btnStop), FALSE);
  }
  if (idTimer == 0) {
    idTimer = gtk_timeout_add(cv1->GetTimeInterval(), ChrVTimerProc, NULL);
  }
}

void ChrVButtonStop(GtkButton *button, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnPlay))) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btnPlay), FALSE);
  }
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }
}

void ChrVButtonViewReset(GtkButton *button, gpointer user_data)
{
  cv1->SetCameraDefault();
  ChrVDrawSprites();
}

void ChrVButtonDirection(GtkButton *button, gpointer user_data)
{
  if (!cv1->sprready) return;
  bool tmpPlay = false;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnPlay))) {
    tmpPlay = true;
  }
  if (tmpPlay) {
    ChrVButtonStop(NULL, NULL);
  }

  GList *items = GTK_LIST(GTK_COMBO(comboAction)->list)->selection;
  int idAct = (int)gtk_object_get_user_data(GTK_OBJECT(items->data));
  cv1->SelectDirection(idAct, (int)user_data);
  ChrVSetScrollBar(1, cv1->numPat, 1);
  ChrVDrawSprites();

  if (tmpPlay) {
    ChrVButtonPlay(NULL, NULL);
  }
}

void ChrVScrAdjustBounds(GtkRange *range, gdouble arg1, gpointer user_data)
{
  cv1->curPat = (int)arg1;
  if (cv1->curPat >= cv1->numPat)  cv1->curPat = 0;
  if (cv1->curPat < 0)  cv1->curPat = cv1->numPat-1;
}

gboolean ChrVScrRelease(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ChrVSetScrollBar(1, cv1->numPat, cv1->curPat+1);
  ChrVDrawSprites();
  return FALSE;
}

void ChrVEnableComboAlmost(bool flag)
{
  gtk_widget_set_sensitive(GTK_WIDGET(comboHead), flag);
  gtk_widget_set_sensitive(GTK_WIDGET(comboAcc1), flag);
  gtk_widget_set_sensitive(GTK_WIDGET(comboAcc2), flag);
  gtk_widget_set_sensitive(GTK_WIDGET(comboAcc3), flag);
  gtk_widget_set_sensitive(GTK_WIDGET(comboShield), flag);
  gtk_widget_set_sensitive(GTK_WIDGET(comboWeapon), flag);
}

void ChrVGetPCBodyName(char *bodyname, char *sexname)
{
  char dispname[100];
  strncpy(dispname, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboBody)->entry)), sizeof(dispname));
  if (dispname[0] == '\0')  return;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnMale))) {
    cv1->GetPCBodyName(bodyname, sexname, dispname, true);
  } else {
    cv1->GetPCBodyName(bodyname, sexname, dispname, false);
  }
}

void ChrVSetBGColor(GtkButton *button, gpointer user_data)
{
  GtkWidget *csdialog;
  csdialog = gtk_color_selection_dialog_new(NULL);
  BYTE r, g, b;
  GdkColor color;
  r = GETCOL_R(cv1->colBG);
  g = GETCOL_G(cv1->colBG);
  b = GETCOL_B(cv1->colBG);
  color.red = r << 8;
  color.green = g << 8;
  color.blue = b << 8;
  gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);

  if (gtk_dialog_run(GTK_DIALOG(csdialog)) != GTK_RESPONSE_OK) return;

  gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  r = color.red >> 8;
  g = color.green >> 8;
  b = color.blue >> 8;
  cv1->colBG = b << 16 | g << 8 | r;
  cv1->InitOpenGL();
  ChrVDrawSprites();
  gtk_widget_destroy(csdialog);
}

gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  cv1->InitOpenGL();
  cv1->SetCameraDefault();
  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);

  return TRUE;
}

gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }

  if (cv1->sprready) {
    cv1->DrawSprites();
  }

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }

  gdk_gl_drawable_gl_end (gldrawable);

  return TRUE;
}

gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  cv1->new_mp.x = (int)event->x;
  cv1->new_mp.y = (int)event->y;
  if (cv1->inMDragging) {
    ChrVGLZoom();
  } else if (cv1->inRDragging) {
    ChrVGLPan();
  }
  cv1->old_mp.x = cv1->new_mp.x;
  cv1->old_mp.y = cv1->new_mp.y;

  return TRUE;
}

gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (event->type == GDK_BUTTON_PRESS) {
    cv1->first_mp.x = cv1->old_mp.x = (int)event->x;
    cv1->first_mp.y = cv1->old_mp.y = (int)event->y;
  }
  switch (event->button) {
  case 2: // middle
    cv1->inMDragging = true;
    break;
  case 3: // right
    cv1->inRDragging = true;
    break;
  }
  return TRUE;
}

gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  switch (event->button) {
  case 2: // middle
    cv1->inMDragging = false;
    break;
  case 3: // right
    cv1->inRDragging = false;
    break;
  }
  return TRUE;
}

gint ChrVCloseDialog(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_main_quit();

  returnCV = 0;

  return TRUE;
}

void ChrVCloseDialogBtn(GtkButton *button, gpointer data)
{
  gtk_main_quit();

  returnCV = (int)data;
}

gboolean ChrVTimerProc(gpointer data)
{
  if (cv1->sprready) {
    cv1->curPat++;
    if (cv1->curPat == cv1->numPat)  cv1->curPat = 0;
    ChrVSetScrollBar(1, cv1->numPat, cv1->curPat+1);
    ChrVDrawSprites();
  }
  return TRUE;
}

void ChrVSignalConnect(void)
{
  gtk_signal_connect(GTK_OBJECT(windowChr), "delete_event", GTK_SIGNAL_FUNC(ChrVCloseDialog), NULL);
  gtk_signal_connect(GTK_OBJECT(btnCloseChr), "clicked", GTK_SIGNAL_FUNC(ChrVCloseDialogBtn), (gpointer)0);
  gtk_signal_connect(GTK_OBJECT(btnToEasy), "clicked", GTK_SIGNAL_FUNC(ChrVCloseDialogBtn), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboBody)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboBody), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboHead)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboHead), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboAcc1)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboAcc), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboAcc2)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboAcc), (gpointer)2);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboAcc3)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboAcc), (gpointer)3);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboWeapon)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboShield), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboShield)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboWeapon), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(comboAction)->entry), "changed", GTK_SIGNAL_FUNC(ChrVSelectComboAct), NULL);
  gtk_signal_connect(GTK_OBJECT(rbtnPC), "clicked", GTK_SIGNAL_FUNC(ChrVButtonPC), NULL);
  gtk_signal_connect(GTK_OBJECT(rbtnMob), "clicked", GTK_SIGNAL_FUNC(ChrVButtonMob), NULL);
  gtk_signal_connect(GTK_OBJECT(rbtnNPC), "clicked", GTK_SIGNAL_FUNC(ChrVButtonNPC), NULL);
  gtk_signal_connect(GTK_OBJECT(rbtnMale), "clicked", GTK_SIGNAL_FUNC(ChrVButtonMale), NULL);
  gtk_signal_connect(GTK_OBJECT(rbtnFemale), "clicked", GTK_SIGNAL_FUNC(ChrVButtonFemale), NULL);
  gtk_signal_connect(GTK_OBJECT(btnPlay), "clicked", GTK_SIGNAL_FUNC(ChrVButtonPlay), NULL);
  gtk_signal_connect(GTK_OBJECT(btnStop), "clicked", GTK_SIGNAL_FUNC(ChrVButtonStop), NULL);
  gtk_signal_connect(GTK_OBJECT(btnBack), "clicked", GTK_SIGNAL_FUNC(ChrVSetBGColor), NULL);
  gtk_signal_connect(GTK_OBJECT(btnViewReset), "clicked", GTK_SIGNAL_FUNC(ChrVButtonViewReset), NULL);
  gtk_signal_connect(GTK_OBJECT(btnS), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)0);
  gtk_signal_connect(GTK_OBJECT(btnSW), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(btnW), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)2);
  gtk_signal_connect(GTK_OBJECT(btnNW), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)3);
  gtk_signal_connect(GTK_OBJECT(btnN), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)4);
  gtk_signal_connect(GTK_OBJECT(btnNE), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)5);
  gtk_signal_connect(GTK_OBJECT(btnE), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)6);
  gtk_signal_connect(GTK_OBJECT(btnSE), "clicked", GTK_SIGNAL_FUNC(ChrVButtonDirection), (gpointer)7);
  gtk_signal_connect(GTK_OBJECT(GTK_RANGE(hscr)), "adjust-bounds", GTK_SIGNAL_FUNC(ChrVScrAdjustBounds), NULL);
  gtk_signal_connect(GTK_OBJECT(hscr), "button-release-event", GTK_SIGNAL_FUNC(ChrVScrRelease), NULL);
  gtk_widget_add_events(oglarea, GDK_BUTTON2_MOTION_MASK | GDK_BUTTON3_MOTION_MASK
                       | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_VISIBILITY_NOTIFY_MASK);
  g_signal_connect(G_OBJECT(oglarea), "configure_event", G_CALLBACK(configure_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "expose_event", G_CALLBACK(expose_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "motion_notify_event", G_CALLBACK(motion_notify_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "button_press_event", G_CALLBACK (button_press_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "button_release_event", G_CALLBACK (button_release_event), NULL);
}

void ChrVBuildWindow(void)
{
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;
  GtkWidget *hbox10;
  GtkWidget *hbox11;
  GtkWidget *hbox2;
  GtkWidget *hbox12;
  GtkWidget *hbox7;
  GtkWidget *hbox8;
  GtkWidget *hbox9;
  GtkWidget *hbox6;
  GtkWidget *label14;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label13;
  GtkWidget *frame3;
  GtkWidget *frame2;
  GtkWidget *alignment1;
  GtkWidget *alignment2;
  GtkWidget *alignment3;
  GtkWidget *alignment4;

  windowChr = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(windowChr), vbox1);

  hbox10 = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox10, TRUE, TRUE, 0);

  frame3 = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(hbox10), frame3, TRUE, TRUE, 0);

  hbox11 = gtk_hbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frame3), hbox11);

  rbtnPC = gtk_radio_button_new_with_mnemonic(NULL, _("PC"));
  gtk_box_pack_start(GTK_BOX(hbox11), rbtnPC, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnPC), rbtnTYPE_group);
  rbtnTYPE_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnPC));

  rbtnMob = gtk_radio_button_new_with_mnemonic(NULL, _("Mob"));
  gtk_box_pack_start(GTK_BOX(hbox11), rbtnMob, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnMob), rbtnTYPE_group);
  rbtnTYPE_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnMob));

  rbtnNPC = gtk_radio_button_new_with_mnemonic(NULL, _("NPC"));
  gtk_box_pack_start(GTK_BOX(hbox11), rbtnNPC, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnNPC), rbtnTYPE_group);
  rbtnTYPE_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnNPC));

  label14 = gtk_label_new(_("type"));
  gtk_frame_set_label_widget(GTK_FRAME(frame3), label14);
  gtk_label_set_justify(GTK_LABEL(label14), GTK_JUSTIFY_LEFT);

  btnBack = gtk_button_new_with_label(_("Back"));
  gtk_box_pack_start(GTK_BOX(hbox10), btnBack, TRUE, TRUE, 0);

  alignment1 = gtk_alignment_new(1, 1, 0.26f, 0);
  gtk_box_pack_start (GTK_BOX(hbox10), alignment1, TRUE, TRUE, 0);

  btnViewReset = gtk_button_new_with_mnemonic(_("view reset"));
  gtk_container_add(GTK_CONTAINER(alignment1), btnViewReset);

  alignment3 = gtk_alignment_new(1, 0, 0.23f, 0.5f);
  gtk_box_pack_start(GTK_BOX(hbox10), alignment3, TRUE, TRUE, 0);

  btnToEasy = gtk_button_new_with_mnemonic(_("To EasyMode"));
  gtk_container_add(GTK_CONTAINER(alignment3), btnToEasy);

  alignment2 = gtk_alignment_new(1, 0, 0.23f, 0.5f);
  gtk_box_pack_start(GTK_BOX(hbox10), alignment2, TRUE, TRUE, 0);

  btnCloseChr = gtk_button_new_with_mnemonic(_("Close"));
  gtk_container_add(GTK_CONTAINER(alignment2), btnCloseChr);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new(FALSE, 3);
  gtk_box_pack_start(GTK_BOX(hbox2), vbox2, TRUE, TRUE, 0);

  alignment4 = gtk_alignment_new(0.5f, 0.5f, 1, 0.15f);
  gtk_box_pack_start(GTK_BOX(vbox2), alignment4, TRUE, TRUE, 0);

  frame2 = gtk_frame_new(NULL);
  gtk_container_add(GTK_CONTAINER(alignment4), frame2);

  hbox12 = gtk_hbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frame2), hbox12);

  rbtnMale = gtk_radio_button_new_with_mnemonic(NULL, _("male"));
  gtk_box_pack_start(GTK_BOX(hbox12), rbtnMale, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnMale), rbtnSEX_group);
  rbtnSEX_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnMale));

  rbtnFemale = gtk_radio_button_new_with_mnemonic(NULL, _("female"));
  gtk_box_pack_start(GTK_BOX(hbox12), rbtnFemale, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnFemale), rbtnSEX_group);
  rbtnSEX_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnFemale));

  labelSex = gtk_label_new(_("sex"));
  gtk_frame_set_label_widget(GTK_FRAME(frame2), labelSex);
  gtk_label_set_justify(GTK_LABEL(labelSex), GTK_JUSTIFY_LEFT);

  label3 = gtk_label_new(_("body"));
  gtk_box_pack_start(GTK_BOX(vbox2), label3, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_LEFT);

  comboBody = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox2), comboBody, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboBody)->entry), FALSE);

  label4 = gtk_label_new(_("head"));
  gtk_box_pack_start(GTK_BOX(vbox2), label4, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label4), GTK_JUSTIFY_LEFT);

  comboHead = gtk_combo_new ();
  gtk_box_pack_start(GTK_BOX(vbox2), comboHead, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboHead)->entry), FALSE);

  label5 = gtk_label_new(_("accessary1"));
  gtk_box_pack_start(GTK_BOX(vbox2), label5, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label5), GTK_JUSTIFY_LEFT);

  comboAcc1 = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox2), comboAcc1, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboAcc1)->entry), FALSE);

  label6 = gtk_label_new(_("accessary2"));
  gtk_box_pack_start(GTK_BOX(vbox2), label6, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label6), GTK_JUSTIFY_LEFT);

  comboAcc2 = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox2), comboAcc2, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboAcc2)->entry), FALSE);

  label7 = gtk_label_new(_("accessary3"));
  gtk_box_pack_start(GTK_BOX(vbox2), label7, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label7), GTK_JUSTIFY_LEFT);

  comboAcc3 = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox2), comboAcc3, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboAcc3)->entry), FALSE);

  oglarea = gtk_drawing_area_new();
  gtk_box_pack_start(GTK_BOX(hbox2), oglarea, TRUE, TRUE, 0);
  gtk_widget_set_size_request(oglarea, 300, 300);
  gtk_widget_set_gl_capability(oglarea,	glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);

  vbox3 = gtk_vbox_new(FALSE, 3);
  gtk_box_pack_start(GTK_BOX(hbox2), vbox3, TRUE, TRUE, 0);

  label8 = gtk_label_new(_("sheild"));
  gtk_box_pack_start(GTK_BOX(vbox3), label8, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label8), GTK_JUSTIFY_LEFT);

  comboShield = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox3), comboShield, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboShield)->entry), FALSE);

  label9 = gtk_label_new(_("weapon"));
  gtk_box_pack_start(GTK_BOX(vbox3), label9, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label9), GTK_JUSTIFY_LEFT);

  comboWeapon = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox3), comboWeapon, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboWeapon)->entry), FALSE);

  label10 = gtk_label_new(_("action"));
  gtk_box_pack_start(GTK_BOX(vbox3), label10, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label10), GTK_JUSTIFY_LEFT);

  comboAction = gtk_combo_new();
  gtk_box_pack_start(GTK_BOX(vbox3), comboAction, FALSE, FALSE, 0);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(comboAction)->entry), FALSE);

  label11 = gtk_label_new(_("direction"));
  gtk_box_pack_start(GTK_BOX(vbox3), label11, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label11), GTK_JUSTIFY_LEFT);

  hbox7 = gtk_hbox_new(TRUE, 3);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox7, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox7), 2);

  btnNW = gtk_button_new_with_mnemonic(_("\\"));
  gtk_box_pack_start(GTK_BOX(hbox7), btnNW, FALSE, TRUE, 0);

  btnN = gtk_button_new_with_mnemonic(_("|"));
  gtk_box_pack_start(GTK_BOX(hbox7), btnN, FALSE, TRUE, 0);

  btnNE = gtk_button_new_with_mnemonic(_("/"));
  gtk_box_pack_start(GTK_BOX (hbox7), btnNE, FALSE, TRUE, 0);

  hbox8 = gtk_hbox_new(TRUE, 3);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox8, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox8), 2);

  btnW = gtk_button_new_with_mnemonic(_("-"));
  gtk_box_pack_start(GTK_BOX(hbox8), btnW, FALSE, TRUE, 0);

  label13 = gtk_label_new(_("+"));
  gtk_box_pack_start(GTK_BOX(hbox8), label13, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label13), GTK_JUSTIFY_LEFT);

  btnE = gtk_button_new_with_mnemonic(_("-"));
  gtk_box_pack_start(GTK_BOX(hbox8), btnE, FALSE, TRUE, 0);

  hbox9 = gtk_hbox_new(TRUE, 3);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox9, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox9), 2);

  btnSW = gtk_button_new_with_mnemonic(_("/"));
  gtk_box_pack_start(GTK_BOX(hbox9), btnSW, FALSE, TRUE, 0);

  btnS = gtk_button_new_with_mnemonic(_("|"));
  gtk_box_pack_start(GTK_BOX(hbox9), btnS, FALSE, TRUE, 0);

  btnSE = gtk_button_new_with_mnemonic(_("\\"));
  gtk_box_pack_start(GTK_BOX(hbox9), btnSE, FALSE, TRUE, 0);

  hbox6 = gtk_hbox_new(TRUE, 5);
  gtk_box_pack_start(GTK_BOX(vbox3), hbox6, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox6), 5);

  btnPlay = gtk_toggle_button_new_with_mnemonic(_("play"));
  gtk_box_pack_start(GTK_BOX(hbox6), btnPlay, FALSE, FALSE, 0);

  btnStop = gtk_toggle_button_new_with_mnemonic(_("stop"));
  gtk_box_pack_start(GTK_BOX(hbox6), btnStop, FALSE, FALSE, 0);

  hscr = gtk_hscrollbar_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 0, 1, 5, 5)));
  gtk_box_pack_start(GTK_BOX(vbox3), hscr, TRUE, TRUE, 0);
  gtk_range_set_update_policy(GTK_RANGE(hscr), GTK_UPDATE_DISCONTINUOUS);

  labelFrame = gtk_label_new("0 / 0");
  gtk_box_pack_start(GTK_BOX (vbox3), labelFrame, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(labelFrame), GTK_JUSTIFY_LEFT);
}

void RemoveComboItems(GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy(widget);
}
