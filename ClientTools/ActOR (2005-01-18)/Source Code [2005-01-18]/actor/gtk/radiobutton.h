#ifndef __MY_RADIOBUTTON__
#define __MY_RADIOBUTTON__

#include <gtk/gtk.h>

#include "../main.h"

LIBGLADE_SIGNAL_FUNC void OnRBtnHead(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnRBtnBody(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnRBtnEtc(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnRBtnNeigh(GtkButton *button, gpointer user_data);

#endif // __MY_RADIOBUTTON__

