#ifndef __MY_COMBOBOX__
#define __MY_COMBOBOX__

#include <gtk/gtk.h>

#include "../main.h"
#include "../../common/global.h"

LIBGLADE_SIGNAL_FUNC void OnComboActChange(GtkOptionMenu *optionmenu, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnComboSoundChange(GtkOptionMenu *optionmenu, gpointer user_data);

#endif  // __MY_COMBOBOX__

