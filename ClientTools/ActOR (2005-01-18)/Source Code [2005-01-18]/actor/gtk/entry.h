#ifndef __MY_ENTRY__
#define __MY_ENTRY__

#include <gtk/gtk.h>

#include "../main.h"

void SetInterval(int inter);

LIBGLADE_SIGNAL_FUNC void OnEntryIntervalActivate(GtkEntry *entry, gpointer user_data);

#endif // __MY_ENTRY__

