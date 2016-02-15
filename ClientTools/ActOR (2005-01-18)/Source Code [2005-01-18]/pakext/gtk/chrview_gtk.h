#ifndef __MY_CHRVIEW_GTK__
#define __MY_CHRVIEW_GTK__

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "../chrview.h"
#include "../../common/pak.h"

extern int CharacterViewer(int argc, char **argv);

void ExitCharacterViewer(void);

void ChrVInitNFD(void);

void ChrVGLZoom(void);
void ChrVGLPan(void);

bool ChrVSpriteSetPCBody(CSprite **sprite, char *dispname);
bool ChrVSpriteSetPCHead(CSprite **sprite);
bool ChrVSpriteSetPCAcc(CSprite **sprite, char *dispname, int no);
bool ChrVSpriteSetPCWeapon(CSprite **sprite);
bool ChrVSpriteSetPCShield(CSprite **sprite);
void ChrVDrawSprites(void);

void ChrVSetScrollBar(int min, int max, int pos);

void ChrVInitControls(void);
void ChrVInitControlsSub(void);
void ChrVInitControlsHead(void);
void ChrVInitControlsBody(void);
void ChrVInitControlsBodyPC(void);
void ChrVInitControlsBodyMob(void);
void ChrVInitControlsBodyNPC(void);
void ChrVInitControlsAccPC(void);
void ChrVInitControlsActPC(void);
void ChrVInitControlsActMob(void);
void ChrVInitControlsActNPC(void);
void ChrVInitControlsWeaponShieldPre(void);
void ChrVInitControlsWeaponShield(void);

void ChrVSelectComboBody(GtkEntry *entry, gpointer user_data);
void ChrVSelectComboBody_PC(void);
void ChrVSelectComboAct(GtkEntry *entry, gpointer user_data);
void ChrVSelectComboHead(GtkEntry *entry, gpointer user_data);
void ChrVSelectComboAcc(GtkEntry *entry, gpointer user_data);
void ChrVSelectComboWeapon(GtkEntry *entry, gpointer user_data);
void ChrVSelectComboShield(GtkEntry *entry, gpointer user_data);
void ChrVButtonPC(GtkButton *button, gpointer user_data);
void ChrVButtonMob(GtkButton *button, gpointer user_data);
void ChrVButtonNPC(GtkButton *button, gpointer user_data);
void ChrVButtonMale(GtkButton *button, gpointer user_data);
void ChrVButtonFemale(GtkButton *button, gpointer user_data);
void ChrVButtonPlay(GtkButton *button, gpointer user_data);
void ChrVButtonStop(GtkButton *button, gpointer user_data);
void ChrVButtonViewReset(GtkButton *button, gpointer user_data);
void ChrVButtonDirection(GtkButton *button, gpointer user_data);
void ChrVScrAdjustBounds(GtkRange *range, gdouble arg1, gpointer user_data);
gboolean ChrVScrRelease(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

void ChrVEnableComboAlmost(bool flag);
void ChrVGetPCBodyName(char *bodyname, char *sexname);

void ChrVSetBGColor(GtkButton *button, gpointer user_data);

gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data);

gint ChrVCloseDialog(GtkWidget *widget, GdkEvent *event, gpointer data);
void ChrVCloseDialogBtn(GtkButton *button, gpointer data);
gboolean ChrVTimerProc(gpointer data);

void ChrVSignalConnect(void);
void ChrVBuildWindow(void);

void RemoveComboItems(GtkWidget *widget, gpointer data);

#endif  // __MY_CHRVIEW_GTK__

