#ifndef __MY_BUTTONS__
#define __MY_BUTTONS__

#include <gtk/gtk.h>

#include "../main.h"

extern bool autoPlay;

void SetStateDirecButton(BYTE stat);
void SetStateAutoPlay(void);
void EnableNeighDirWindow(gboolean sw);

LIBGLADE_SIGNAL_FUNC void OnBtnDirectClick(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnPlay(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnStop(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnViewReset(GtkButton *button, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnNeiDirec(GtkButton *button, gpointer user_data);

#endif  // __MY_BUTTONS__

