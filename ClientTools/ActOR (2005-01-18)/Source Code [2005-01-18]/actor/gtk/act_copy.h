#ifndef __MY_ACT_COPY__
#define __MY_ACT_COPY__

#include <gtk/gtk.h>

#include "../../common/global.h"
#include "../actor.h"
#include "../main.h"
#include "buttons.h"

LIBGLADE_SIGNAL_FUNC void OpenDialogActCopy(GtkMenuItem *menuitem, gpointer user_data);

void ACDSetInitial(void);
void ACDSetInitial_Combo(void);
void ACDSetInitial_Radio(int actno, bool isfrom);

void ACDDetailComboChange(GtkOptionMenu *optionmenu, gpointer user_data);
void ACDProcAny(void);
void ACDProcCopy4to8(bool cw);
void ACDProcCopyDetail(void);

void ACDBuildDialog(GtkWidget *dialog);
void ACDResponse(GtkDialog *dialog, gint arg1, gpointer user_data);

#endif  // __MY_ACT_COPY__
