#ifndef __MY_MAIN__
#define __MY_MAIN__

#include <libintl.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glade/glade.h>

#include "../common/global.h"
#include "actor.h"
#include "../common/vec.h"

#include "gtk/entry.h"
#include "gtk/frames.h"
#include "gtk/table.h"
#include "gtk/buttons.h"
#include "gtk/checkbox.h"
#include "gtk/radiobutton.h"
#include "gtk/combobox.h"
#include "gtk/scroll.h"
#include "gtk/table.h"
#include "gtk/frame_edit.h"
#include "gtk/mbutton_assign.h"
#include "gtk/act_copy.h"
#include "gtk/mod_sound.h"
#include "gtk/about.h"

#define FILTER_ACT (1 << 0)
#define FILTER_SPR (1 << 1)
#define FILTER_BMP (1 << 2)
#define FILTER_TGA (1 << 3)
#define FILTER_TXT (1 << 4)
#define FILTER_WAV (1 << 5)
#define FILTER_NONE (1 << 31)

extern CActor *actor;

extern GtkWidget *window;
extern GladeXML *windowXML;

extern MOUSE_ASSIGN mouseAssign;
extern DWORD colorBG, colorSelect, colorLine;
extern int idTimer;

LIBGLADE_SIGNAL_FUNC void AuditAll(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OpenDialog_ReadFile(GtkMenuItem *menuitem, gpointer definedFilename);
LIBGLADE_SIGNAL_FUNC void WriteAct(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void WriteActText(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void WriteBmp(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void WriteSpr(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ReplaceImage(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void Write1Image(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void InsertImage(GtkWidget *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void DeleteImage(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void SetSameColorUnusedPal(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ProcMenuNextFrame(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ProcMenuPrevFrame(GtkMenuItem *menuitem, gpointer user_data);
void SpritePan(BYTE key);
void SpriteZoom(bool equalRatio);
void SpriteRot(void);
void CameraPan(void);
void CameraZoom(void);
LIBGLADE_SIGNAL_FUNC void SelectColor(GtkWidget *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ChangeStateHLine(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ChangeStateVLine(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void Undo(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void Redo(GtkMenuItem *menuitem, gpointer user_data);
bool OnClose_Save(bool frommenu);
//void ShowPopupFrameImage(LPARAM lParam);


bool IsSprReady(void);
extern bool IsActReady(void);
void ProcAfterReadSPR_ACT(char *fn);
void CheckReference(char *fn);
extern void ProcPatternChange(bool drawAll);

void UndoRedoInit(void);
void DispUndoInitWarningFE(void);
bool DispUndoInitWarningSPO(void);
void UndoEnable(bool enable);
void RedoEnable(bool enable);
void BeginSpritePan(void);
void EndSpritePan(void);
void StoreSpritePan(void);
void BeginSpriteMag(void);
void EndSpriteMag(void);
void StoreSpriteMag(void);
void BeginSpriteRot(void);
void EndSpriteRot(void);
void StoreSpriteRot(void);
void BeginSpriteMirror(void);
void EndSpriteMirror(void);
void StoreSpriteMirror(void);
void BeginSpriteSaturation(void);
void EndSpriteSaturation(void);
void StoreSpriteSaturation(void);

bool GetFileName(char *fn, int fnlen, char *title, bool open, int filter);
void SetMousePoint(GdkEventButton *event);

LIBGLADE_SIGNAL_FUNC void CloseMainWindow(GtkWidget *widget, gpointer data);
LIBGLADE_SIGNAL_FUNC void CloseWindowFromMenu(GtkMenuItem *menuitem, gpointer user_data);
LIBGLADE_SIGNAL_FUNC gboolean KeyReleaseWindow(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
extern gboolean TimerFrameAll(gpointer data);
void ShowPopupMenu(GdkEventButton *event);

LIBGLADE_SIGNAL_FUNC gboolean configureOGL(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean exposeAllImg(GtkWidget *widget, GdkEventExpose *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean exposeFrmImg(GtkWidget *widget, GdkEventExpose *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean motionAllImg(GtkWidget *widget, GdkEventMotion *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean bpressAllImg(GtkWidget *widget, GdkEventButton *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean breleaseAllImg(GtkWidget *widget, GdkEventButton *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean keypressAllImg(GtkWidget *widget, GdkEventKey *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean keyreleaseAllImg(GtkWidget *widget, GdkEventKey *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean bpressFrmImg(GtkWidget *widget, GdkEventButton *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean breleaseFrmImg(GtkWidget *widget, GdkEventButton *event, gpointer data);

void SignalConnect(void);
void BuildWindow(void);
void BuildTable(GtkWidget *table, GtkListStore *store);

void SetTextDomain(char *progname);

bool GetAgreement(void);
void GetPrefFileName(char *fname, int fname_size);
void ReadPreference(void);
void WritePreference(void);

#endif // __MY_MAIN__

