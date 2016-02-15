#ifndef __MY_MBUTTON__
#define __MY_MBUTTON__

#include <gtk/gtk.h>

#include "../main.h"
#include "../../common/global.h"

#ifndef WIN32
#define MK_LBUTTON 0x01
#define MK_RBUTTON 0x02
#define MK_MBUTTON 0x10
#endif

LIBGLADE_SIGNAL_FUNC void OpenDialogMouseButtonAssign(GtkMenuItem *menuitem, gpointer user_data);

void MBABuildDialog(GtkWidget *dia);
void MBASetInitDialog(int LMR, GtkWidget *move, GtkWidget *rot, GtkWidget *zoom);
void MBAProcRadio(GtkWidget *button, gpointer user_data);
void MBASetState(void);

#endif // __MY_MBUTTON__
