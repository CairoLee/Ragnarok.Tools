#ifndef __MY_FRAME_E__
#define __MY_FRAME_E__

#include <gtk/gtk.h>

#include "../../common/global.h"
#include "../actor.h"
#include "../main.h"
#include "buttons.h"

LIBGLADE_SIGNAL_FUNC void OpenDialogFrameEdit(GtkMenuItem *menuitem, gpointer user_data);

void FEDSetInitial(void);
void FEDSetInitial_Spin(void);
void FEDProcAny(void);
void FEDCheckMaxInEdit(void);

void FEResponse(GtkDialog *dialog, gint arg1, gpointer user_data);
void FEBuildDialog(GtkWidget *dialog);

#endif // __MY_FRAME_E__
