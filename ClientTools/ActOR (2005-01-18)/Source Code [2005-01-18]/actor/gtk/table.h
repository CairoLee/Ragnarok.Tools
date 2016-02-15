#ifndef __MY_TABLE__
#define __MY_TABLE__

#include <gtk/gtk.h>

#include "../main.h"
#include "../actor.h"

enum
{
  SPRNO_COLUMN,
  SPTYPE_COLUMN,
  XOFFS_COLUMN,
  YOFFS_COLUMN,
  MIRROR_COLUMN,
  SAT_COLUMN,
  XMAG_COLUMN,
  YMAG_COLUMN,
  ROT_COLUMN,
  EDITABLE_COLUMN,
  N_COLUMNS
};

DWORD HextToDWORD(char *hex);
BYTE SaturateByte5(BYTE orig, bool plus);
void GetSelectedTableItem(int *sel);
void SetTableFromEdit(char *str, int row, int column);
void SetTableItemAll(int noAct, int noPat);
void SetTableItem(char *txt, int row, int column);
void SwapTableItemText(int max, int rowA, int rowB);
void DownSelectedTableItem(int num);
void UpSelectedTableItem(int num);
void SwapTableItemText(int max, int rowA, int rotB);

void DeleteSelectedSpriteInTable(void);
void UnSelectSelection(void);
void SelectUnselectRow(int selno, int unselno);

extern void EditedTable(GtkCellRendererText *cell, const gchar *path_string, const gchar *new_test, gpointer data);

LIBGLADE_SIGNAL_FUNC void ChangeSaturationABGRValue(GtkWidget *menuitem, gpointer data);
LIBGLADE_SIGNAL_FUNC void ChangeMirror(GtkMenuItem *menuitem, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean motionTable(GtkWidget *widget, GdkEventMotion *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean bpressTable(GtkWidget *widget, GdkEventButton *event, gpointer data);
LIBGLADE_SIGNAL_FUNC gboolean breleaseTable(GtkWidget *widget, GdkEventButton *event, gpointer data);

#endif  // __MY_TABLE__

