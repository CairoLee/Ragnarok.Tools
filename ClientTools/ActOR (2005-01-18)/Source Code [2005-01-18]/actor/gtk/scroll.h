#ifndef __MY_SCROLL__
#define __MY_SCROLL_

#include <gtk/gtk.h>

#include "../main.h"
#include "../../common/global.h"

int GetScrollImgPos(void);
void SetScrollPos_(GtkWidget *scr, int pos, int max);

LIBGLADE_SIGNAL_FUNC void AdjustBoundsScrFrame(GtkRange *range, gdouble arg1, gpointer user_data);
LIBGLADE_SIGNAL_FUNC gboolean ReleaseScrFrame(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void ChangedScrImage(GtkRange *range, gpointer user_data);

#endif  // __MY_SCROLL__

