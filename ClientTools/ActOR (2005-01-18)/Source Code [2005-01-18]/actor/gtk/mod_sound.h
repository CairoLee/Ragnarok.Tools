#ifndef __MY_MOD_SOUND__
#define __MY_MOD_SOUND__

#include <gtk/gtk.h>

#include "../../common/global.h"
#include "../actor.h"
#include "../main.h"

LIBGLADE_SIGNAL_FUNC void OpenDialogModSound(GtkMenuItem *menuitem, gpointer user_data);

void MSDProcUpSel(GtkButton *button, gpointer user_data);
void MSDProcDownSel(GtkButton *button, gpointer user_data);
void MSDProcAddLast(GtkButton *button, gpointer user_data);
void MSDProcDelSel(GtkButton *button, gpointer user_data);

int MSDGetSelectedRow(void);
void MSDSetSelection(int row);

void MSDResponse(GtkDialog *dialog, gint arg1, gpointer user_data);
void MSDSetInitial(void);
void MSDBuildDialog(GtkWidget *dialog);

#endif  // __MY_MOD_SOUND__
